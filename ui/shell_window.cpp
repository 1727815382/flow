#include "ui/shell_window.hpp"
#include "core/message_bus.hpp"
#include "core/message.hpp"
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QDebug>

ShellWindow::ShellWindow(QWidget* parent) : QWidget(parent), bus_(0) {
    stackedWidget_ = new QStackedWidget(this);
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->addWidget(stackedWidget_);

    switchButton_ = new QPushButton("Switch View", this);
    mainLayout_->addWidget(switchButton_);

    connect(switchButton_, &QPushButton::clicked, [this]() {
        int count = stackedWidget_->count();
        if (count <= 1) return;
        int next = (stackedWidget_->currentIndex() + 1) % count;
        stackedWidget_->setCurrentIndex(next);
        emitNavCurrent();
    });
}

void ShellWindow::setBus(MessageBus* bus) { bus_ = bus; }

void ShellWindow::addView(const QString& name, QWidget* view) {
    if (!view || views_.contains(name)) return;
    views_[name] = view;
    order_.push_back(name);
    stackedWidget_->addWidget(view);
    if (stackedWidget_->count() == 1) {
        stackedWidget_->setCurrentWidget(view);
        emitNavCurrent();
    }
}

void ShellWindow::switchToView(const QString& name) {
    if (!views_.contains(name)) return;
    stackedWidget_->setCurrentWidget(views_[name]);
    emitNavCurrent();
}

void ShellWindow::emitNavCurrent() {
    if (!bus_) return;
    int idx = stackedWidget_->currentIndex();
    if (idx < 0 || idx >= order_.size()) return;
    Message nav; nav.header.type = "ui.nav";
    nav.payload = QJsonObject{{"view", order_.at(idx)}};
    bus_->publish("ui.nav", nav);
}
