#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <deque>
#include <queue>
#include <vector> 
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "md5.h"


using namespace std;

priority_queue<Packet, vector<Packet>, Comparator> packetBuffer;
vector<int> recievedAcks;
sem_t threadLimiterR, threadRead;

void *client_receive_thread(void* ptr);

class Receiver {
	public:
	Receiver();
	void run();
		
	private:
	int nextToWrite;
	int numPackets;
	int maxSequenceNumber;
	
};

Receiver::Receiver()
{ 
}

void Receiver::run() 
{
	int RWS;
	//the first thing we need to do is get the number of characters from the sender
	//then we need to loop through and obtain acks until we have written all of them
	//however, once we receive a packet and we confirm that it is within the window,
	//we must send back an ack immediately.
	cout << "Please enter the RWS: ";
	cin >> RWS;
	
	UdpSocket s1("thing-0.cs.uwec.edu", "thing-1.cs.uwec.edu");
	Udpsocket2 s2("thing-1.cs.uwec.edu", "thing-0.cs.uwec.edu");
	s1.openSocket();
	int tmpSize;
	
	//initialize the ack vector
	
	char* tmpRecvRP;
	s1.listener(tmpSize,tmpRecvRP);
	Packet tmpPacket(tmpRecvRP,tmpSize);
	numPackets = tmpPacket.getAck();
	maxSequenceNumber = tmpPacket.getLocation();
	for(int i = 0; i < numPackets; i++){
		recievedAcks.push_back(0);
	}
	cout << "NumPackets Recvd: " << numPackets << " MaxSeqNum: "<< maxSequenceNumber <<endl;
	FileWriter outFile ("/tmp/AHAHHAHAHAHAHHAAHA.txt",""); //Open the FileWriter
	//FileWriter outFile ("oldfile",""); 
	
	nextToWrite = 0;
	while(nextToWrite < numPackets){
		char* incoming;
		int packSize;
		s1.openSocket();
		s1.listener(packSize,incoming);
		Packet tmpP1(incoming, packSize);
		int countGrab = 0;
		int countLoop = 0;
		while(countLoop < 4) {
			if(incoming[countGrab] == '|') {
				countLoop = countLoop + 1;	
			}
			countGrab = countGrab + 1;
		}
		//char* tempIncoming = new char[tmpP1.getSize()+1];
		string tempIncoming = "";
		int countInsert = 0;
		for(int y = countGrab; y < packSize; y++) {
			tempIncoming += incoming[y];
			//countInsert = countInsert + 1;
		}
		Packet p1(tmpP1.getAck(),tmpP1.getSize(),tmpP1.getLocation(),tmpP1.isCorrupt(),tempIncoming);
		
		countLoop = 0;
		countGrab = 0;

		free(incoming);
		//rules for discarding packet...this discards any previously recieved packets
		if(p1.isCorrupt() == 0){
			int location = p1.getLocation();
			
			int LFR = (nextToWrite - 1) + RWS;
			if((nextToWrite - 1) < location && location <= LFR) {
				//check and see if it is in the ack range
				//we also need to maintain a vector of ints to hold these values.
				if(recievedAcks[location] == 0){
					#ifdef TESTING
						cout << "[";
							for(int i = nextToWrite; i <= LFR; i++) {
								cout << i << " ";
							}
						cout << "]" << endl;
					#endif
					//packet is good to be put into the q;
					//hold the packet until it is able to be written
					recievedAcks[location] = 1;
					s2.openSocket();
					//send back the ack because it is a good packet and send it back
					string ackSendString = boost::lexical_cast<string>(p1.getAck()) + "|3|0|0|ACK";
					int shouldDrop = 0;
					#ifdef DEBUG
						cout << "Dr. Tan would you like to drop this ack? 1 = yes 0 = no" << endl;
						cin >> shouldDrop;
					#endif 
					if(shouldDrop == 0){
						#ifdef TESTING
							cout << "Sending Ack: " << ackSendString << endl;
						#endif
						s2.sendData(ackSendString, ackSendString.size());
					} else {
						cout << "Dropped Packet" << endl;
					}
					packetBuffer.push(p1);
				} else{
				}
			} else {
				//send back ack for that location because we alreadey got the ack
				if(location < nextToWrite){
					cout << "ALREADY RECEIVED ACK...RESENDING ACK " << 	p1.getAck() << endl;;
					s2.openSocket();
					string ackSendString = boost::lexical_cast<string>(p1.getAck()) + "|3|0|0|ACK";
					s2.sendData(ackSendString, ackSendString.size());
				} else {
					//do nothing because the ack is out of range
					cout << "THE ACK IS OUT OF RANGE DISCARDING PACKET" << endl;
				}
			}
			//check and see if we want to write out the file
			Packet possibleWrite;
			if(!packetBuffer.empty()){
				possibleWrite = packetBuffer.top();
				int keepRunning = 0;
				while(keepRunning == 0){
					if(possibleWrite.getLocation() == nextToWrite){
						outFile.append(possibleWrite.getData(), possibleWrite.getSize());
						packetBuffer.pop();
						nextToWrite++;
						//set the possible write to the new top
						if(packetBuffer.empty()){
							keepRunning = 1;
						} else{
							possibleWrite = packetBuffer.top();
						}
					} else {
						keepRunning = 1;
					}
				}
			}
		} else {
			cout << "PACKET IS CORRUPT THROW IT AWAY!" << endl;
		}
	}
	outFile.close();
}
