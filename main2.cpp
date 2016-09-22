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
#include "main.h"
#include "packet.h"
#include "udpsocket.h"
#include "comparator.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "Sender.h"



using namespace std;
using namespace boost;

int RWS;
int LAP;
int maxSeqNum = 10;

priority_queue<Packet, vector<Packet>, Comparator> packetBuffer;
sem_t threadLimiterR, threadRead;

struct receive_packet {
	char* serializedPacket;
	int packetSize;
};

void *client_receive_thread(void* ptr);

int main() {
	
	int input;
	cout << "If client press 1, server press 0" << endl;
	cin >> input;
	
	//start server
	if (input == 0) {
		Sender send;
		send.run();
	} else { //start client
		cout << "Please enter the RWS" << endl;
		cin >> RWS;
		LAP = RWS;
		int ackDropPercent;
		cout << "Please enter the Max Sequence Number" << endl;
		cin >> maxSeqNum;
		
		cout << "What percentage of acks dropped would you like?" << endl;
		cin >> ackDropPercent;
		
		UdpSocket s1("thing-0.cs.uwec.edu", "thing-1.cs.uwec.edu");
		s1.openSocket();
		int tmpSize;
		char* tmpRecvRP = NULL;
		s1.listener(tmpSize,tmpRecvRP);
		Packet tmpPacket(tmpRecvRP,tmpSize);
		numPackets = tmpPacket.getAck();
		cout << "NumPackets Recvd: " << numPackets << endl;
		FileWriter outFile ("TheOdd.txt",""); //Open the FileWriter
		sem_init(&threadLimiterR, 0, 1);
		
		ackArray = new int[numPackets];
		fill_n(ackArray,numPackets, 0);
		int windowSize = 1;
		int countWritten = 0; //Counts the number of packets currently written
		
		
		for(int x = 0; x < windowSize; x++) {
			pthread_t recieveThread;
			int ret1 = pthread_create( &recieveThread, NULL, client_receive_thread, (void*) &s1);
			ret1++;
		}
		while(countWritten < numPackets) {
			if(!packetBuffer.empty()) {
				Packet tempP = packetBuffer.top();
				if(tempP.getAck() == countWritten) {
					cout << tempP.getData() << endl;
					//Write Packet
					outFile.append(tempP.getData());
					countWritten = countWritten + 1;
					cout << "Count Written: " << countWritten << endl;
					packetBuffer.pop();
				} else {
					packetBuffer.push(tempP);
				}
			}
		}
		outFile.close();
	}
	
	//FileWriter outFile("TheOdd.txt", "");
	//for(int i = 0; i < numPackets; i++){
	//	string s = "This is test";
		//Have to pull out the get next block because of thing-0 and thingies differences in g++ compilers
	//	int numChars = inFile.getNumChars();
	//	Packet p1(i,numChars,0,inFile.getNextBlock());
		
	//}

}

void *client_receive_thread(void* ptr) {
	UdpSocket r1 = *((UdpSocket *) ptr);
	UdpSocket s2("thing-0.cs.uwec.edu", "thing-1.cs.uwec.edu");
	cout << "Client thread created" << endl;
	while(true) {
		//Listen for a packet from the server
		sem_wait(&threadLimiterR);
		r1.openSocket();
		int size;
		char* recvRP;
		r1.listener(size,recvRP);
		Packet rp(recvRP,size);
		cout << recvRP << endl;
		cout << "SIZE OF " << size << endl;
		
		
		sem_post(&threadLimiterR);
		//Once a packet is recieved, push it to the packetBuffer
		//cout << rp.getSize() << endl;
		int lowest = LAP - RWS;
		if(rp.getAck() >= lowest && rp.getAck() <= LAP) { //Check to see if it is in the RWS
			if(rp.getAck() == lowest) {
				LAP = LAP + 1;
			}
			cout << "Inserting: " << rp.getAck() << endl;
			packetBuffer.push(rp);
			if(rp.getAck() == maxSeqNum) {
				LAP = RWS;
			}
			//Send back an ack to the server with the recieved ack #
			string ackS = "ACK";
			int ackval = rp.getAck();
			int acksize = rp.getSize();
			int location = 0;
			ostringstream firstPart;
			firstPart << ackval << "|" << 3 << "|" << location << "|11111111111111111111111111111111|" << "ack";
			cout << "RECVD ACK: " << rp.getAck() << endl;
			//Packet ack(ackval, acksize, location, (char*)ackS.c_str());
			cout << firstPart.str() << endl;
			s2.sendData((char*)firstPart.str().c_str(), strlen((char*)firstPart.str().c_str()));
		} else {
			cout << "OUT OF WINDOW...IGNORING" << endl;
		}
		cout << "RWS: " << RWS << endl;
		cout << "LAP: " << LAP << endl;
		cout << "SEQNUM: " << rp.getAck() << endl;
	}
	
	return 0;
}


