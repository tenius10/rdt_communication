#include <netinet/in.h>
#include <ctime>
 
#include "socketSupport.h"

#define MAXBUF 2000

SocketSupport* SocketSupport::instance=nullptr;

void SocketSupport::handleSendTimeout(){
    logger_.log(lastestPacket_, TIMEOUT);

    timeoutCnt_++;
    if(timeoutCnt_>=TIMEOUT_CNT_LIMIT){
        perror("Error : Fail to send file. please try again\n");
        logger_.log("Error : Fail to send file. please try again\n");
        exit(1);
    }

    // 패킷 재전송
    sendPacket(RETRANSMISSION, lastestPacket_, true); 
}

void SocketSupport::handleRecvTimeout(){
    perror("Error : Fail to receive file. please try again\n");
    logger_.log("Error : Fail to receive file. please try again\n");
    exit(1);
}

bool SocketSupport::isDrop(double dropRate){
    if(dropRate!=0){
        int mod=(int)(1.0/dropRate);
        return rand()%mod==0;
    }
    else return false;
}

// 패킷을 전송한다.
void SocketSupport::sendPacket(Event event, Packet packet, bool startTimer){
    bool isDrop_=isDrop(dropRate_);
    
    // 타이머 시작
    if(startTimer){
        Alarm::startSendTimer(timeoutInterval_);   
    }

    if(isDrop_){
        // 패킷 드랍 (이후 sigalrm에 의해서 timeout, retransmission이 일어난다)
        logger_.log(packet, DISCARD);
    }
    else{
        // 패킷 전송
        std::string packetString=Packet::serialize(packet);
        send(sendSocketfd_, packetString.c_str(), packetString.length(), 0);
        lastestPacket_=packet;
        logger_.log(packet, event);  // TRANSMISSION or RETRANSMISSION
    }
}

// 받은 패킷을 반환한다.
Packet SocketSupport::recvPacket(bool stopTimer){
    char buffer[MAXBUF];
    memset(buffer, MAXBUF, 0);
    int count=recv(recvSocketfd_, buffer, sizeof(buffer), 0);
    if(count>0){
        timeoutCnt_=0;

        std::string packetString(buffer);
        Packet packet=Packet::deserialize(packetString);

        // 타이머 종료
        if(stopTimer){
            Alarm::stopTimer();
        }

        logger_.log(packet, RECEPTION);
        return packet;
    }
    else throw std::runtime_error("Fail to recieve packet");
}
