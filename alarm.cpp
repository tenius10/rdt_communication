
#include <csignal>
#include <sys/time.h>

#include "alarm.h"
#include "socketSupport.h"


void Alarm::onSendTimeout(int signal){
    SocketSupport::instance->handleSendTimeout();
}

void Alarm::startSendTimer(int seconds){
    signal(SIGALRM, onSendTimeout);
    alarm(seconds);
}


void Alarm::onRecvTimeout(int signal){
    exit(0);
}

void Alarm::startRecvTimer(int seconds){
    signal(SIGALRM, onRecvTimeout);
    alarm(seconds);
}


void Alarm::stopTimer(){
    signal(SIGALRM, SIG_IGN);  // 핸들러 해제
    alarm(0);
}