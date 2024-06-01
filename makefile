CC = gcc
LIBS = -lboost_serialization -lstdc++

all: sender receiver

sender: sender.cpp packet.cpp logger.cpp socketSupport.cpp alarm.cpp
	$(CC) -o sender sender.cpp packet.cpp logger.cpp socketSupport.cpp alarm.cpp $(LIBS)

receiver: receiver.cpp packet.cpp logger.cpp socketSupport.cpp alarm.cpp
	$(CC) -o receiver receiver.cpp packet.cpp logger.cpp socketSupport.cpp alarm.cpp $(LIBS)

clean:
	rm -f sender receiver