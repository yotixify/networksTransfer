#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include "main.h"
#include <stdio.h>
#include <boost/lexical_cast.hpp>

using namespace std;

class FileWriter
{
	public:
		FileWriter(string fname, string fileLocation); //if location is null it just writes to the current directory
		void append(string block, int size);
		void close();
		
	private:
		string fileName;
		string fileLocation;
		fstream outFile;
};

FileWriter::FileWriter(string fName, string fLoc)
{
	fileName = fName;
	fileLocation = fLoc;
	string filePath = fLoc + fName;
	outFile.open(filePath.c_str(), ios::out|ios::binary);
	outFile.close();
}

void FileWriter::append(string block, int size)
{
	string filePath = fileLocation + fileName;
	//outFile.open(filePath.c_str(), ios::out|ios::binary|ios::app);
	//for(int i = 0; i < size; i++){
	//		int temp = (int)block.at(i);
	//		outFile << (unsigned char)temp;
	//}
	FILE * outFile2;
	outFile2 = fopen(filePath.c_str(),"ab");
	fwrite(&block[0],sizeof(char),size,outFile2);
	fclose(outFile2);

	//outFile.close();
}

void FileWriter::close(){
	outFile.close();
}
