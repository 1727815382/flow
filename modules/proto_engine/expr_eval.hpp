#pragma once
#include <QString>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>

// 仅提供 data("key") 的数据访问
class IDataProvider {
public:
    virtual ~IDataProvider() {}
    virtual QVariant get(const QString& key) = 0;
};

// 轻量表达式求值：整数/布尔/字符串；msg.xxx；data("k"); now_ms();
// 三目 ?:；关系 ==、!=；算术 + - * /（整数）
class ExprEval {
public:
    explicit ExprEval(IDataProvider* prov = 0) : prov_(prov) {}
    QVariant eval(const QString& expr, const QJsonObject& ctx, bool* ok = 0) const;

    static long long nowMs();

private:
    QVariant getVar(const QString& path, const QJsonObject& ctx, bool* ok) const;
    QVariant callFunc(const QString& name, const QStringList& args, bool* ok) const;
    QVariant evalSimple(const QString& e, const QJsonObject& ctx, bool* ok) const;
    QVariant evalRelation(const QString& e, const QJsonObject& ctx, bool* ok) const;
    QVariant evalArith(const QString& e, const QJsonObject& ctx, bool* ok) const;
    static QString trimWS(const QString& s);

private:
    IDataProvider* prov_;
};
