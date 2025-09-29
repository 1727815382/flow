#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPoint>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_tabWidget_currentChanged(int index);


private:
    Ui::MainWidget *ui;
    QPoint m_originalPos;
     QSize m_originalSize;
     bool m_firstShow;
     bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
     void showEvent(QShowEvent *event) override;
};

#endif // MAINWIDGET_H
