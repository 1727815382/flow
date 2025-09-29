// core/supervisor.cpp
#include "core/supervisor.hpp"
#include <QDateTime>

static long long now_ms(){ return (long long)QDateTime::currentMSecsSinceEpoch(); }
