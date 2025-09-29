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
    msgBox.setWindowTitle(QString::fromLocal8Bit("收腿位置修改"));
    msgBox.setText(QString::fromLocal8Bit("确认设置当前位置为收腿位置吗？"));

    // 自定义两个按钮
    QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
    QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

    // 显示弹窗并获取结果
    msgBox.exec();

    // 判断用户点击的按钮
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
        msgBox.setWindowTitle(QString::fromLocal8Bit("收腿状态"));
        msgBox.setText(QString::fromLocal8Bit("设置收腿位置失败，请检查设备状态"));

        // 自定义两个按钮
        QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
        QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

        // 显示弹窗并获取结果
        msgBox.exec();

        // 判断用户点击的按钮
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
            msgBox.setWindowTitle(QString::fromLocal8Bit("收腿状态"));
            msgBox.setText(QString::fromLocal8Bit("设置收腿位置成功，请重启轨排设备和程序"));

            // 自定义两个按钮
            QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
//            QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

            // 显示弹窗并获取结果
            msgBox.exec();

            // 判断用户点击的按钮
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
            msgBox.setWindowTitle(QString::fromLocal8Bit("参考点状态"));
            msgBox.setText(QString::fromLocal8Bit("设置参考点失败，请检查设备状态"));

            // 自定义两个按钮
            QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
//            QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

            // 显示弹窗并获取结果
            msgBox.exec();

            // 判断用户点击的按钮
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
            msgBox.setWindowTitle(QString::fromLocal8Bit("参考点状态"));
            msgBox.setText(QString::fromLocal8Bit("设置参考点位置成功，请重启轨排设备和程序"));

            // 自定义两个按钮
            QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
//            QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

            // 显示弹窗并获取结果
            msgBox.exec();

            // 判断用户点击的按钮
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
