#pragma once
#include <QMainWindow>
#include <QMap>
#include <QStringList>

class QStackedWidget;
class QComboBox;
class QAction;
class MessageBus;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    void setBus(MessageBus* bus);

    void addView(const QString& name, QWidget* view);
    void switchToView(const QString& name);

private slots:
    void onViewChanged(int idx);
    void onNext();

private:
    MessageBus* bus_;
    QStackedWidget* stacked_;
    QComboBox* combo_;
    QAction* actNext_;
    QMap<QString, QWidget*> views_;
    QStringList order_;
};
