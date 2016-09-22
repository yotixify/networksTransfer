#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include "main.h"
#include <stdio.h>
#include "packet.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "Sender.h"
#include "Receiver.h"
#include <deque>


#define TESTING
//#define DEBUG

using namespace std;
int main() {
   cout << "Please enter a 0 if this is server" << endl;
   int input;
   cin >> input;
   if(input == 0){
	Sender s1;
	s1.run();
   } else {
	Receiver r1;
	r1.run();
   }
}
//deque<Packet> packetsToSend;
/*  
FileWriter outFile("TheOdd.txt", "");
FileReader inFile("newfile",1);
Packet();
for(int i = 0; i < 10; i++){
	
	//Have to pull out the get next block because of Shiva and thingies differences in g++ compilers
	int numChars = inFile.getNumChars();
	char * temp;
	inFile.getNextBlock(temp);
	Packet p1(i,numChars,0,0,temp);
	Packet p2(p1.getSerializedString(), p1.getSize());
	free(temp);
	outFile.append(p2.getData(), p2.getSize());
	
}
  //outFile.append(inFile.getNextBlock());
  //cout << woot << endl; 
  
  //close the file at the end
  inFile.closeFile(); 
} 
*/
/*
//  Packet p1(0, 111, 222, "The MD5", "The data | tes|t on|e |||");
//  string s = p1.serialize();
//  cout << s << endl;
  
  //create a packet from the serialized data
//  Packet p2(s);
//  string s2 = p2.serialize();
//  cout << s2 << endl;
*/
