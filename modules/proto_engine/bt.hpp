#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <vector>
#include "modules/proto_engine/expr_eval.hpp"

// 行为树基础
struct BTResult { bool ok=true; QJsonObject ctx; };

class IDataProvider; // 前置声明

class BTNode {
public:
    virtual ~BTNode() {}
    virtual bool exec(QJsonObject& ctx, IDataProvider* dp, ExprEval* eval) = 0;
};

// ========== 基础节点 ==========
class BTSequence : public BTNode {
public:
    std::vector<std::unique_ptr<BTNode>> children;
    bool exec(QJsonObject& ctx, IDataProvider* dp, ExprEval* eval) override {
        for (auto& c: children) if (!c->exec(ctx, dp, eval)) return false;
        return true;
    }
};
class BTSelector : public BTNode {
public:
    std::vector<std::unique_ptr<BTNode>> children;
    bool exec(QJsonObject& ctx, IDataProvider* dp, ExprEval* eval) override {
        for (auto& c: children) if (c->exec(ctx, dp, eval)) return true;
        return false;
    }
};

// ========== 增强节点 ==========
class BTGuard : public BTNode {
public:
    QString cond; // 表达式（返回 bool）
    std::unique_ptr<BTNode> thenNode;
    std::unique_ptr<BTNode> elseNode;
    bool exec(QJsonObject& ctx, IDataProvider* dp, ExprEval* eval) override {
        bool ok=false; QVariant v = eval->eval(cond, ctx, &ok);
        bool b = ok && v.toBool();
        if (b && thenNode) return thenNode->exec(ctx, dp, eval);
        if (!b && elseNode) return elseNode->exec(ctx, dp, eval);
        return true;
    }
};

class BTLoop : public BTNode {
public:
    int times = 1; // <0 无限（谨慎使用）
    std::unique_ptr<BTNode> body;
    bool exec(QJsonObject& ctx, IDataProvider* dp, ExprEval* eval) override {
        if (!body) return true;
        if (times < 0) { // 为防止卡死，限制为 10000 次
            for (int i=0;i<10000;++i) if (!body->exec(ctx, dp, eval)) return false;
            return true;
        }
        for (int i=0;i<times;++i) if (!body->exec(ctx, dp, eval)) return false;
        return true;
    }
};

// Wait：不在此处sleep，由 ProtocolEngineTask 接管调度
class BTWait : public BTNode {
public:
    int ms=0;
    QJsonObject thenTree; // JSON 子树
    bool exec(QJsonObject& ctx, IDataProvider* /*dp*/, ExprEval* /*eval*/) override {
        // 这里返回 true，实际动作由 ProtocolEngineTask::actTimer + bt.exec 完成
        return true;
    }
};

class BTParallel : public BTNode {
public:
    std::vector<std::unique_ptr<BTNode>> children;
    QString policy = "all"; // "all" / "any"
    bool exec(QJsonObject& ctx, IDataProvider* dp, ExprEval* eval) override {
        if (children.empty()) return true;
        if (policy == "any") {
            for (auto& c: children) { QJsonObject tmp=ctx; if (c->exec(tmp, dp, eval)) { ctx=tmp; return true; } }
            return false;
        } else { // all
            for (auto& c: children) if (!c->exec(ctx, dp, eval)) return false;
            return true;
        }
    }
};

class BTRace : public BTNode {
public:
    std::vector<std::unique_ptr<BTNode>> children;
    bool exec(QJsonObject& ctx, IDataProvider* dp, ExprEval* eval) override {
        for (auto& c: children) { QJsonObject tmp=ctx; if (c->exec(tmp, dp, eval)) { ctx=tmp; return true; } }
        return false;
    }
};

// 叶子：发布动作（把 BT 的一个动作转换到 ProtocolEngine 的 actXxx）
// 这里只保存动作 JSON，由 ProtocolEngineTask 的 execAction 执行
class BTAction : public BTNode {
public:
    QString name; QJsonObject args;
    std::function<bool(const QString&, const QJsonObject&, const QJsonObject&)> execAction;
    bool exec(QJsonObject& ctx, IDataProvider* /*dp*/, ExprEval* /*eval*/) override {
        if (!execAction) return false;
        return execAction(name, args, ctx), true;
    }
};

// 构建器
struct BTBuilder {
    static std::unique_ptr<BTNode> build(const QJsonObject& o, ExprEval* eval,
                                         std::function<bool(const QString&, const QJsonObject&, const QJsonObject&)> actionExec);
private:
    static std::unique_ptr<BTNode> buildNode(const QJsonObject& o, ExprEval* eval,
                                         std::function<bool(const QString&, const QJsonObject&, const QJsonObject&)> actionExec);
    static void buildChildren(BTNode* parent, const QJsonArray& arr, ExprEval* eval,
                              std::function<bool(const QString&, const QJsonObject&, const QJsonObject&)> actionExec);
};
