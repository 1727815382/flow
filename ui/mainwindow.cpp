#include "ui/mainwindow.hpp"
#include <QStackedWidget>
#include <QToolBar>
#include <QComboBox>
#include <QAction>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), bus_(0) {
    stacked_ = new QStackedWidget(this);
    setCentralWidget(stacked_);

    QToolBar* tb = addToolBar("Views");
    combo_ = new QComboBox(tb);
    tb->addWidget(combo_);
    actNext_ = tb->addAction("Next");

    connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(onViewChanged(int)));
    connect(actNext_, SIGNAL(triggered()), this, SLOT(onNext()));
}

void MainWindow::setBus(MessageBus* bus) { bus_ = bus; }

void MainWindow::addView(const QString& name, QWidget* view) {
    if (!view) return;
    if (views_.contains(name)) return;
    views_[name] = view;
    order_.push_back(name);
    int idx = stacked_->addWidget(view);
    combo_->addItem(name);
    qDebug() << "[MainWindow] addView:" << name << " idx=" << idx;
    if (stacked_->count() == 1) {
        stacked_->setCurrentIndex(0);
        combo_->setCurrentIndex(0);
    }
}

void MainWindow::switchToView(const QString& name) {
    if (!views_.contains(name)) return;
    QWidget* w = views_[name];
    int idx = stacked_->indexOf(w);
    if (idx >= 0) {
        stacked_->setCurrentIndex(idx);
        combo_->setCurrentIndex(idx);
        qDebug() << "[MainWindow] switchToView:" << name << " idx=" << idx;
    }
}

void MainWindow::onViewChanged(int idx) {
    if (idx < 0 || idx >= stacked_->count()) return;
    stacked_->setCurrentIndex(idx);
    qDebug() << "[MainWindow] onViewChanged -> idx=" << idx;
}

void MainWindow::onNext() {
    int count = stacked_->count();
    if (count <= 1) return;
    int next = (stacked_->currentIndex() + 1) % count;
    stacked_->setCurrentIndex(next);
    combo_->setCurrentIndex(next);
    qDebug() << "[MainWindow] onNext -> idx=" << next;
}
