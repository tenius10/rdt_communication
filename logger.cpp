#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>

#include "logger.h"
#define DATA_DISPLAY_LIMIT 100

using namespace std;

Logger::Logger(std::string who, std::string logFilename):who_(who), logFilename_(logFilename){
    // 로그 파일 내용 비우기
    ofstream logFile(logFilename);
    logFile<<"";
    logFile.close();
}

string Logger::getCurrentTime(){
    time_t currentTime = time(nullptr);
    tm* localTime = localtime(&currentTime);

    // YYYY-MM-dd hh:mm:ss.SSS
    char formattedTime[24];
    strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", localTime);

    // millisecond 계산
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long milliseconds = ts.tv_nsec / 1000000;

    string result = formattedTime;
    result += "." + to_string(milliseconds);

    return result;
}

void Logger::log(Packet packet, Event event){
    // time
    string log="["+getCurrentTime()+"] ";  
    
    // who
    log+=who_+" -> ";  

    // event
    switch(event){
        case TRANSMISSION:
            log+="Packet Transmission : ";
            break;
        case RECEPTION:
            log+="Packet Reception : ";
            break;
        case DISCARD:
            log+="Packet Loss : ";
            break;
        case TIMEOUT:
            log+="Timeout : ";
            break;
        case RETRANSMISSION:
            log+="Packet Retransmission : ";
            break;
    }

    // packet type
    switch(packet.type()){
        case DATA:
            log+="DATA ";
            break;
        case ACK:
            log+="ACK ";
            break;
        case EOT:
            log+="EOT ";
            break;
    }

    // packet
    log+=("Packet(seq="+to_string(packet.seqNum())+", ack="+to_string(packet.ackNum())+")\n");  

    if(packet.type()==DATA){
        log+=("\tlength: "+to_string(packet.length())+"\n");
        log+=("\tdata: "+packet.data().substr(0, DATA_DISPLAY_LIMIT)+(packet.length()>DATA_DISPLAY_LIMIT?"...\n":"\n"));
    }

    ofstream logFile(logFilename_, ios::app);
    logFile<<log;
    logFile.close();
}

void Logger::log(std::string msg){
    ofstream logFile(logFilename_, ios::app);
    logFile<<"["+getCurrentTime()+"] "+msg;
    logFile.close();
}
