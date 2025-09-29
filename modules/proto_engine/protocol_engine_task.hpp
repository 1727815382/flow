#pragma once
#include "core/task_base.hpp"
#include "modules/proto_engine/expr_eval.hpp"
#include "modules/proto_engine/field_codec.hpp"
#include <QVariant>
#include <QJsonArray>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ProtocolEngineTask : public TaskBase, public IDataProvider {
public:
    ~ProtocolEngineTask() override;
    explicit ProtocolEngineTask(MessageBus* bus);

    // IDataProvider
    QVariant get(const QString& key) override;

protected:
    void onStart() override;
    void onStop() override;
    void onMessage(Message& m) override;

private:
    // ==== 配置结构（略：与您现有一致，省去重复定义）====
    struct MatchCond { int offset=0; QByteArray equals; QByteArray mask; };
    struct Bit { QString name; int pos=0; int len=1; };
    struct FieldDef {
        QString name, type, endian, lenFrom;
        int offset=0, fixlen=0;
        std::vector<Bit> bits;
    };
    struct MessageDef {
        QString id, binding;
        std::vector<MatchCond> matchAll, matchAny;
        std::vector<FieldDef> fields;
        bool hasChecksum=false;
        struct { QString algo; int rangeFrom=0; QString rangeTo; } checksum;
    };
    struct TemplateDef { QString id; QJsonArray compose; };
    struct Binding { QString id, udpTask; int listenPort=0; struct Peer { QString ip; int port; }; std::vector<Peer> peers; };
    struct Action { QString type; QJsonObject args; };
    struct Rule { QString id, when, message, binding; QJsonObject bt; std::vector<Action> actions; };
    struct Config {
        bool debug=true;
        std::vector<Binding> bindings;
        std::vector<MessageDef> messages;
        std::vector<Rule> rules;
        std::map<QString,int> msgIndex;
        std::map<QString, TemplateDef> templates;
    } cfg_;

    // ==== 定时器（单线程小顶堆）====
    struct TimerItem {
        long long due=0;
        int repeat=0; // <0 无限
        int period_ms=0;
        QJsonObject action;
        QJsonObject ctx;
        int id=0;
        bool operator<(const TimerItem& o) const { return due > o.due; }
    };
    std::priority_queue<TimerItem> tqueue_;
    std::mutex tmu_;
    std::condition_variable tcv_;
    std::thread tworker_;
    std::atomic<bool> tstop_{false};
    int timer_seq_=1;
    void timerThread();
    void scheduleTimer(int delay_ms, int repeat, const QJsonObject& action, const QJsonObject& ctx);

    // ==== 工具 ====
    static QByteArray stripHex(const QString& s);
    static QString bytesToHex(const QByteArray& ba, bool withSpaces);
    static QByteArray hexToBytes(const QString& hex);

    bool loadConfigFile(const QString& path);
    bool matchMessage(const MessageDef& md, const QByteArray& bytes) const;
    QJsonObject decodeFields(const MessageDef& md, const QByteArray& bytes) const;
    QByteArray composeFromTemplate(const QString& tplId, const QJsonObject& ctx);
    QByteArray composeBySteps(const QJsonArray& steps, const QJsonObject& ctx);

    // 动作
    bool execAction(const QString& name, const QJsonObject& args, const QJsonObject& ctx);
    void actPublish(const QJsonObject& args, const QJsonObject& ctx);
    void actReply(const QJsonObject& args, const QJsonObject& ctx);
    void actTimer(const QJsonObject& args, const QJsonObject& ctx);
    void actStateSet(const QJsonObject& args, const QJsonObject& ctx);

    // 校验工具
    bool applyChecksum(QByteArray& buf, const QJsonObject& cs);
    // 数据来源
    std::map<QString, QVariant> datahub_;
    std::unique_ptr<ExprEval>   eval_;
    std::unique_ptr<FieldCodec> codec_;
};
