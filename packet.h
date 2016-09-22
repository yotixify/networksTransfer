#include<sstream>
#include <vector>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <string>
#include "md5.h"
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#include <boost/lexical_cast.hpp>
#include <functional>

using namespace std;

class Packet
{
public:
	//size is in kilobytes
	Packet(int ack, int size, int location, int corrupt, char* data);
	Packet(int ak, int sz, int loc, int corrupt, string dt);
	Packet(string serializedData, int serSize);
	Packet& operator=(const Packet& packet); //Assignment Constructor
	Packet();
	void serialize();
	void setStartTime();
	void setTimesInside(int i);
	long getTimeElapsed();
	int getTimesInside();
	void setmd5Sum(string i);
	string getData();
	string getmd5Sum();
	string getSerializedString();
	int getAck();
	int getSize();
	int getLocation();
	int getSerialSize();
	int isCorrupt();
	
private:
	string data;
	string md5Sum;
	string serializedString;
	int location;
	int ack;
	int size;
	int serialSize;
	int timesInside;
	clock_t start;
	clock_t finish;
};

Packet::Packet()
{

}

Packet& Packet::operator=(const Packet& otherPacket)
{
	location = otherPacket.location;
	ack = otherPacket.ack;
	size = otherPacket.size;
	serialSize = otherPacket.serialSize;
	//copy the serialized data
	data = otherPacket.data;
	serializedString = otherPacket.serializedString;
	md5Sum = otherPacket.md5Sum;
	timesInside = 0;
	return *this;
}

Packet::Packet(int ak, int sz, int loc, int corrupt, string dt)
{
	md5Sum = boost::lexical_cast<string>(corrupt);
	//cout << md5Sum << endl;
	//for(int i = 0; i < sz; i++) {
	//	data += dt[i];
	//}
	data = dt;
	//data = dt;
	ack = ak;
	size = sz;
	location = loc;
	serialize();
	timesInside = 0;
	//cout << "Made new Packet" << endl;
}

Packet::Packet(int ak, int sz, int loc, int corrupt, char* dt)
{
	md5Sum = boost::lexical_cast<string>(corrupt);
	//cout << md5Sum << endl;
	for(int i = 0; i < sz; i++) {
		data += dt[i];
	}
	//data = dt;
	ack = ak;
	size = sz;
	location = loc;
	serialize();
	timesInside = 0;
	//cout << "Made new Packet" << endl;
}

Packet::Packet(string sd, int serSize)
{	
	timesInside = 0;
	serialSize = serSize;
	serializedString = sd;
	//cout << "Constructing new Packet from serialized data" << endl;	
	//deserialize the string
	//four variables before data
	//store as string
	string ak;
	string sz;
	string loc;
	string m5;
	string dt;
	//loop until we get the number as to where to start writing data
	int numPipes = 0;
	unsigned int i;
	for(i = 0; numPipes < 4; i++){
		if (numPipes == 3) {
			if (serializedString[i] == '|') {
				md5Sum = string(m5);
				//cout << md5Sum << endl;
				numPipes = 4;
			} else {
				m5 += serializedString[i];
			}
		} else if(numPipes == 2) {
			if (serializedString[i] == '|') {
				location = boost::lexical_cast<int>(loc);
				numPipes = 3;
			} else {
				loc += serializedString[i];
			}
		} else if(numPipes == 1) {
			if (serializedString[i] == '|') {
				size = boost::lexical_cast<int>(sz);
				numPipes = 2;
			} else {
				sz += serializedString[i];
			}
		} else if(numPipes == 0){
			if (serializedString[i] == '|') {
				ack = boost::lexical_cast<int>(ak);
				numPipes = 1;
			} else {
				ak += serializedString[i];
			}
		}
	}
	//loop through and get all the data
	unsigned int dataSize = i + size;
	while(i < dataSize){
		data += serializedString[i];
		i++;
	}
	//data += '\0';
}

//condences the whole packet into a single string 
void Packet::serialize()
{
	serializedString = boost::lexical_cast<string>(ack) + "|" + boost::lexical_cast<string>(size) + "|" + boost::lexical_cast<string>(location) + "|" + md5Sum + "|";
	serialSize = serializedString.size();
	for(int i = 0; i < size; i++){
		serializedString += data[i];
		serialSize++;
	}
}

string Packet::getData()
{
	return data;
}

string Packet::getmd5Sum()
{
	return md5Sum;
}

void Packet:: setmd5Sum(string i)
{	
	md5Sum = i;
	serialize();
}

string Packet::getSerializedString()
{
	return serializedString;
}

int Packet::getAck()
{
	return ack;
}
int Packet::getSize()
{
	return size;
}
int Packet::getLocation()
{
	return location;
}
int Packet::getSerialSize()
{
	return serialSize;
}

void Packet::setStartTime()
{
	start = clock();
}

long Packet::getTimeElapsed()
{
	return (long)((std::clock() - start) / (double)(CLOCKS_PER_SEC / 100));
}

void Packet::setTimesInside(int i)
{
	timesInside = i;
}

int Packet::getTimesInside()
{
	return timesInside;
}

int Packet::isCorrupt(){
	return boost::lexical_cast<int>(md5Sum);
}
