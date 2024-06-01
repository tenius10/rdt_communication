#ifndef LOGGER_H

#define LOGGER_H

#include <fstream>

#include "packet.h"

enum Event{ TRANSMISSION, RECEPTION, DISCARD, TIMEOUT, RETRANSMISSION };

class Logger{
private:
    std::string who_;
    
    std::string getCurrentTime();
    
    std::string logFilename_;

public:
    Logger(std::string who, std::string logFilename);

    void log(Packet packet, Event event);

    void log(std::string msg);
};

#endif