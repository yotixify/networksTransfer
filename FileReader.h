#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include "main.h"
#include <stdio.h>

using namespace std;

class FileReader
{
public:
	FileReader(string fname, int packetSize);
	FileReader();
	~FileReader();
	void getNextBlock(char* &buf);
	long getFileSize();
	int getCurrentPosition();
	int getBlockSize();
	int getNumChars();
	void closeFile();
	
private:
	int blockSize;
	string fileName;
	int currentPosition;
	char* buffer;
	long fileSize; //in bytes
	ifstream in_file;
	int numChars;
	int currentBlock;
	int packetsNeeded;
};

FileReader::FileReader(){
	fileName = "";
	buffer = (char*)malloc(5 * sizeof(char*));
}

FileReader::FileReader(string fname, int packetSize)
{
	fileName = fname;
	in_file.open(fname.c_str(), ios::binary);
	if(in_file.fail()){
		cout << "Bad file path. Failed to open file." << endl;
		exit(1);
	}
	//finds and sets the file size
	buffer = (char*)malloc(5 * sizeof(char*));
	buffer[0] = '\0';
	long begin = in_file.tellg();
	in_file.seekg (0, ios::end);
	long end = in_file.tellg();
	fileSize = end - begin;
	//reset the file pointer and set the current position to zero
	in_file.clear();
	in_file.seekg(0);
	currentPosition = 0;
	currentBlock = 1;
	blockSize = packetSize * 1024;
	packetsNeeded = fileSize / blockSize;
	if((fileSize % blockSize) != 0){
		packetsNeeded++;
	}
	numChars = blockSize;
	cout << "The size of the file is " << fileSize << " characters." << endl;
}

FileReader::~FileReader()
{
	//buffer = NULL;
	free(buffer);
}

void FileReader::getNextBlock(char* &buf){
	buffer = (char*)realloc(buffer, blockSize + 1);
	currentPosition = in_file.tellg();
	
	//sets the position of the next read
	currentPosition = currentPosition + blockSize;
	if(currentPosition > fileSize){
		buffer = (char*)calloc((currentPosition - fileSize + 1),sizeof(char*));
		in_file.readsome(buffer,numChars);
		buffer[numChars] = '\0';
	} else {
		in_file.readsome(buffer,blockSize);
		buffer[blockSize] = '\0';
	}
	currentBlock++;
	if(currentBlock == packetsNeeded){
		//numChars = fileSize - (blockSize *  (packetsNeeded - 1));
		numChars = fileSize - currentPosition;
	}
	buf = (char*)malloc(blockSize * sizeof(char));
	for(int i = 0; i < blockSize; i++){
		buf[i] = buffer[i];
	}
}

int FileReader::getCurrentPosition(){
	return currentPosition;
}

int FileReader::getBlockSize(){
	return blockSize;
}
int FileReader::getNumChars(){
	return numChars;
}


long FileReader::getFileSize(){
	return fileSize;
}

void FileReader::closeFile(){
	in_file.close();
}
