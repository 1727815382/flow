#pragma once
#include <QJsonObject>

class MessageBus;
class ITask;

extern "C" {
typedef ITask* (*CreateTaskFn)(MessageBus* bus, const char* id, const QJsonObject* args);
typedef void   (*DestroyTaskFn)(ITask*);
typedef void*  (*CreateViewFn)(QWidget* parent, const QJsonObject* args);
typedef void   (*DestroyViewFn)(void*);
}
