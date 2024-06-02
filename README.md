데이터 통신 수업 과제물입니다.
<br/>
<br/>

## ✅ 프로그램 설명

Reliable Data Transfer을 지원하는 파일 전송 프로그램입니다.
UDP 소켓을 이용해서 TCP처럼 Packet Loss에 대처하도록 했습니다.

sender, receiver 프로그램은 RDT 프로토콜로 통신하며, stop-and-wait 모델을 사용합니다. Linux 시스템에서 동작하고, 같은 machine 내에서 통신하는 것을 가정합니다.

RDT 프로토콜은 Packet loss, Duplicate packets에 대한 처리를 제공합니다.
같은 machine 내에서는 일반적으로 Packet loss가 발생하지 않기 때문에 확률적으로 패킷을 버리도록 구현했습니다. 패킷의 손실 여부를 판단하기 위해서 타이머를 사용했습니다.
DATA 패킷 전송 후 일정 시간 동안 ACK 패킷이 도착하지 않으면 중간에 패킷이 손실된 것으로 간주하고 재전송합니다.

타이머의 빠른 종료로 인해 패킷이 손실되지 않았는데도 재전송하는 경우를 대비하기 위해, receiver측에서는 가장 최근에 받은 패킷의 시퀀스 번호를 기억해둬야 합니다. 이미 받은 패킷이 다시 전송되면 receiver은 해당 패킷을 폐기하고, 다시 ACK 패킷을 전송합니다.
<br/><br/>

## ✅ 실행 가이드

### ◾ 실행 조건
- __Linux__
- __localhost간 통신__
<br/>

### ◾ 라이브러리
```
sudo apt install libboost-all-dev
```
### ◾ 컴파일
```
make
```

### ◾ 실행
```
./receiver [receiver's port#] [drop probability]
./sender [sender's port#] [receiver's IP addr] [receiver's port#] [timeout interval] [filename] [drop probability]
```
./receiver 51000 0.25 & <br/>
./sender 51000 127.0.0.1 51001 5 file.txt 0.1 & <br/><br/>
<br/>

## ✅ 출력 예시

### ◾ 송수신
```
[2024-06-01 23:13:23.557] Sender -> Packet Transmission : DATA Packet(seq=3, ack=0)
[2024-06-01 23:13:23.557] Receiver -> Packet Reception : DATA Packet(seq=3, ack=0)
```
Sender가 전신한 3번 패킷을 Receiver가 수신하는 로그입니다.
Receiver은 다음에 요청할 패킷의 번호를 ackNum에 담아서 보낼 예정입니다.
<br/>

### ◾ 패킷 손실
```
[2024-06-01 23:13:23.557] Receiver -> Packet Loss : ACK Packet(seq=0, ack=4)
```
Receiver가 3번 패킷을 잘 받았다는 ACK 패킷을 Sender에게 전송하던 도중 Packet Loss가 발생합니다.
<br/>

### ◾ 타임아웃
```
[2024-06-01 23:13:28.557] Sender -> Timeout : DATA Packet(seq=3, ack=0)
```
Sender가 3번 패킷을 전송할 때 시작했던 타이머가 종료되면서 Timeout이 발생합니다.
<br/>

### ◾ 재전송
```
[2024-06-01 23:13:28.558] Sender-> Packet Retransmission : DATA Packet(seq=3, ack=0)
[2024-06-01 23:13:28.558] Receiver-> Packet Reception : DATA Packet(seq=3, ack=0)
[2024-06-01 23:13:28.558] Receiver-> Packet Transmission : ACK Packet(seq=0, ack=4)
[2024-06-01 23:13:28.558] Sender-> Packet Reception : ACK Packet(seq=0, ack=4)
```
Sender 입장에서는 ACK 패킷을 받지 못했으므로 Receiver가 3번 패킷을 제대로 전달받지 못했다고 간주하여 3번 패킷을 다시 전송합니다. 이번에는 중간에 Packet loss가 발생하지 않아 무사히 3번 패킷에 대한 ACK를 전달받고, 타이머를 종료합니다.
<br/>
<br/>

### ◾ 전체 로그
```
[2024-06-01 23:13:23.553] Sender -> Packet Transmission : DATA Packet(seq=0, ack=0)
	length: 8
	data: Greeting
[2024-06-01 23:13:23.553] Receiver -> Packet Reception : DATA Packet(seq=0, ack=0)
	length: 8
	data: Greeting
[2024-06-01 23:13:23.553] Receiver -> Packet Transmission : ACK Packet(seq=0, ack=1)
[2024-06-01 23:13:23.553] Sender -> Packet Reception : ACK Packet(seq=0, ack=1)
[2024-06-01 23:13:23.553] Sender -> Packet Transmission : DATA Packet(seq=1, ack=0)
	length: 8
	data: file.txt
[2024-06-01 23:13:23.553] Receiver -> Packet Reception : DATA Packet(seq=1, ack=0)
	length: 8
	data: file.txt
[2024-06-01 23:13:23.554] Receiver -> Packet Transmission : ACK Packet(seq=0, ack=2)
[2024-06-01 23:13:23.554] Sender -> Packet Reception : ACK Packet(seq=0, ack=2)
[2024-06-01 23:13:23.554] Sender -> Packet Transmission : DATA Packet(seq=2, ack=0)
	length: 1024
	data: The Transmission Control Protocol (TCP) is one of the main protocols of the Internet protocol suite....
[2024-06-01 23:13:23.556] Receiver -> Packet Reception : DATA Packet(seq=2, ack=0)
	length: 1024
	data: The Transmission Control Protocol (TCP) is one of the main protocols of the Internet protocol suite....
[2024-06-01 23:13:23.557] Receiver -> Packet Transmission : ACK Packet(seq=0, ack=3)
[2024-06-01 23:13:23.557] Sender -> Packet Reception : ACK Packet(seq=0, ack=3)
[2024-06-01 23:13:23.557] Sender -> Packet Transmission : DATA Packet(seq=3, ack=0)
	length: 1024
	data: reliable data stream service may use the User Datagram Protocol (UDP) instead, 
which provides a con...
[2024-06-01 23:13:23.557] Receiver -> Packet Reception : DATA Packet(seq=3, ack=0)
	length: 1024
	data: reliable data stream service may use the User Datagram Protocol (UDP) instead, 
which provides a con...
[2024-06-01 23:13:23.557] Receiver -> Packet Loss : ACK Packet(seq=0, ack=4)
[2024-06-01 23:13:28.557] Sender -> Timeout : DATA Packet(seq=3, ack=0)
	length: 1024
	data: reliable data stream service may use the User Datagram Protocol (UDP) instead, 
which provides a con...
[2024-06-01 23:13:28.558] Sender -> Packet Retransmission : DATA Packet(seq=3, ack=0)
	length: 1024
	data: reliable data stream service may use the User Datagram Protocol (UDP) instead, 
which provides a con...
[2024-06-01 23:13:28.558] Receiver -> Packet Reception : DATA Packet(seq=3, ack=0)
	length: 1024
	data: reliable data stream service may use the User Datagram Protocol (UDP) instead, 
which provides a con...
[2024-06-01 23:13:28.558] Receiver -> Packet Transmission : ACK Packet(seq=0, ack=4)
[2024-06-01 23:13:28.558] Sender -> Packet Reception : ACK Packet(seq=0, ack=4)
[2024-06-01 23:13:28.558] Sender -> Packet Transmission : DATA Packet(seq=4, ack=0)
	length: 1024
	data:  links and datagram services between hosts. 
The monolithic Transmission Control Program was later d...
[2024-06-01 23:13:28.559] Receiver -> Packet Reception : DATA Packet(seq=4, ack=0)
	length: 1024
	data:  links and datagram services between hosts. 
The monolithic Transmission Control Program was later d...
[2024-06-01 23:13:28.559] Receiver -> Packet Transmission : ACK Packet(seq=0, ack=5)
[2024-06-01 23:13:28.559] Sender -> Packet Reception : ACK Packet(seq=0, ack=5)
[2024-06-01 23:13:28.559] Sender -> Packet Loss : DATA Packet(seq=5, ack=0)
	length: 865
	data: yer, TCP handles all handshaking and transmission details 
and presents an abstraction of the networ...
[2024-06-01 23:13:33.559] Sender -> Timeout : DATA Packet(seq=4, ack=0)
	length: 1024
	data:  links and datagram services between hosts. 
The monolithic Transmission Control Program was later d...
[2024-06-01 23:13:33.560] Sender -> Packet Retransmission : DATA Packet(seq=4, ack=0)
	length: 1024
	data:  links and datagram services between hosts. 
The monolithic Transmission Control Program was later d...
[2024-06-01 23:13:33.560] Receiver -> Packet Reception : DATA Packet(seq=4, ack=0)
	length: 1024
	data:  links and datagram services between hosts. 
The monolithic Transmission Control Program was later d...
[2024-06-01 23:13:33.560] Receiver -> Packet Loss : ACK Packet(seq=0, ack=5)
[2024-06-01 23:13:38.560] Sender -> Timeout : DATA Packet(seq=4, ack=0)
	length: 1024
	data:  links and datagram services between hosts. 
The monolithic Transmission Control Program was later d...
[2024-06-01 23:13:38.560] Sender -> Packet Retransmission : DATA Packet(seq=4, ack=0)
	length: 1024
	data:  links and datagram services between hosts. 
The monolithic Transmission Control Program was later d...
[2024-06-01 23:13:38.560] Receiver -> Packet Reception : DATA Packet(seq=4, ack=0)
	length: 1024
	data:  links and datagram services between hosts. 
The monolithic Transmission Control Program was later d...
[2024-06-01 23:13:38.561] Receiver -> Packet Transmission : ACK Packet(seq=0, ack=5)
[2024-06-01 23:13:38.561] Sender -> Packet Reception : ACK Packet(seq=0, ack=5)
[2024-06-01 23:13:38.561] Sender -> Packet Transmission : EOT Packet(seq=5, ack=0)
[2024-06-01 23:13:38.561] Receiver -> Packet Reception : EOT Packet(seq=5, ack=0)
[2024-06-01 23:13:38.561] Receiver -> Packet Transmission : ACK Packet(seq=0, ack=6)
[2024-06-01 23:13:38.561] Sender -> Packet Reception : ACK Packet(seq=0, ack=6)
[2024-06-01 23:13:38.561] Sender >> File Transmission Finish
[2024-06-01 23:13:38.565] Receiver >> File Transmission Finish

```

