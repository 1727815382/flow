#pragma once
#include <QWidget>
#include <QMap>
#include <QStringList>

class QStackedWidget;
class QVBoxLayout;
class QPushButton;
class MessageBus;

class ShellWindow : public QWidget {
    Q_OBJECT
public:
    explicit ShellWindow(QWidget* parent = 0);
    void setBus(MessageBus* bus);

    void addView(const QString& name, QWidget* view);
    void switchToView(const QString& name);

private:
    void emitNavCurrent();

    MessageBus* bus_;
    QMap<QString, QWidget*> views_;
    QStringList order_;
    QStackedWidget* stackedWidget_;
    QVBoxLayout* mainLayout_;
    QPushButton* switchButton_;
};
