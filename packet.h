#ifndef PACKET_H

#define PACKET_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


enum PacketType{ DATA, ACK, EOT };

class Packet {
    friend class boost::serialization::access;

private:
    PacketType type_;
    int seqNum_;
    int ackNum_;
    int length_;
    std::string data_;

public:
    Packet(){}
    Packet(PacketType type, int seqNum):type_(type){
        if(type==ACK) {
            // ACK 패킷의 경우 seqNum을 사용하지 않음
            seqNum_=0;
            ackNum_=seqNum;
        }
        else{
            seqNum_=seqNum;
            ackNum_=0;
        }
    }
    Packet(PacketType type, int seqNum, std::string data):Packet(type, seqNum){
        length_=data.length();
        data_=data;        
    }
    
    PacketType type(){ return type_; }
    int seqNum(){ return seqNum_; }
    int ackNum(){ return ackNum_; }
    int length(){ return length_; }
    std::string data(){ return data_; }

    void setData(std::string data){ data_=data; }
    void setLength(int length){ length_=length;}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & type_;
        ar & seqNum_;
        ar & ackNum_;
        ar & length_;
        ar & data_;
    }

    // 직렬화 (object -> string)
    static std::string serialize(Packet& packet);

    // 역직렬화 (string -> object)
    static Packet deserialize(std::string& packetString);
};

#endif