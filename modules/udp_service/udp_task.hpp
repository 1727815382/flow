#pragma once
#include <map>
#include <unordered_set>
#include <vector>
#include <atomic>
#include <thread>
#include <QString>
#include <QJsonObject>
#include "core/task_base.hpp"
#include "modules/udp_service/udp_socket.hpp"
#include "core/ts_queue.hpp"

class UdpTask : public TaskBase {
public:
    struct RetryCfg {
        int ack_timeout_ms = 500;
        int max_retries    = 5;
        int backoff_ms_init= 200;
        int backoff_ms_max = 2000;
    };
    struct Peer {
        std::string ip; uint16_t port=0;
        RetryCfg cfg;
        int heartbeat_ms=0;
        long long last_seen_ms=0;
        bool up=false;
        // 可靠子层
        uint32_t last_epoch=0;
        int64_t  last_seq=-1;
        std::unordered_set<uint32_t> recent_seq; // 简易去重窗口
        // RTT 自适应（EWMA）
        double srtt_ms=0.0, rttvar_ms=0.0;
    };
    struct Pending {
        std::string key;
        std::string ip; uint16_t port=0;
        uint32_t msg_id=0;
        std::vector<unsigned char> frame;
        RetryCfg cfg;
        int tries=0;
        long long next_ms=0;
        long long first_send_ms=0;
    };

    explicit UdpTask(MessageBus* bus, uint16_t localPort);

    static long long nowMs();

protected:
    void onStart() override;
    void onStop() override;
    void onMessage(Message& m) override;

private:
    // —— RX->TX 事件 —— //
    struct RxEvent {
        std::string ip; uint16_t port=0;
        std::vector<unsigned char> bytes;
        bool isUR=false, isAck=false;
        uint32_t msgid=0;
        const unsigned char* payload=nullptr;
        int payloadLen=0;
    };

    // 小工具
    static QString toHex(const unsigned char* p, int n);
    static std::vector<unsigned char> parseHex(const QString& h);

    // UR 编解码
    static std::vector<unsigned char> buildUR(uint32_t msgid, bool isAck,
                                              const unsigned char* payload, int n);
    static bool parseUR(const unsigned char* buf, int n, bool& isUR, bool& isAck,
                        uint32_t& msgid, const unsigned char*& payload, int& payloadLen);

    // 业务分发（TX/Actor）
    void actorProcessOne(const RxEvent& ev);
    void processAck(const std::string& ip, uint16_t port, uint32_t msgid);
    void publishRecv(const std::string& ip, uint16_t port,
                     const unsigned char* data, int n, bool reliable, uint32_t msgid);
    void sendAck(const std::string& ip, uint16_t port, uint32_t msgid);
    void sendRaw(const std::string& ip, uint16_t port, const unsigned char* data, int n);
    void scheduleReliableSend(const std::string& ip, uint16_t port,
                              const unsigned char* payload, int n,
                              const RetryCfg& cfg, uint32_t msgid);

    // 命令处理
    void cmdConfig(const QJsonObject& o);
    void cmdPeerAdd(const QJsonObject& o);
    void cmdPeerRemove(const QJsonObject& o);
    void cmdSend(const QJsonObject& o);

    // 线程循环
    void rxLoop();   // 仅收包
    void txLoop();   // Actor：ack/retry/心跳/事件

    // 其他
    void heartbeatSweep();
    void maybeReopenSocket(bool force, const char* reason);
    void updatePeerSeen_actor(const std::string& ip, uint16_t port);
    static std::string keyOf(const std::string& ip, uint16_t port);
    static std::string pkey(uint32_t msg_id, const std::string& ip, uint16_t port);

    // 自适应：更新 RTT
    void updateRtt(const std::string& ip, uint16_t port, long long sent_ms, long long now_ms);

    // msgid 方案
    uint32_t nextMsgId_compose(); // (epoch<<20)|seq
    static uint32_t ip_port_epoch(const Peer& p); // 目前不使用，可扩展

private:
    UdpSocket sock_;
    uint16_t  localPort_;
    int       rxBufSize_;

    // 线程与控制
    std::atomic<bool> stopRx_;
    std::thread rxThread_;
    std::thread txThread_;

    // 状态（仅 TX/Actor 线程访问）
    std::map<std::string, Peer>    peers_;
    std::map<std::string, Pending> pendings_;
    std::atomic<uint32_t> seq_{1};
    uint32_t epoch_; // 12bit 左移值

    RetryCfg defaultCfg_;
    // 事件通道
    TSQueue<RxEvent> rx2tx_;

    // 调试：HEX 截断
    int debug_hex_max_ = 128; // bytes，可通过 udp.config 配置
};
