#ifndef ALARM_H

#define ALARM_H

class Alarm{
public:
    static void onSendTimeout(int signal);
    static void startSendTimer(int seconds);
    
    static void onRecvTimeout(int signal);
    static void startRecvTimer(int seconds);

    static void stopTimer();
};

#endif