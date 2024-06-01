#ifndef SOCKET_SUPPORT_H

#define SOCKET_SUPPORT_H

#include "packet.h"
#include "logger.h"
#include "alarm.h"

#define TIMEOUT_CNT_LIMIT 10

class SocketSupport{
private:
    int sendSocketfd_;
    int recvSocketfd_;
    double dropRate_;
    
    int timeoutCnt_;
    int timeoutInterval_;
    Packet lastestPacket_;  // 가장 최근에 전송한 패킷

    Logger& logger_;

    bool isDrop(double dropRate);

public:
    static SocketSupport* instance;

    SocketSupport(int sendSocketfd, int recvSocketfd, double dropRate, Logger& logger)
    :sendSocketfd_(sendSocketfd), recvSocketfd_(recvSocketfd), logger_(logger){
        if(instance==nullptr) instance=this;
        dropRate_=(dropRate<1.0)?dropRate:1.0;
        srand(time(nullptr));
        timeoutCnt_=0;
    }

    void setTimeoutInterval(int seconds){ timeoutInterval_=seconds; }

    // 패킷을 전송한다.
    void sendPacket(Event event, Packet packet, bool startTimer);

    // 받은 패킷을 반환한다.
    Packet recvPacket(bool stopTimer);

    static void onTimerExpired(int sig);

    // 패킷 timeout 핸들러
    void handleSendTimeout();
    
    void handleRecvTimeout();
};

#endif