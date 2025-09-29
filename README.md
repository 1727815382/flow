一个用于 Qt/C++11 桌面工具与设备/协议调试场景的轻量级任务框架。它把网络、协议编解码、数据共享与 UI 解耦，支持“可配置协议引擎+可靠 UDP 叠加层+任务化并发+优先级消息队列+Ask/Reply 请求-应答+可视化日志与指标”。

目标：降低锁和跨线程复杂度，让多人并行开发变简单，同时保证线上稳定性与可测试性。

亮点特性

消息总线（MessageBus）：任务按 type/to/topic 路由；线程安全；弱引用注册；转发在锁外完成，避免 use-after-free。

任务基类（TaskBase）：每个任务 1 个工作线程 + 心跳；优先级队列（高/中/低）+ 背压策略（丢前、丢后、限时阻塞，可配置）。

Ask/Reply（相关性 ID）：correlation_id 与 reply_to 语义完整；挂起表+超时；取消/超时会上报 supervisor.exception。

可靠 UDP（UR 子层）：

报文头 UR|v1|flags|msgid|payload；ACK + 指数退避；自适应超时（基于 RTT）

乱序/重复/过期 保护：每 peer 维护滑动窗口/去重；重启不撞号（纪元戳 | 计数器）

ProtocolEngine（协议引擎）：

匹配：all/any、mask、范围

字段：u8/u16/u32/i16/i32/bytes/string(fix/len-from)…

校验：CRC16(Modbus/CCITT)/CRC32/SUM8/XOR8（LE/BE 可配）

动作：publish/reply/timer/state.set、行为树（Wait/Loop/Parallel/Race/Guard）、bt.exec 子树

模板组包：可表达式 ${...}、可嵌入 checksum

DataHub（全局数据中心）：读多写少场景下使用读写锁；支持单点/批量更新；UI 与引擎通过它解耦。

UI 适配（UiBridge/UiAdapterTask）：后台只与总线交互；永不直接触摸 QWidget；invokeMethod(Qt::QueuedConnection) 保证 UI 线程安全。

日志/指标：

proto.debug/udp.*/supervisor.* 等可视化；HEX 输出节流+长度上限

任务/队列/UDP/引擎的计数器与直方图（队列水位、drop、retry、RTT）

配置热加载与校验：

基础 schema 校验（例如 len_from 引用存在性）

失败定位 messages[i].fields[j]...；可选文件监视热重载

库化与工程化：

纯 C++11 + Qt5.6+；Windows(MSVC/MinGW) / Linux

支持静/动态库，FRAMEWORK_EXPORT 跨编译器导出；可 add_subdirectory 或 find_package 复用

CMake 最低 3.5；可选 Qt5::Sql（DB 模块）

目录结构（核心）
core/
  message.hpp / message_bus.{hpp,cpp}
  task_base.{hpp,cpp}
  itask.hpp
  ts_prio_queue.{hpp,cpp}
  task_manager.{hpp,cpp}
  metrics_task.cpp (可选)

modules/
  udp_service/
    udp_task.{hpp,cpp}
    udp_socket.{hpp,cpp}
  proto_engine/
    protocol_engine_task.{hpp,cpp}
    field_codec.{hpp,cpp}
    expr_eval.{hpp,cpp}
    bt.{hpp,cpp}
  datahub/
    datahub_task.{hpp,cpp}
  replay_service/ (可选)
  log_service/ (可选)

ui/
  ui_bridge.{hpp,cpp}
  ui_adapter_task.{hpp,cpp}
  builtin_views/...
  mainwindow.cpp (仅示例)

app/
  main.cpp (仅示例，可删除；框架以库形式交付)

运行时模型

每个任务拥有自己的 MPSC 优先级队列：TSPrioQueue(max_size, policy)

policy：drop_oldest / drop_newest / block_timeout_ms

超限会计数并上报 supervisor.queue.drop

MessageBus：

registerTask / unregisterTask / subscribe / publish / sendTo

转发时先把 shared_ptr<ITask> 复制到栈上，再出锁 post()，避免在锁里调用用户代码

Ask/Reply：

TaskBase::ask(to, type, payload, timeout) 返回 std::future<Message>

收到带 correlation_id 的应答时唤醒；超时/取消会清理挂起表并上报异常

计时器：

统一定时轮（单线程，最小堆），scheduleTimer(delay_ms, repeat, action, ctx)

任务 stop() 将显式唤醒并回收，不使用 detach()，避免回调越界

关键模块
Message & Header
enum MsgPrio { PRIO_HIGH=0, PRIO_NORM=1, PRIO_LOW=2 };

struct MsgHeader {
  std::string id;             // 生成的消息ID
  std::string type;           // 业务类型，如 "udp.recv"
  std::string version{"1.0"};
  std::string from, to;       // 发送者/指定接收者
  std::string topic;          // 发布订阅主题
  std::string correlation_id; // 请求-应答相关性
  std::string reply_to;       // 指定应答目标
  long long   ts_epoch_ms{0};
  int         priority{PRIO_NORM};
};

struct Message { MsgHeader header; QJsonObject payload; };

TaskBase（线程/队列/Ask/心跳）

start()/stop() 生命周期

post(Message) 入队

ask(...) 封装请求-应答（挂起表+超时）

heartbeatLoop() 周期上报 supervisor.status

UdpTask（含 UR 可靠层）

多线程成员（rxThread_ + txThread_），保护容器：

peers_、pendings_ 分别用独立 std::mutex

txLoop 遍历时，先复制待触发的 key 到本地数组，再逐个处理，避免 erase 迭代器失效

去重与窗口：每 peer 保存 last_recv_msgid + bitset 滑窗

msgid：(epoch_ms << 12) | seq，重启不撞号

指标：udp.send.retry / udp.drop / udp.ack / udp.peer.up/down / rtt

日志节流：上报给 UI 的 data_hex 可截断（按配置长度）

ProtocolEngineTask（引擎）

proto.config.load 载入 JSON；失败精确定位

udp.recv → 匹配（mask/all/any）→ 字段解码 → 触发 rules

动作：

publish：替换 ${expr} 后执行 bus.publish

reply：按模板 compose + checksum → 发送到 udp

timer：进入统一定时轮

state.set：更新 DataHub/会话态

bt.exec：执行子行为树

字段/校验：u8/u16/u32/i16/i32/bytes/string.fix/string.from，CRC16/CRC32/SUM8/XOR8…

行为树（bt）：Wait/Loop/Parallel/Race/Guard/Action，自定义节点可映射到 execAction

DataHubTask（全局数据）

QReadWriteLock 保护，读多写少

datahub.set / datahub.bulk / datahub.get

可扩展：CAS 更新、快照导入/导出（配合回放）

UI（UiBridge/UiAdapterTask）

白名单订阅：ui.* / proto.debug / udp.* / supervisor.* / datahub.value

UiBridge::uiMessage(QString, QJsonObject) 信号下发到各界面视图

任何后台线程 → UI 的操作都通过 invokeMethod(..., Qt::QueuedConnection)

构建与集成
依赖

CMake ≥ 3.5

C++11 编译器（MSVC 2015+/GCC 5+/Clang 3.8+）

Qt ≥ 5.6（Core、Widgets；可选 Sql）

作为库集成（推荐两种）

A. 同仓库并行（最简）

# your_app/CMakeLists.txt
add_subdirectory(../qt_task_framework ${CMAKE_BINARY_DIR}/qt_task_framework_build)
add_executable(your_app main.cpp)
target_link_libraries(your_app PRIVATE qt_task_framework Qt5::Core Qt5::Widgets)
target_include_directories(your_app PRIVATE ../qt_task_framework)


B. 安装后 find_package（工程化）

框架安装时会生成 qt_task_frameworkConfig.cmake：

find_package(qt_task_framework CONFIG REQUIRED)
add_executable(your_app main.cpp)
target_link_libraries(your_app PRIVATE qt_task_framework::qt_task_framework)


Windows/MinGW/MSVC 均支持。导出宏 FRAMEWORK_EXPORT 已处理跨编译器符号可见性。

快速上手（最小示例）
// main.cpp
QApplication app(argc, argv);

MessageBus bus;
auto udp = std::make_shared<UdpTask>(&bus, 9000);
auto proto = std::make_shared<ProtocolEngineTask>(&bus);
auto hub = std::make_shared<DataHubTask>(&bus);

bus.registerTask(udp);    udp->start();
bus.registerTask(proto);  proto->start();
bus.registerTask(hub);    hub->start();

// 加载协议配置
Message m; m.header.type="proto.config.load";
m.payload = QJsonObject{{"file","configs/proto_demo.json"}};
bus.publish("proto.config.load", m);

// 发送一帧
Message s; s.header.type="udp.send"; s.header.to="udp";
s.payload = QJsonObject{
  {"ip","127.0.0.1"}, {"port",9000},
  {"reliable",false}, {"data_hex","01 00 2A 00 03 AA BB CC"}
};
bus.sendTo("udp", s);

return app.exec();


configs/proto_demo.json（骨架）：

{
  "options": { "debug": true, "hex_max": 128 },
  "bindings": [{
    "id":"b1", "udp_task":"udp", "listen_port":9000,
    "peers":[{"ip":"127.0.0.1","port":9000}]
  }],
  "messages": [{
    "id":"DemoPing", "binding":"b1",
    "match":{"all":[{"offset":0,"equals_hex":"01"}]},
    "fields":[
      {"name":"cmd","type":"u8","offset":0},
      {"name":"seq","type":"u16","offset":1,"endian":"be"}
    ]
  },{
    "id":"DemoRespTemplate",
    "compose":[
      {"type":"u8","value_hex":"02"},
      {"type":"u16","endian":"be","expr":"${msg.seq}"}
    ]
  }],
  "rules": [{
    "id":"r1","when":"on_receive","message":"DemoPing","binding":"b1",
    "actions":[{"type":"reply","args":{"template":"DemoRespTemplate","to_peer":"source"}}]
  }]
}

线程安全与性能策略

容器上锁：

UdpTask：peers_、pendings_ 分锁；遍历先复制 key，再处理与 erase

MessageBus：注册表/订阅表有锁，post 在锁外

背压：每任务队列 max_size 可配；drop_oldest 推荐用于高吞吐日志

日志节流：HEX 输出长度上限 + 采样节流

定时器：统一时间轮，无 detach 线程

零拷贝（边界内）：内部传递 QByteArray/原始 buffer，UI/日志才做十六进制

监控与排障

指标主题：metrics.*（如 metrics.queue.size, metrics.udp.rtt）

关键事件：udp.socket.* / udp.peer.* / udp.send.retry / udp.drop / proto.debug

常见编译/链接问题：

undefined reference to vtable：类含 Q_OBJECT 时需开启 AUTOMOC

msvc/MinGW 符号导出：确认 FRAMEWORK_EXPORT 宏在公有头文件中

测试与回归

Loopback：本机 127.0.0.1 端到端自测（DemoPing → Decode → Reply → RTT）

Fuzz：对 decodeFields/composeFromTemplate 做随机输入模糊测试

回放/虚拟时钟（可选）：录制 udp.recv/proto.debug，重播时走虚拟时钟（0.5x/2x）

Roadmap（已纳入设计/可选开启）

IPv6 / 组播 / 广播

批量 ACK 与 piggyback

大包分片/重组

命令行工具：抓包导出（pcap）、热重载、队列诊断

版本与兼容

当前版本：v1.0.2

ABI：以主版本为单位（SOVERSION 1）
