// #include <QApplication>
// #include <QTimer>
// #include <QJsonObject>

// #include "core/message_bus.hpp"
// #include "core/task_manager.hpp"
// #include "core/scheduler_task.hpp"
// #include "core/supervisor.hpp"
// #include "core/schema_registry.hpp"

// #include "modules/log_service/log_service.hpp"
// #include "modules/udp_service/udp_task.hpp"
// #include "modules/metrics_service/metrics_task.hpp"
// #include "modules/replay_service/replay_task.hpp"

// #include "ui/shell_window.hpp"
// #include "ui/ui_bridge.hpp"
// #include "ui/ui_adapter_task.hpp"
// #include "ui/builtin_views/view_supervisor.hpp"
// #include "ui/builtin_views/view_udp_console.hpp"

// static void sched_fixed_rate(MessageBus& bus, int period_ms,
//                              const char* to, const char* type, const QJsonObject& payload)
// {
//     Message job;
//     job.header.to   = "scheduler";
//     job.header.type = "sched.add.fixedrate";
//     QJsonObject msg; msg.insert("_to", to); msg.insert("_type", type); msg.insert("payload", payload);
//     QJsonObject p; p.insert("period_ms", period_ms); p.insert("msg", msg);
//     job.payload = p;
//     bus.sendTo("scheduler", job);
// }

// int main(int argc, char** argv) {
//     QApplication app(argc, argv);

//     MessageBus bus;

//     // core
//     std::shared_ptr<TaskManager>    taskmgr(new TaskManager(&bus));
//     std::shared_ptr<SchedulerTask>  sched(new SchedulerTask(&bus));
//     std::shared_ptr<Supervisor>     superv(new Supervisor(&bus));
//     std::shared_ptr<SchemaRegistry> schema(new SchemaRegistry(&bus));
//     std::shared_ptr<LogService>     log(new LogService(&bus));
//     std::shared_ptr<MetricsTask>    metrics(new MetricsTask(&bus));
//     std::shared_ptr<ReplayTask>     replay(new ReplayTask(&bus));
//     bus.registerTask(taskmgr); bus.registerTask(sched); bus.registerTask(superv);
//     bus.registerTask(schema);  bus.registerTask(log);   bus.registerTask(metrics);
//     bus.registerTask(replay);
//     taskmgr->start(); sched->start(); superv->start(); schema->start();
//     log->start(); metrics->start(); replay->start();

//     // UDP
//     std::shared_ptr<UdpTask> udp(new UdpTask(&bus, 9000));
//     bus.registerTask(udp); udp->start();
//     // 自发自收 peer（含心跳）
//     {
//         Message p; p.header.to="udp"; p.header.type="udp.peer.add";
//         p.payload = QJsonObject{
//             {"ip","127.0.0.1"}, {"port",9000}, {"heartbeat_ms", 1000},
//             {"cfg", QJsonObject{{"ack_timeout_ms",400},{"max_retries",3},{"backoff_ms_init",200},{"backoff_ms_max",800}}}
//         };
//         bus.sendTo("udp", p);
//     }

//     // UI bridge & adapter
//     ShellWindow w; w.setBus(&bus);
//     UiBridge bridge(&w);
//     std::shared_ptr<UiAdapterTask> uiTask(new UiAdapterTask(&bus, &bridge));
//     bus.registerTask(uiTask); uiTask->start();

//     // UI 适配收到并转发的主题
//     bus.subscribe(uiTask->id(), "udp.recv");
//     bus.subscribe(uiTask->id(), "udp.ack");
//     bus.subscribe(uiTask->id(), "udp.drop");
//     bus.subscribe(uiTask->id(), "udp.socket");
//     bus.subscribe(uiTask->id(), "log.event");
//     bus.subscribe(uiTask->id(), "metrics.summary");
//     bus.subscribe(uiTask->id(), "ui.nav");
//     bus.subscribe(uiTask->id(), "replay.state");

//     // Replay 也订阅这些主题（用于录制   —— 录什么回放什么）
//     bus.subscribe("replay", "udp.recv");
//     bus.subscribe("replay", "udp.ack");
//     bus.subscribe("replay", "udp.drop");
//     bus.subscribe("replay", "udp.socket");
//     bus.subscribe("replay", "log.event");
//     bus.subscribe("replay", "metrics.summary");
//     bus.subscribe("replay", "ui.nav");

//     // Views
//     w.addView("Supervisor",  new ViewSupervisor(&w, &bus, &bridge));
//     w.addView("UDP Console", new ViewUdpConsole(&w, &bus, &bridge));

//     // Quick demos：开机写一条日志 + 刷一次 metrics + 发一帧可靠 UDP + 定时 metrics
//     QTimer::singleShot(200, [&bus](){
//         Message m; m.header.type="log.write"; m.header.to="log";
//         m.payload = QJsonObject{{"level","info"},{"tag","ui"},{"msg","App started"}};
//         bus.sendTo("log", m);
//     });
//     QTimer::singleShot(400, [&bus](){
//         Message m; m.header.type="metrics.flush"; m.header.to="metrics";
//         bus.sendTo("metrics", m);
//     });
//     sched_fixed_rate(bus, 3000, "metrics", "metrics.flush", QJsonObject());
//     QTimer::singleShot(800, [&bus](){
//         Message s; s.header.to="udp"; s.header.type="udp.send";
//         s.payload = QJsonObject{{"ip","127.0.0.1"},{"port",9000},{"reliable",true},{"data_hex","48656C6C6F20554450"}};
//         bus.sendTo("udp", s);
//     });

//     w.resize(1100, 700);
//     w.show();
//     return app.exec();
// }


#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include "core/message_bus.hpp"
#include "core/task_manager.hpp"
#include "core/scheduler_task.hpp"
#include "modules/log_service/log_service.hpp"
#include "modules/udp_service/udp_task.hpp"

#include "modules/datahub/datahub_task.hpp"
#include "modules/proto_engine/protocol_engine_task.hpp"
#include "ui/ui_bridge.hpp"
#include "ui/ui_adapter_task.hpp"
#include "ui/builtin_views/view_protocol_console.hpp"

int main(int argc, char** argv){
    QApplication app(argc, argv);

    // —— 消息总线 —— //
    MessageBus bus;

    // —— 基础任务 —— //
    std::shared_ptr<SchedulerTask> sched(new SchedulerTask(&bus));
    std::shared_ptr<LogService> log(new LogService(&bus));
    std::shared_ptr<UdpTask> udp(new UdpTask(&bus, 9000)); // 监听 9000
    std::shared_ptr<DataHubTask> datahub(new DataHubTask(&bus));
    std::shared_ptr<ProtocolEngineTask> proto(new ProtocolEngineTask(&bus));

    bus.registerTask(sched);  sched->start();
    bus.registerTask(log);    log->start();
    bus.registerTask(udp);    udp->start();
    bus.registerTask(datahub);datahub->start();
    bus.registerTask(proto);  proto->start();

    // —— UI 桥 —— //
    UiBridge bridge;
    std::shared_ptr<UiAdapterTask> uiTask(new UiAdapterTask(&bus, &bridge));
    bus.registerTask(uiTask); uiTask->start();

    // —— UI 窗口 —— //
    QMainWindow win;
    ViewProtocolConsole* view = new ViewProtocolConsole(&win, &bus, &bridge);
    win.setCentralWidget(view);
    win.resize(1100, 700);
    win.show();

    // —— 自动加载示例配置 —— //
    {
        Message m; m.header.type="proto.config.load"; m.header.to="proto";
        m.payload = QJsonObject{{"file","E:/work/github_proj/flow/configs/proto_demo.json"}};
        bus.sendTo("proto", m);
    }
    // —— 设置一些初始 DataHub 值 —— //
    {
        Message m; m.header.type="datahub.bulk"; m.header.to="datahub";
        QJsonArray items;
        items.push_back(QJsonObject{{"key","ui.mode"},{"value","auto"}});
        items.push_back(QJsonObject{{"key","ui.speed"},{"value", 42}});
        m.payload = QJsonObject{{"items", items}};
        bus.sendTo("datahub", m);
    }

    return app.exec();
}
