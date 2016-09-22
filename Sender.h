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
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include <queue>
#include "md5.h"
#include "UdpSocket.h"
#include "UdpSocket2.h"
#include "Comparator.h"

//#define DEBUG
#define TESTING

using namespace std;

sem_t stopListening;
vector<int> waitingAcks;

int numPackets;
unsigned int SWS;
unsigned int LAR;
int basePacket;


void *recieve_thread(void* ptr);


class Sender {
	public:
		Sender();
		void run();
		
	private:
		string fileToSend;
		int kilobyte;
		int packetSize;
		int numPacketsOverFlow;
		int acksToDrop;
		int numCorrupt;
		int packetTimeOutTime;
		double percentAcksToDrop;
		double percentCorrupt;
		deque<Packet> packetQueue;
		
		FileReader inFile;
		priority_queue<Packet, vector<Packet>, Comparator> packetBuffer;
};

Sender::Sender()
{
}

void Sender::run()
{	
	cout << "Please enter the SWS: ";
	cin >> SWS;
	cout << "Please give the packet size. (In kb)" << endl;
	int packetSize; // in kilobytes
	cin >> packetSize;
	cout << "Please give the packet timeout time in nanosecods" << endl;
	cin >> packetTimeOutTime;
	LAR = 0;
	string fileToSend = "/tmp/1Gtestfile";
	//string fileToSend = "newfile";
	
	//60 kilobytes
	FileReader inFile(fileToSend, packetSize);
	//determines how many packets we will need
	int kilobyte = 1024;
	int numPackets = inFile.getFileSize() / (kilobyte * packetSize);
	int numPacketsOverFlow = inFile.getFileSize() % (kilobyte * packetSize);
	if (numPackets == 0) {
		numPackets = 1; //incase the file is smaller than the overall packet size.
	} else if (numPacketsOverFlow != 0) {
		numPackets = numPackets + 1; //we will most likely need another packet
	}
	cout << "The number of packets needed is: " << numPackets << endl;
	//number of acks
	cout << "Now you know the number of packets needed, please input the Max Seq Num " << endl;
	unsigned int maxSeqNum = 6;
	cin >> maxSeqNum;
	
	//populate the ack array with zeros.
	for(unsigned int i = 0; i < maxSeqNum; i++){
		waitingAcks.push_back(0);
	}
	
	UdpSocket s2("thing-0.cs.uwec.edu", "thing-1.cs.uwec.edu");
	s2.openSocket();
	
	//first we must send the number of packets
	string numPacs = boost::lexical_cast<string>(numPackets) + "|3|" + boost::lexical_cast<string>(maxSeqNum) + "|0|pac";
	s2.sendData(numPacs.c_str(), numPacs.size());
	pthread_t recieverThread; //Receiver thread
	//Start the reciever thread
	int recieveThreadRet = pthread_create( &recieverThread, NULL, recieve_thread, (void*) 1);
	cout << "The thread status of recieving Thread: " << recieveThreadRet << endl;
	
	cout << "NumPackets " << numPackets << endl;
	unsigned int currentSeq = 0;
	for(int i = 0; i < numPackets; i++){
		int numChars = inFile.getNumChars();
		char* buf;
		inFile.getNextBlock(buf);
		Packet p1(currentSeq, numChars, i, 0, buf);
		p1.setStartTime();
		//always pushes the next packet to the back of the queue
		packetQueue.push_back(p1);
		free(buf);
		//working un-corrupted packet
		
		#ifdef TESTING
		if(packetQueue.size() == SWS){
			cout << "["; 
			for(unsigned int i = 0; i < packetQueue.size(); i++){
				if(i == (packetQueue.size() - 1)){
					cout << packetQueue[i].getAck() << "..." << packetQueue[i].getLocation();
				}else{
					cout << packetQueue[i].getAck() << "..." << packetQueue[i].getLocation() << ", ";
				}
			}
			cout << "]" << endl;
		}
		#endif
		
		while(packetQueue.size() == SWS){
			//here we have to get the current Ack from the listener;
			//if we reeive something back; kill the packet in the q to stop it from sending
			//restart, this will force the for loop to get the next packet until window size shirinks below
			//the max sequence number
			//if the packet is in the q for the first time	
			int seqNum = packetQueue.front().getAck();
				if(packetQueue.front().getTimesInside() == 0) {
					int runNormal = 0;
					int badPacket = 0;
					#ifdef DEBUG
						cout << "Dr. Tan, Would you like to drop this packet? 0 = no, 1 = yes" << endl;
						cin >> runNormal;
					#endif
					#ifdef DEBUG
						cout << "Dr. Tan, Would you like to corrupt this packet? 0 = no, 1 = yes" << endl;
						cin >> badPacket;
					#endif
					waitingAcks[seqNum] = 1;
					if(badPacket != 0 && runNormal == 0){
						Packet badPac = packetQueue.front(); 
						badPac.setmd5Sum("1");
						s2.sendData(badPac.getSerializedString(), packetQueue.front().getSerialSize());
						#ifdef TESTING
							cout << "Sending Packet " <<  packetQueue.front().getAck() << " Current Location = " << packetQueue.front().getLocation() << " Time Elapsed: " << "0" << endl;
						#endif
					}
					if(runNormal == 0 && badPacket == 0){
						s2.sendData(packetQueue.front().getSerializedString(), packetQueue.front().getSerialSize());
						#ifdef TESTING
							cout << "Sending Packet " <<  packetQueue.front().getAck() << " Current Location = " << packetQueue.front().getLocation() << " Time Elapsed: " << "0" << endl;
						#endif
					} 
					packetQueue.front().setTimesInside(1);
					packetQueue.front().setStartTime();
				} else if(packetQueue.front().getTimesInside() != 0 && packetQueue.front().getTimeElapsed() > packetTimeOutTime) {
					int seqNum = packetQueue.front().getAck();
					if(waitingAcks[seqNum] == 0) {
						packetQueue.pop_front();
						LAR++;
						waitingAcks[seqNum] = 1;
						#ifdef TESTING
							cout << "Received Ack, popping packet: " << packetQueue.front().getAck() << endl;
						#endif
					} else{
						#ifdef TESTING
							cout << "Sending Packet " <<  packetQueue.front().getAck() << " Current Location = " << packetQueue.front().getLocation() << " Time Elapsed: " << packetQueue.front().getTimeElapsed() << endl;
						#endif
						s2.sendData(packetQueue.front().getSerializedString(), packetQueue.front().getSerialSize());
						//waitingAcks[packetQueue.front().getAck()] = 0;
						packetQueue.front().setStartTime();
						//sleep(1);
					}
				}
				//cout << "IAM HERE!" << endl;
			if(!packetQueue.empty()){
				Packet p2 = packetQueue.front();
				packetQueue.pop_front();
				packetQueue.push_back(p2);
			}
		}
		if(currentSeq == maxSeqNum){
			currentSeq = 0;
		}else{
			currentSeq++;
		}
	}
	inFile.closeFile(); 
//second loop to finish out the remaining packets
	
	while(!packetQueue.empty()){
		if(packetQueue.front().getTimeElapsed() > packetTimeOutTime){
			if(waitingAcks[packetQueue.front().getAck()] == 0) {
				waitingAcks[packetQueue.front().getAck()] = 1;
				#ifdef TESTING
					cout << "Received Ack, popping packet: " << packetQueue.front().getAck() << endl;
				#endif
				packetQueue.pop_front();
			} else {
				#ifdef TESTING
					cout << "Sending Packet " <<  packetQueue.front().getAck() << " Time Elapsed: " << packetQueue.front().getTimeElapsed() << endl;
				#endif
				s2.sendData(packetQueue.front().getSerializedString(), packetQueue.front().getSize());
				packetQueue.front().setStartTime();
			}
		} else {
			//cout << "IAM HERE!" << endl;
			if(!packetQueue.empty()){
				Packet p2 = packetQueue.front();
				packetQueue.pop_front();
				if(waitingAcks[p2.getAck()] != 0){
					packetQueue.push_back(p2);
				}
			}
		}
	}
	inFile.closeFile(); 
}

void *recieve_thread(void* ptr) {
	while(true) {
		//need a semaphore here to stop listening until the we handle dismissing the ack.
		//stop listening until the sender fixes the amount of packets being sent.
		Udpsocket2 r1("thing-0.cs.uwec.edu", "thing-1.cs.uwec.edu");
		r1.openSocket();
		int size;
		char* recvAck;
		r1.listener(size,recvAck);
		Packet ack(recvAck,size);
		if(waitingAcks[ack.getAck()] == 1){
			waitingAcks[ack.getAck()] = 0;
		}
		#ifdef TESTING
			cout << "Returned Ack " << ack.getAck() << endl;
		#endif
		free(recvAck);
	}
	return 0;
}
