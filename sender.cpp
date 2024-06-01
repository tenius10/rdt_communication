#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "packet.h"
#include "socketSupport.h"

#define MAXBUF 1024

using namespace std;


int setSendSocket(const char* SENDER_IP, const int SENDER_PORT);
int setRecvSocket(const char* RECEIVER_IP, const int RECEIVER_PORT);

int main(int argc, char *argv[]) {
    // 프로그램 인자 설정
    const char* SENDER_IP="127.0.0.1";
    const int SENDER_PORT=stoi(argv[1]);
    const char* RECEIVER_IP=argv[2];
    const int RECEIVER_PORT=stoi(argv[3]);
    const int TIMEOUT_INTERVAL=stoi(argv[4]);
    const string FILENAME=argv[5];
    const double DROP_PROBABILITY=atof(argv[6]);

    // 송수신 소켓 설정
    int sockfd_send=setSendSocket(SENDER_IP, SENDER_PORT);
    int sockfd_recv=setRecvSocket(RECEIVER_IP, RECEIVER_PORT);

    Logger logger("Sender", "logs.txt");
    SocketSupport socketSupport(sockfd_send, sockfd_recv, DROP_PROBABILITY, logger);
    socketSupport.setTimeoutInterval(TIMEOUT_INTERVAL);

    // 패킷 송수신

    // Greeting
    int seqNum=0;
    Packet greetingPacket(DATA, seqNum, "Greeting");

    socketSupport.sendPacket(TRANSMISSION, greetingPacket, true);

    Packet greetingAckPacket=socketSupport.recvPacket(true);
    seqNum=greetingAckPacket.ackNum();

    // Filename
    Packet filenamePacket(DATA, seqNum, FILENAME);
    socketSupport.sendPacket(TRANSMISSION, filenamePacket, true);

    Packet filenameAckPacket=socketSupport.recvPacket(true);
    seqNum=filenameAckPacket.ackNum();

    // 대용량 File을 packet 단위로 잘라서 전송
    ifstream file(FILENAME, ios::binary);
    if (!file.is_open()) {
        perror("[ERROR] File Open Error\n");
        exit(errno);
    }

    while (file) {
        char buffer[MAXBUF];
        file.read(buffer, sizeof(buffer));
        size_t count = file.gcount();
        if(count<MAXBUF) buffer[count]='\0';
        if (count > 0) {
            // DATA 패킷 전송
            Packet packet(DATA, seqNum, buffer);
            socketSupport.sendPacket(TRANSMISSION, packet, true);

            // ACK 패킷 수신
            Packet ackPacket=socketSupport.recvPacket(true);
            seqNum=ackPacket.ackNum();
        }
    }

    file.close();

    // EOT 패킷 전송
    Packet eotPacket(EOT, seqNum);
    socketSupport.sendPacket(TRANSMISSION, eotPacket, true);

    Packet eotAckPacket=socketSupport.recvPacket(true);

    logger.log("Sender >> File Transmission Finish\n");

    // 소켓 close
    close(sockfd_send);
    close(sockfd_recv);
    
    exit(0);
}


// 송신 소켓을 설정한다.
int setSendSocket(const char* SENDER_IP, const int SENDER_PORT){
    int sockfd_send = socket(AF_INET, SOCK_DGRAM, 0);  // Socket의 파일 디스크립터
    if (sockfd_send < 0) {
        perror("[ERROR] Socket Creation Error\n");
        exit(errno);
    }

    struct sockaddr_in dest_send;
    memset(&dest_send, 0, sizeof(dest_send));
    dest_send.sin_family = AF_INET;
    dest_send.sin_port = htons(SENDER_PORT);  //포트 번호
    int inet_send=inet_aton(SENDER_IP, &dest_send.sin_addr);  // IP 주소
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
    int sockfd_recv = socket(AF_INET, SOCK_DGRAM, 0);  // Socket의 파일 디스크립터
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