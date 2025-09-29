#include "SystemConfigForm.h"
#include "ui_SystemConfigForm.h"

#include <QMessageBox>

SystemConfigForm::SystemConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemConfigForm)
{
    ui->setupUi(this);

    {
        int col = ui->comboBox->model()->rowCount();
        for(int i =0; i <col; i++)
        {
            QModelIndex idx = ui->comboBox->model()->index(i,0, ui->comboBox->rootModelIndex());
            ui->comboBox->model()->setData(idx,QSize(0,40),Qt::SizeHintRole);
        }
    }

}

SystemConfigForm::~SystemConfigForm()
{
    delete ui;
}

void SystemConfigForm::on_pBtnSetZero_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(QString::fromLocal8Bit("����λ���޸�"));
    msgBox.setText(QString::fromLocal8Bit("ȷ�����õ�ǰλ��Ϊ����λ����"));

    // �Զ���������ť
    QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("ȷ��"), QMessageBox::AcceptRole);
    QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("ȡ��"), QMessageBox::RejectRole);

    // ��ʾ��������ȡ���
    msgBox.exec();

    // �ж��û�����İ�ť
    if (msgBox.clickedButton() == saveBtn) {
        emit signCtrlClicked("SetFoldedPosition",ui->comboBox->currentIndex(),0);
    } else if (msgBox.clickedButton() == cancelBtn) {
        return;
    }
}

void SystemConfigForm::on_pBtnToZero_clicked()
{
    emit signCtrlClicked("toFoldedPosition",ui->comboBox->currentIndex(),0);
}

void SystemConfigForm::slotMotorSettingStatus(MotorSettingStatus settingStatus)
{
    switch (settingStatus)
    {
    case Motor_Setting_Fold_Fail:
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QString::fromLocal8Bit("����״̬"));
        msgBox.setText(QString::fromLocal8Bit("��������λ��ʧ�ܣ������豸״̬"));

        // �Զ���������ť
        QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("ȷ��"), QMessageBox::AcceptRole);
        QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("ȡ��"), QMessageBox::RejectRole);

        // ��ʾ��������ȡ���
        msgBox.exec();

        // �ж��û�����İ�ť
        if (msgBox.clickedButton() == saveBtn) {
            emit signCtrlClicked("SetFoldedPosition",ui->comboBox->currentIndex(),0);
        } else if (msgBox.clickedButton() == cancelBtn) {
            return;
        }

        break;
    }

    case Motor_Setting_Fold_Success:
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle(QString::fromLocal8Bit("����״̬"));
            msgBox.setText(QString::fromLocal8Bit("��������λ�óɹ��������������豸�ͳ���"));

            // �Զ���������ť
            QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("ȷ��"), QMessageBox::AcceptRole);
//            QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("ȡ��"), QMessageBox::RejectRole);

            // ��ʾ��������ȡ���
            msgBox.exec();

            // �ж��û�����İ�ť
            if (msgBox.clickedButton() == saveBtn) {
                exit(1);
            } /*else if (msgBox.clickedButton() == cancelBtn) {
                exit(1);
            }*/

            break;
        }


    case Motor_Setting_Zero_Fail:
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle(QString::fromLocal8Bit("�ο���״̬"));
            msgBox.setText(QString::fromLocal8Bit("���òο���ʧ�ܣ������豸״̬"));

            // �Զ���������ť
            QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("ȷ��"), QMessageBox::AcceptRole);
//            QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("ȡ��"), QMessageBox::RejectRole);

            // ��ʾ��������ȡ���
            msgBox.exec();

            // �ж��û�����İ�ť
            if (msgBox.clickedButton() == saveBtn) {
                emit signCtrlClicked("SetFoldedPosition",ui->comboBox->currentIndex(),0);
            } /*else if (msgBox.clickedButton() == cancelBtn) {
                return;*/
//            }

            break;
        }

    case Motor_Setting_Zero_Success:
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle(QString::fromLocal8Bit("�ο���״̬"));
            msgBox.setText(QString::fromLocal8Bit("���òο���λ�óɹ��������������豸�ͳ���"));

            // �Զ���������ť
            QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("ȷ��"), QMessageBox::AcceptRole);
//            QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("ȡ��"), QMessageBox::RejectRole);

            // ��ʾ��������ȡ���
            msgBox.exec();

            // �ж��û�����İ�ť
            if (msgBox.clickedButton() == saveBtn)
            {
                exit(1);
            } /*else if (msgBox.clickedButton() == cancelBtn)
            {
                exit(1);
            }*/

            break;
        }

    default:
        break;
    }
}

void SystemConfigForm::slotShowInfo(int state,QString info)
{
    if (state == 0)
        this->ui->textBrowser->append(QString::fromLocal8Bit("<font color = 'red'> %1</font>").arg(info));
    else
        this->ui->textBrowser->append(QString::fromLocal8Bit("<font color = 'green'> %1</font>").arg(info));

    if (ui->textBrowser->document()->lineCount() > 500)
    {
        ui->textBrowser->clear();
    }
}
