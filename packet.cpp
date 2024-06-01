#include "packet.h"

// 직렬화 (object -> string)
std::string Packet::serialize(Packet& packet){
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << packet;
    return ss.str();
}

// 역직렬화 (string -> object)
Packet Packet::deserialize(std::string& packetString){
    Packet packet;
    std::stringstream ss(packetString);
    boost::archive::text_iarchive ia(ss);
    ia >> packet;
    return packet;
}