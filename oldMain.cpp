#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include "main.h"
#include <stdio.h>
#include "packet.h"
#include "FileReader.h"
#include "FileWriter.h"
#include <deque>

using namespace std;
int main() {
  string fileToSend = "theOdyssey.txt";
  //60 kilobytes
  int packetSize = 60; // in kilobytes
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
  
  deque<Packet> packetsToSend;
  
FileWriter outFile("TheOdd.txt", "");
for(int i = 0; i < numPackets; i++){
	string s = "This is test";
	
	//Have to pull out the get next block because of Shiva and thingies differences in g++ compilers
	int numChars = inFile.getNumChars();
	Packet p1(i,numChars,0,inFile.getNextBlock());
	packetsToSend.push_back(p1);
	
}
  //outFile.append(inFile.getNextBlock());
  //cout << woot << endl; 
  
  //close the file at the end
  inFile.closeFile(); 
} 
//  Packet p1(0, 111, 222, "The MD5", "The data | tes|t on|e |||");
//  string s = p1.serialize();
//  cout << s << endl;
  
  //create a packet from the serialized data
//  Packet p2(s);
//  string s2 = p2.serialize();
//  cout << s2 << endl;
