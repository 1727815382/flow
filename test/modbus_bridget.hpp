#include <QApplication>
#include <QJsonDocument>
#include "core/task_manager.hpp"
#include "motor/motor_control_task.hpp"
#include "motor/plc_command_task.hpp"
#include "motor/motor_state_task.hpp"
#include "config/device_config_loader_legacy.hpp"
#include "net/modbus_bridge.hpp"
#include "net/udp_task.hpp"
#include "net/ftp_task.hpp"
#include "measure/measure_task.hpp"
#include "ui/main_window.hpp"

// 你的 Sequencer 头（项目已有）
#include "network/modbussequencer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 1) 创建任务
    auto* ctrlTask  = new gpxm::MotorControlTask();
    auto* cmdTask   = new PlcCommandTask();
    auto* stateTask = new MotorStateTask();
    auto* udpTask   = new UdpTask();
    auto* ftpTask   = new FtpTask();
    auto* measTask  = new MeasureTask();

    TaskManager::instance().addTask(ctrlTask);
    TaskManager::instance().addTask(cmdTask);
    TaskManager::instance().addTask(stateTask);
    TaskManager::instance().addTask(udpTask);
    TaskManager::instance().addTask(ftpTask);
    TaskManager::instance().addTask(measTask);
    TaskManager::instance().startAll();

    // 2) 构建 ModbusSequencer（按你的工程方式初始化；下行仅示意）
    auto* sequencer = new ModbusSequencer(&app);
    // 如果需要设置连接参数、线程或调度策略，在此补：例如
    // sequencer->setServer("192.168.6.200", 502);
    // sequencer->start();   // 具体以你工程为准

    // 3) 用桥接器适配 sequencer → 提供统一 read/write + readDone 信号
    auto* bridge = new ModbusBridge(&app);
    bridge->setSequencer(sequencer); // 自动连接读完成信号（若命名不匹配会有日志警告）

    // 4) 读取你的旧配置 [PLCDevices] 并注册所有设备（30×4）
    DeviceConfigLoaderLegacy loader("devices.ini");
    // 若想改 unitId 策略为“001..120 直接作为电机 unitId”，解除下一行注释：
    // loader.setMotorUnitIdMapper([](int, int, int, const QString& secStr){ return secStr.toInt(); });

    const auto summary = loader.build(cmdTask, stateTask, bridge);
    qInfo().noquote() << "Device registry summary:"
                      << QJsonDocument(summary).toJson(QJsonDocument::Compact);

    // 5) UI
    MainWindow w; w.resize(1200, 800); w.show();

    return app.exec();
}
