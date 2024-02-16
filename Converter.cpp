#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "lz4.h"
#define MAX_RAMSIZE 0x20000
using namespace std;
int Error(string message){
	cout << "[Error] " + message << endl;
	return 0;
}
int main(int argc,char** argv){
	static char outputBuffer[MAX_RAMSIZE] = {0};
	int (*LZ4)(const char*,char*,int,int) = NULL;
	if(argc < 2){
		return Error("[Error] No parameter supplied.");
	}
	auto path = (string)argv[1];
	auto extension = path.substr(path.find_last_of('.'));
	if(extension.compare(".gbs") == 0){LZ4 = LZ4_decompress_safe;}
	if(extension.compare(".sav") == 0){LZ4 = LZ4_compress_default;}
	if(!LZ4){
		return Error("[Error] Couldn't identify input file format. Expected '.gbs' or '.sav'.");
	}
	auto inputFile = fstream(path,ios::in|ios::binary);
	if(!inputFile){
		return Error("[Error] Couldn't find file '"+path+"'.");
	}
	auto length = (int)filesystem::file_size(path);
	cout << length << endl;
	if(length < 1){
		return Error("[Error] Input file is empty.");
	}
	auto compressing = LZ4 == LZ4_compress_default;
	path = path.replace(path.find(extension),extension.length(),compressing ? ".gbs" : ".sav");
	auto outputFile = fstream(path,ios::out|ios::binary);
	auto inputStream = ostringstream() << inputFile.rdbuf();
	auto inputBuffer = inputStream.str();
	auto outputSize = LZ4(inputBuffer.c_str(),outputBuffer,length,MAX_RAMSIZE);
	outputFile.write(outputBuffer,outputSize);
	return 0;
}
