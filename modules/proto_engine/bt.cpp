#include "modules/proto_engine/bt.hpp"

std::unique_ptr<BTNode> BTBuilder::build(const QJsonObject& o, ExprEval* eval,
    std::function<bool(const QString&, const QJsonObject&, const QJsonObject&)> actionExec){
    return buildNode(o, eval, actionExec);
}

void BTBuilder::buildChildren(BTNode* parent, const QJsonArray& arr, ExprEval* eval,
    std::function<bool(const QString&, const QJsonObject&, const QJsonObject&)> actionExec){
    if (auto seq = dynamic_cast<BTSequence*>(parent)) {
        for (auto v: arr) seq->children.push_back(buildNode(v.toObject(), eval, actionExec));
    } else if (auto sel = dynamic_cast<BTSelector*>(parent)) {
        for (auto v: arr) sel->children.push_back(buildNode(v.toObject(), eval, actionExec));
    } else if (auto par = dynamic_cast<BTParallel*>(parent)) {
        for (auto v: arr) par->children.push_back(buildNode(v.toObject(), eval, actionExec));
    } else if (auto race = dynamic_cast<BTRace*>(parent)) {
        for (auto v: arr) race->children.push_back(buildNode(v.toObject(), eval, actionExec));
    }
}

std::unique_ptr<BTNode> BTBuilder::buildNode(const QJsonObject& o, ExprEval* eval,
    std::function<bool(const QString&, const QJsonObject&, const QJsonObject&)> actionExec){
    QString ty = o.value("type").toString();
    if (ty=="sequence") {
        auto p = std::unique_ptr<BTSequence>(new BTSequence());
        buildChildren(p.get(), o.value("children").toArray(), eval, actionExec);
        return p;
    } else if (ty=="selector") {
        auto p = std::unique_ptr<BTSelector>(new BTSelector());
        buildChildren(p.get(), o.value("children").toArray(), eval, actionExec);
        return p;
    } else if (ty=="guard") {
        auto p = std::unique_ptr<BTGuard>(new BTGuard());
        p->cond = o.value("cond").toString();
        if (o.contains("then")) p->thenNode = buildNode(o.value("then").toObject(), eval, actionExec);
        if (o.contains("else")) p->elseNode = buildNode(o.value("else").toObject(), eval, actionExec);
        return p;
    } else if (ty=="loop") {
        auto p = std::unique_ptr<BTLoop>(new BTLoop());
        p->times = o.value("times").toInt(1);
        p->body = buildNode(o.value("body").toObject(), eval, actionExec);
        return p;
    } else if (ty=="wait") {
        auto p = std::unique_ptr<BTWait>(new BTWait());
        p->ms = o.value("ms").toInt(0);
        p->thenTree = o.value("then").toObject();
        return p;
    } else if (ty=="parallel") {
        auto p = std::unique_ptr<BTParallel>(new BTParallel());
        p->policy = o.value("policy").toString("all");
        buildChildren(p.get(), o.value("children").toArray(), eval, actionExec);
        return p;
    } else if (ty=="race") {
        auto p = std::unique_ptr<BTRace>(new BTRace());
        buildChildren(p.get(), o.value("children").toArray(), eval, actionExec);
        return p;
    } else if (ty=="action") {
        auto p = std::unique_ptr<BTAction>(new BTAction());
        p->name = o.value("name").toString();
        p->args = o.value("args").toObject();
        p->execAction = actionExec;
        return p;
    }
    // 默认空序列
    return std::unique_ptr<BTSequence>(new BTSequence());
}
