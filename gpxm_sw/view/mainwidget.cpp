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

    this->setWindowTitle("�������ϵͳ");

    //����С����������
    NetDataProcessing* netPrc = new NetDataProcessing();
    connect (netPrc, SIGNAL(signMesureUpdate(TrackMesure)), ui->workWgt, SLOT(slotMesureUpdate(TrackMesure)));

    //����plc��motor״̬
    DeviceManager* dm = new DeviceManager();
    dm->initDevCfg();
    connect (dm, SIGNAL(signMotorUpdate(MotorResponse&)), ui->DataWgt, SLOT(slotUpdateMotor(MotorResponse&))); //���״̬
    connect (dm, SIGNAL(signPlcUpdate(PlcResponse&)),     ui->DataWgt, SLOT(slotUpdatePlc(PlcResponse&))); //plc״̬
    connect (dm, SIGNAL(signCannotWorkMessage(MotorStatus)),ui->DataWgt, SLOT(slotCannotWorkMessage(MotorStatus))); //������æ�͹�����ʾ
    connect (dm, SIGNAL(signMotorFoldDoState (int&)),     ui->DataWgt, SLOT(slotMotorFoldDoState (int&)));

    connect (ui->DataWgt, SIGNAL(signCtrlClicked(QString,double)), dm, SLOT(slotCtrlClicked(QString,double))); //���ư�ť
    connect (ui->DataWgt, SIGNAL(signSectionSelected(int)), dm, SLOT(slotSectionSelected(int))); //����ѡ��
    connect (ui->DataWgt, SIGNAL(signFoldDoClicked(QString,double)), dm, SLOT(slotFoldDoClicked(QString, double))); //֧��ѹʵ
    connect (ui->SysCfgWgt, SIGNAL(signCtrlClicked(QString ,int , int )), dm, SLOT(slotCtrlClicked(QString ,int , int ))); //���ư�ť

    connect (dm, SIGNAL(signMotorSettingStatus(MotorSettingStatus)), ui->SysCfgWgt, SLOT(slotMotorSettingStatus(MotorSettingStatus)));
    connect (dm, SIGNAL(signMotorSettingStatus(MotorSettingStatus)), ui->DataWgt,   SLOT(slotMotorSettingStatus(MotorSettingStatus)));

    //    connect (ui->workWgt, SIGNAL(signMesureSelected(int), dm, SLOT(slotMesureSelected(int))); //ѡ�ж���
    connect (ui->workWgt, SIGNAL(signDealSection(int)),               dm, SLOT(slotDealSection(int))); //��������
    connect (ui->workWgt, SIGNAL(signRowSelected(int)),            dm, SLOT(slotRowSelected(int))); //ѡ�в�������
    connect (ui->workWgt, SIGNAL(signMenuAddMesure(TrackMesure&)), dm, SLOT(slotMenuAddMesure(TrackMesure&)));
    connect (ui->workWgt, SIGNAL(signMenuMdyMesure(TrackMesure&)), dm, SLOT(slotMenuMdyMesure(TrackMesure&)));
    connect (ui->workWgt, SIGNAL(signMenuDelMesure()),             dm, SLOT(slotMenuDelMesure()));
    connect (ui->workWgt, SIGNAL(signMenuClrMesure()),             dm, SLOT(slotMenuClrMesure()));
    connect (ui->workWgt, SIGNAL(signSingleMesure(int)),           dm, SLOT(slotSingleMesure(int))); //�������
    connect (ui->workWgt, SIGNAL(signAutoMesure(int)),             dm, SLOT(slotAutoMesure(int))); //�������
    connect (ui->workWgt, SIGNAL(signStopMesure()),                dm, SLOT(slotStopMesure())); //ֹͣ����
//    connect (ui->workWgt, SIGNAL(signDataCheck()),                 dm, SLOT(slotDataCheck())); //��ǰʹ���������㷨����������

    connect (dm, SIGNAL(signSelectedMesure(TrackMesure&)),   ui->workWgt, SLOT(slotSelectedMesure(TrackMesure&))); //ѡ�����ݸ��µ�������
    connect (dm, SIGNAL(signOldMesure (TrackMesure&)),       ui->workWgt, SLOT(slotOldMesure(TrackMesure&))); //������ˢ��
    connect (dm, SIGNAL(signMesureRemoved(int)),             ui->workWgt, SLOT(slotMesureRemoved(int))); //֪ͨ����ɾ������
    connect (dm, SIGNAL(signMesureUpdated(int,TrackMesure&)),ui->workWgt, SLOT(slotMesureUpdate(int,TrackMesure&))); //֪ͨ����ɾ������
    connect (dm, SIGNAL(signMesureUpdate(TrackMesure)),      ui->workWgt, SLOT(slotMesureUpdate(TrackMesure)));

    connect (dm, SIGNAL(signShowInfo(QString)),     ui->workWgt,   SLOT(slotShowErrInfo(QString))); //�쳣��Ϣ��ʾ
    connect (dm, SIGNAL(signShowInfoToSys(int, QString)),     ui->SysCfgWgt, SLOT(slotShowInfo(int, QString))); //�쳣��Ϣ��ʾ
    connect (dm, SIGNAL(signShowInfoToData(int,QString)),     ui->DataWgt,   SLOT(slotShowInfo(int,QString))); //�쳣��Ϣ��ʾ

    connect (dm, SIGNAL(signMotorRunOver()),        ui->workWgt, SLOT(slotMotorRunOver())); //����������ź�



    //    connect (netPrc, SIGNAL(signMesureUpdate(TrackMesure), dm, SLOT(slotMesureUpdate(TrackMesure))); //�������ݣ�����һ������

    //�������ݽ����л�����
    connect (ui->DataWgt, SIGNAL(signPlcSelected(int)), dm, SLOT (slotUpdatePlcSelected(int)));


    ui->workWgt->setObjectName("MainWidget");
    ui->ConfigWgt->setObjectName("MainWidget");
    ui->DataWgt->setObjectName("MainWidget");
    ui->TuingWgt->setObjectName("MainWidget");
    ui->tabWidget->removeTab(4);

    // ������������ - ȷ��������������ʾ
    setWindowFlags(
        Qt::Window |                  // ������������
        Qt::WindowTitleHint |         // ��ʾ������
        Qt::WindowCloseButtonHint |   // �����رհ�ť
        Qt::MSWindowsFixedSizeDialogHint  // �̶����ڴ�С
    );

//    // ���ù̶��ߴ磨ʹ��UI��Ƶĳߴ磩
    setFixedSize(1920, 1200);
}

MainWidget::~MainWidget()
{
    delete ui;
}
// ������ʾ�¼�����¼��ʼλ��
void MainWidget::showEvent(QShowEvent *event)
{
    // �״���ʾʱ��¼��ʼλ��
    if (m_firstShow) {
        m_originalPos = this->pos();
        m_firstShow = false;
    }
    QWidget::showEvent(event);
}
// Windowsԭ���¼����� - �ؼ�ʵ��
bool MainWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    // ֻ����Windows��Ϣ
    if (eventType != "windows_generic_MSG") {
        return QWidget::nativeEvent(eventType, message, result);
    }

    MSG *msg = static_cast<MSG*>(message);
    switch (msg->message) {
        // ���ر�������갴���¼�����ֹ��ק��ʼ��
        case WM_NCLBUTTONDOWN:
            // HTCAPTION��ʾ������Ǳ�����
            if (msg->wParam == HTCAPTION) {
                *result = 0;
                return true; // ���ش��¼��������ݸ�ϵͳ
            }
            break;

        // ���ش����ƶ���Ϣ����ֹ�����ƶ���
        case WM_MOVE:
            // �������λ��ƫ���ʼλ�ã�ǿ���ƻ�
            if (m_originalPos.x() != 0 || m_originalPos.y() != 0) { // ȷ���ѳ�ʼ��
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

        // ���ش��ڴ�С�ı���Ϣ����ֹ���ڱ�С��
        case WM_SIZE:
            // ǿ�ƴ��ڱ��ֳ�ʼ��С
            SetWindowPos((HWND)winId(), NULL,
                        0, 0, width(), height(),
                        SWP_NOMOVE | SWP_NOZORDER);
            break;
    }

    // ������Ϣ��������
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
