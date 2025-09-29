// #include <QApplication>
// #include <QJsonDocument>
// #include "core/task_manager.hpp"
// #include "motor/motor_control_task.hpp"
// #include "motor/plc_command_task.hpp"
// #include "motor/motor_state_task.hpp"
// #include "config/device_config_loader_legacy.hpp"
// #include "net/modbus_bridge.hpp"
// #include "net/udp_task.hpp"
// #include "net/ftp_task.hpp"
// #include "measure/measure_task.hpp"
// #include "ui/main_window.hpp"

// // 你项目已有：
// #include "network/modbussequencer.h"

// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);

//     // 1) 任务
//     auto* ctrlTask  = new gpxm::MotorControlTask();
//     auto* cmdTask   = new PlcCommandTask();
//     auto* stateTask = new MotorStateTask();
//     auto* udpTask   = new UdpTask();
//     auto* ftpTask   = new FtpTask();     // 使用 Qt5Ftp（qftp.h）
//     auto* measTask  = new MeasureTask();

//     TaskManager::instance().addTask(ctrlTask);
//     TaskManager::instance().addTask(cmdTask);
//     TaskManager::instance().addTask(stateTask);
//     TaskManager::instance().addTask(udpTask);
//     TaskManager::instance().addTask(ftpTask);
//     TaskManager::instance().addTask(measTask);
//     TaskManager::instance().startAll();

//     // 2) Sequencer（按你的工程方式初始化——如有 IP/端口/线程设置在此补上）
//     auto* sequencer = new ModbusSequencer(&app);
//     // 例如（请按你工程真实接口补充/修改）：
//     // sequencer->setServer("192.168.6.200", 502);
//     // sequencer->start();

//     // 3) 桥接 Sequencer
//     auto* bridge = new ModbusBridge(&app);
//     bridge->setSequencer(sequencer); // 自动连接 read-done 信号（若命名不同会报警）

//     // 4) 读取旧配置并注册设备（30×4），并把桥接的读写/回调注入设备
//     DeviceConfigLoaderLegacy loader("devices.ini");
//     // 如需用 "001..120" 直接作为 motor unitId，可启用下一行：
//     // loader.setMotorUnitIdMapper([](int, int, int, const QString& secStr){ return secStr.toInt(); });

//     const auto summary = loader.build(cmdTask, stateTask, bridge);
//     qInfo().noquote() << "Device registry summary:"
//                       << QJsonDocument(summary).toJson(QJsonDocument::Compact);

//     // 5) UI
//     MainWindow w; w.resize(1200, 800); w.show();
//     return app.exec();
// }


