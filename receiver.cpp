#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "packet.h"
#include "socketSupport.h"
#include "logger.h"

#define MAXBUF 1024
#define RECV_TIMEOUT_LIMIT 10

using namespace std;


int setSendSocket(const char* SENDER_IP, const int SENDER_PORT);
int setRecvSocket(const char* RECEIVER_IP, const int RECEIVER_PORT);

int main(int argc, char *argv[]) {
    // 프로그램 인자 설정
    const char* RECEIVER_IP="127.0.0.1";
    const int RECEIVER_PORT=stoi(argv[1]);
    const char* SENDER_IP="127.0.0.1";
    const int SENDER_PORT=51001;
    const double DROP_PROBABILITY=atof(argv[2]);
    string SAVE_DIRECTORY="./download";

    // 송수신 소켓 설정
    int sockfd_recv=setRecvSocket(RECEIVER_IP, RECEIVER_PORT);
    int sockfd_send=setSendSocket(SENDER_IP, SENDER_PORT);

    Logger logger("Receiver", "logs.txt");
    SocketSupport socketSupport(sockfd_send, sockfd_recv, DROP_PROBABILITY, logger);

    // 패킷 송수신
    int lastestSeqNum=-1;  // 가장 최근에 받은 패킷의 시퀀스 넘버

    // Greeting 수신
    Packet packet;
    while(true){
        packet=socketSupport.recvPacket(false);

        if(packet.seqNum()!=lastestSeqNum){
            // 새로운 패킷 받았으면 ACK 전송하고 루프 탈출
            lastestSeqNum=packet.seqNum();

            Packet ackPacket(ACK, packet.seqNum()+1);
            socketSupport.sendPacket(TRANSMISSION, ackPacket, false);
            break;
        }

        // 중복되는 패킷을 받은 경우 (premature retransmission)
        Packet ackPacket(ACK, packet.seqNum()+1);
        socketSupport.sendPacket(TRANSMISSION, ackPacket, false);
    }

    // Filename
    string filename;
    while(true){
        packet=socketSupport.recvPacket(false);

        if(packet.seqNum()!=lastestSeqNum){
            // 새로운 패킷 받았으면 ACK 전송하고 루프 탈출
            lastestSeqNum=packet.seqNum();
            filename=packet.data();

            Packet ackPacket(ACK, packet.seqNum()+1);
            socketSupport.sendPacket(TRANSMISSION, ackPacket, false);
            break;
        }

        // 중복되는 패킷을 받은 경우 (premature retransmission)
        Packet ackPacket(ACK, packet.seqNum()+1);
        socketSupport.sendPacket(TRANSMISSION, ackPacket, false);
    }

    // 파일 path 설정    
    string filepath=SAVE_DIRECTORY;

    // 저장할 디렉터리 없으면 생성
    if(!filesystem::exists(filepath)){
        filesystem::create_directory(filepath);
    }

    if(filepath.back()!='/') filepath+='/';
    filepath+=filename;
    
    // 대용량 File을 packet 단위로 수신해서 파일에 저장
    ofstream file(filepath, ios::binary);
    if(!file.is_open()){
        perror("[ERROR] File Open Error\n");
        exit(errno);
    }

    while (true) {
        try{
            packet=socketSupport.recvPacket(false);

            if(packet.seqNum()!=lastestSeqNum){
                // 새로운 패킷 받았으면 ACK 전송
                lastestSeqNum=packet.seqNum();
                    
                // EOT 패킷을 수신하면 통신 종료
                if(packet.type()==EOT){
                    Alarm::startRecvTimer(RECV_TIMEOUT_LIMIT);
                    Packet ackPacket(ACK, packet.seqNum()+1);
                    socketSupport.sendPacket(TRANSMISSION, ackPacket, false);
                    break;
                }

                // 파일 쓰기
                file.write(packet.data().c_str(), packet.length());

                Packet ackPacket(ACK, packet.seqNum()+1);
                socketSupport.sendPacket(TRANSMISSION, ackPacket, false);
                continue;
            }

            // 중복되는 패킷을 받은 경우 (premature retransmission)
            Packet ackPacket(ACK, packet.seqNum()+1);
            socketSupport.sendPacket(TRANSMISSION, ackPacket, false);

        }catch(const exception& e){
            perror(e.what());
            break;
        }
    }

    file.close();

    logger.log("Receiver >> File Transmission Finish\n");
    
    // 소켓 close
    close(sockfd_recv);
    close(sockfd_send);
    
    exit(0);
}


int setSendSocket(const char* SENDER_IP, const int SENDER_PORT){
    int sockfd_send = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_send < 0) {
        perror("[ERROR] Socket Creation Error\n");
        exit(errno);
    }

    struct sockaddr_in dest_send;
    memset(&dest_send, 0, sizeof(dest_send));
    dest_send.sin_family = AF_INET;
    dest_send.sin_port = htons(SENDER_PORT);
    int inet_send=inet_aton(SENDER_IP, &dest_send.sin_addr);
    if (inet_send == 0) {
        perror("[ERROR] Socket IP Setting Error\n");
        exit(errno);
    }
    
    int con_send=connect(sockfd_send, (struct sockaddr *)&dest_send, sizeof(dest_send));
    if (con_send != 0) {
        perror("[ERROR] Socket Connection Error\n");
        exit(errno);
    }

    return sockfd_send;
}


int setRecvSocket(const char* RECEIVER_IP, const int RECEIVER_PORT){
    int sockfd_recv = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_recv < 0) {
        perror("[ERROR] Socket Creation Error\n");
        exit(errno);
    }

    struct sockaddr_in dest_recv;
    memset(&dest_recv, 0, sizeof(dest_recv));
    dest_recv.sin_family = AF_INET;
    dest_recv.sin_port = htons(RECEIVER_PORT);
    int inet_recv=inet_aton(RECEIVER_IP, &dest_recv.sin_addr);
    if (inet_recv == 0) {
        perror("[ERROR] Socket IP Setting Error\n");
        exit(errno);
    }
    
    int bind_recv=bind(sockfd_recv, (struct sockaddr *)&dest_recv, sizeof(dest_recv));
    if (bind_recv != 0) {
        perror("[ERROR] Socket Bind Error\n");
        exit(errno);
    }

    return sockfd_recv;
}