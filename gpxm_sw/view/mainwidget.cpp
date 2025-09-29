#include "loginwidget.h"
#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "flow/devicemanager.h"
#include "flow/netdataprocessing.h"
#include "flow/precisiondataprocessing.h"
#include "Windows.h"
#include <QKeyEvent>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    this->setWindowTitle("轨道精调系统");

    //更新小车测量数据
    NetDataProcessing* netPrc = new NetDataProcessing();
    connect (netPrc, SIGNAL(signMesureUpdate(TrackMesure)), ui->workWgt, SLOT(slotMesureUpdate(TrackMesure)));

    //更新plc和motor状态
    DeviceManager* dm = new DeviceManager();
    dm->initDevCfg();
    connect (dm, SIGNAL(signMotorUpdate(MotorResponse&)), ui->DataWgt, SLOT(slotUpdateMotor(MotorResponse&))); //电机状态
    connect (dm, SIGNAL(signPlcUpdate(PlcResponse&)),     ui->DataWgt, SLOT(slotUpdatePlc(PlcResponse&))); //plc状态
    connect (dm, SIGNAL(signCannotWorkMessage(MotorStatus)),ui->DataWgt, SLOT(slotCannotWorkMessage(MotorStatus))); //工作繁忙和故障提示
    connect (dm, SIGNAL(signMotorFoldDoState (int&)),     ui->DataWgt, SLOT(slotMotorFoldDoState (int&)));

    connect (ui->DataWgt, SIGNAL(signCtrlClicked(QString,double)), dm, SLOT(slotCtrlClicked(QString,double))); //控制按钮
    connect (ui->DataWgt, SIGNAL(signSectionSelected(int)), dm, SLOT(slotSectionSelected(int))); //短面选择
    connect (ui->DataWgt, SIGNAL(signFoldDoClicked(QString,double)), dm, SLOT(slotFoldDoClicked(QString, double))); //支腿压实
    connect (ui->SysCfgWgt, SIGNAL(signCtrlClicked(QString ,int , int )), dm, SLOT(slotCtrlClicked(QString ,int , int ))); //控制按钮

    connect (dm, SIGNAL(signMotorSettingStatus(MotorSettingStatus)), ui->SysCfgWgt, SLOT(slotMotorSettingStatus(MotorSettingStatus)));
    connect (dm, SIGNAL(signMotorSettingStatus(MotorSettingStatus)), ui->DataWgt,   SLOT(slotMotorSettingStatus(MotorSettingStatus)));

    //    connect (ui->workWgt, SIGNAL(signMesureSelected(int), dm, SLOT(slotMesureSelected(int))); //选中断面
    connect (ui->workWgt, SIGNAL(signDealSection(int)),               dm, SLOT(slotDealSection(int))); //精调断面
    connect (ui->workWgt, SIGNAL(signRowSelected(int)),            dm, SLOT(slotRowSelected(int))); //选中测量数据
    connect (ui->workWgt, SIGNAL(signMenuAddMesure(TrackMesure&)), dm, SLOT(slotMenuAddMesure(TrackMesure&)));
    connect (ui->workWgt, SIGNAL(signMenuMdyMesure(TrackMesure&)), dm, SLOT(slotMenuMdyMesure(TrackMesure&)));
    connect (ui->workWgt, SIGNAL(signMenuDelMesure()),             dm, SLOT(slotMenuDelMesure()));
    connect (ui->workWgt, SIGNAL(signMenuClrMesure()),             dm, SLOT(slotMenuClrMesure()));
    connect (ui->workWgt, SIGNAL(signSingleMesure(int)),           dm, SLOT(slotSingleMesure(int))); //逐枕测量
    connect (ui->workWgt, SIGNAL(signAutoMesure(int)),             dm, SLOT(slotAutoMesure(int))); //逐枕测量
    connect (ui->workWgt, SIGNAL(signStopMesure()),                dm, SLOT(slotStopMesure())); //停止测量
//    connect (ui->workWgt, SIGNAL(signDataCheck()),                 dm, SLOT(slotDataCheck())); //当前使用王关征算法，舍弃这里

    connect (dm, SIGNAL(signSelectedMesure(TrackMesure&)),   ui->workWgt, SLOT(slotSelectedMesure(TrackMesure&))); //选中数据更新到界面上
    connect (dm, SIGNAL(signOldMesure (TrackMesure&)),       ui->workWgt, SLOT(slotOldMesure(TrackMesure&))); //旧数据刷新
    connect (dm, SIGNAL(signMesureRemoved(int)),             ui->workWgt, SLOT(slotMesureRemoved(int))); //通知界面删除数据
    connect (dm, SIGNAL(signMesureUpdated(int,TrackMesure&)),ui->workWgt, SLOT(slotMesureUpdate(int,TrackMesure&))); //通知界面删除数据
    connect (dm, SIGNAL(signMesureUpdate(TrackMesure)),      ui->workWgt, SLOT(slotMesureUpdate(TrackMesure)));

    connect (dm, SIGNAL(signShowInfo(QString)),     ui->workWgt,   SLOT(slotShowErrInfo(QString))); //异常信息显示
    connect (dm, SIGNAL(signShowInfoToSys(int, QString)),     ui->SysCfgWgt, SLOT(slotShowInfo(int, QString))); //异常信息显示
    connect (dm, SIGNAL(signShowInfoToData(int,QString)),     ui->DataWgt,   SLOT(slotShowInfo(int,QString))); //异常信息显示

    connect (dm, SIGNAL(signMotorRunOver()),        ui->workWgt, SLOT(slotMotorRunOver())); //处理精调完成信号



    //    connect (netPrc, SIGNAL(signMesureUpdate(TrackMesure), dm, SLOT(slotMesureUpdate(TrackMesure))); //测量数据，保存一份数据

    //轨排数据界面切换轨排
    connect (ui->DataWgt, SIGNAL(signPlcSelected(int)), dm, SLOT (slotUpdatePlcSelected(int)));


    ui->workWgt->setObjectName("MainWidget");
    ui->ConfigWgt->setObjectName("MainWidget");
    ui->DataWgt->setObjectName("MainWidget");
    ui->TuingWgt->setObjectName("MainWidget");
    ui->tabWidget->removeTab(4);

    // 基础窗口设置 - 确保窗口能正常显示
    setWindowFlags(
        Qt::Window |                  // 基础窗口属性
        Qt::WindowTitleHint |         // 显示标题栏
        Qt::WindowCloseButtonHint |   // 保留关闭按钮
        Qt::MSWindowsFixedSizeDialogHint  // 固定窗口大小
    );

//    // 设置固定尺寸（使用UI设计的尺寸）
    setFixedSize(1920, 1200);
}

MainWidget::~MainWidget()
{
    delete ui;
}
// 处理显示事件，记录初始位置
void MainWidget::showEvent(QShowEvent *event)
{
    // 首次显示时记录初始位置
    if (m_firstShow) {
        m_originalPos = this->pos();
        m_firstShow = false;
    }
    QWidget::showEvent(event);
}
// Windows原生事件处理 - 关键实现
bool MainWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    // 只处理Windows消息
    if (eventType != "windows_generic_MSG") {
        return QWidget::nativeEvent(eventType, message, result);
    }

    MSG *msg = static_cast<MSG*>(message);
    switch (msg->message) {
        // 拦截标题栏鼠标按下事件（阻止拖拽开始）
        case WM_NCLBUTTONDOWN:
            // HTCAPTION表示点击的是标题栏
            if (msg->wParam == HTCAPTION) {
                *result = 0;
                return true; // 拦截此事件，不传递给系统
            }
            break;

        // 拦截窗口移动消息（防止意外移动）
        case WM_MOVE:
            // 如果窗口位置偏离初始位置，强制移回
            if (m_originalPos.x() != 0 || m_originalPos.y() != 0) { // 确保已初始化
                RECT rect;
                GetWindowRect((HWND)winId(), &rect);
                QPoint currentPos(rect.left, rect.top);

                if (currentPos != m_originalPos) {
                    MoveWindow((HWND)winId(),
                              m_originalPos.x(), m_originalPos.y(),
                              width(), height(),
                              TRUE);
                }
            }
            break;

        // 拦截窗口大小改变消息（防止窗口变小）
        case WM_SIZE:
            // 强制窗口保持初始大小
            SetWindowPos((HWND)winId(), NULL,
                        0, 0, width(), height(),
                        SWP_NOMOVE | SWP_NOZORDER);
            break;
    }

    // 其他消息正常处理
    return QWidget::nativeEvent(eventType, message, result);
}


void MainWidget::keyPressEvent(QKeyEvent *event)
{
    static bool a = false;
    if(event->key() == Qt::Key_F6)
    {
        QString str = "";
        if (a)
        {
            QFile f(":/res/img/light.css");
            f.open(QIODevice::ReadOnly);
            str = f.readAll();
            qApp->setStyleSheet(str);
            f.close();
            a = !a;
        }
        else
        {
            QFile f(":/res/img/black.css");
            f.open(QIODevice::ReadOnly);
            str = f.readAll();
            qApp->setStyleSheet(str);
            f.close();
            a = !a;
        }

        this->style()->unpolish(this);
        this->style()->polish(this);
        this->update();

        for(auto child: this->children())
        {
            if (child->isWidgetType())
            {
                QWidget* cw = qobject_cast<QWidget*>(child);

                cw->style()->unpolish(this);
                cw->style()->polish(this);
                cw->update();
            }
        }
    }
}

void MainWidget::on_tabWidget_currentChanged(int index)
{
    if(index==4)
    {
        LoginWidget login(this);
//        login.open();
        login.exec();

        if(!login.isLoginSucess())
        {
            ui->tabWidget->setCurrentIndex(0);
            return;
        }
    }
}
