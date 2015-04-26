//
// main.cpp
// Task 2
//
// Created by Gregory Moon on 4/17/15.
// Copyright (c) 2015 Gregory Moon. All rights reserved.
//
#include <iostream>
#include <sstream>
#include <string>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <libgen.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <vector>


#ifdef _WIN32
using namespace std;


const char* os_pathsep = "\\";
#else
const char* os_pathsep = "/";
#endif


enum HashType{SHA1_HASH, MD5_HASH};
char *getHash(HashType hashType);
char *getFilename(char *path);
void writeOutFile(char *path, HashType hashType);
void initHexCodes();
void skipBytes(int numBytesToSkip);
void readPartitionTable();
void readPartition();
std::vector<std::string> splitString(std::string s);

//my additions
void parType(int type);
void parStart(int start);

void testPrint();
void readVBR();



std::unordered_map<std::string, const char*> hexCodes;
FILE *inFile;
int i;

//my additions
int z = 0;
int y = 0;
unsigned int par1[4];
unsigned int par2[4];


int main(int argc, char * argv[]) {
    i = 0;
    initHexCodes();
    char *filepath; //path to image file
    if(argc == 2)
	filepath = argv[1];
    else{
	//unable to open file
	printf("No filepath specified\n");
	exit(EXIT_FAILURE);
    }
    inFile = fopen(filepath, "rb");
    if(inFile != NULL){
	printf("Checksums:\n====================================\n");
	writeOutFile(filepath, MD5_HASH);
	writeOutFile(filepath, SHA1_HASH);
	readPartitionTable();
	readVBR();
    }
    else{
	printf("Unable to open file at:\n%s\n", filepath);
	exit(EXIT_FAILURE);
    }
    fclose(inFile);
    return 0;
}

void writeOutFile(char *path, HashType hashType){
    std::string escapedPath(path);

    std::string replace = "\\ ", search = " ";

    for( size_t pos = 0; ; pos += replace.length() ) 
    {
	pos = escapedPath.find( search, pos );
	if( pos == std::string::npos ) break;

	escapedPath.erase( pos, search.length() );
	escapedPath.insert( pos, replace );
    }

    char *outFilename = dirname(path);
    strcat (outFilename, os_pathsep);
    if(hashType == MD5_HASH)
	strcat(outFilename, "MD5-");
    else
	strcat(outFilename, "SHA1-");

    strcat (outFilename, getFilename(path));
    strcat (outFilename, ".txt");

    std::string cmd = "openssl ";

    if(hashType == MD5_HASH)
	cmd += "md5 ";
    else
	cmd += "sha1 ";

    cmd += escapedPath;

    FILE* pipe = popen(cmd.c_str(), "r");
    char buffer[128];
    std::string result = "";

    while(!feof(pipe)) {
	if(fgets(buffer, 128, pipe) != NULL)
	    result += buffer;
    }

    pclose(pipe);
    std::vector<std::string> tokens = splitString(result);
    std::string hash = tokens[tokens.size() - 1];

    if(hashType == MD5_HASH)
	printf("MD5: %s\n", hash.c_str());
    else
	printf("SHA1: %s\n", hash.c_str());


    FILE *outFile = fopen(outFilename, "w");

    fwrite(hash.c_str(), sizeof(char), strlen(hash.c_str()), outFile);
    fclose(outFile);
}



//==================================================================================


void readPartitionTable(){

    skipBytes(446);
    printf("====================================\n");
    for(int j = 0; j < 4; j++)
    {
	readPartition();
    }           
    printf("====================================\n");
}

//extracts each partition type in decimal
void parType(int type)
{
    par1[y] = type;
    y++; 
}

//start position of each partition (1-4)
void parStart(int start)
{ 
    par2[z] = start;
    z++;
}

//testing partition start sectors and types
void testPrint()
{

    printf("\npar1: %d\n", par2[0]);
    printf("par2: %d\n", par2[1]);
    printf("par3: %d\n", par2[2]);
    printf("par4: %d\n", par2[3]);

    printf("\npar1[type]: %d\n", par1[0]);
    printf("par2[type]: %d\n", par1[1]);
    printf("par3[type]: %d\n", par1[2]);
    printf("par4[type]: %d\n", par1[3]);
}

void readPartition(){
    skipBytes(1);
    std::stringstream stream;
    int cylinder, sector, head; 
    std::string type, size(""), start("");

    head = fgetc(inFile);
    cylinder = fgetc(inFile);
    sector = fgetc(inFile);

    stream << std::hex << fgetc(inFile);
    type = std::string(stream.str());;

    i+=4;
    skipBytes(3);

    for(int j = 0; j < 4; j++){
	std::stringstream temp;
	temp << std::hex << fgetc(inFile);
	if(temp.str().size() == 1)
		start = std::string("0") + temp.str() + start;
	else
	    start = temp.str() + start;
	i++;
    }

    for(int j = 0; j < 4; j++){
	std::stringstream temp;
	temp << std::hex << fgetc(inFile);
	if(temp.str().size() == 1)
		size = std::string("0") + temp.str() + size;
	else
	    size = temp.str() + size;
	i++;

    }

    unsigned int sizeInt, startInt, conv1;
    char *input;

    std::stringstream sizeStream, startStream, sr, sx;
    sizeStream << std::hex << size.c_str();
    sizeStream >> sizeInt;

    startStream << std::hex << start.c_str();
    startStream >> startInt;


    sx << std::hex <<type.c_str();
    sx >> conv1;

    printf("(%s) %s, %d, %d\n", type.c_str(), hexCodes[type], startInt, sizeInt);
    parType(conv1);
}


void readVBR()
{  
    std::stringstream stream;
    int start, s_clus, fat_num;
    std::string type, reserve(""), fat_size("");
    start = (par2[0] * 512) - 510; 


    skipBytes(start);
    skipBytes(13);
    s_clus = fgetc(inFile);


    i+=2;
    for(int j = 0; j < 2; j++)
    {
	stream.str("");;
	stream << std::hex << fgetc(inFile);
	std::string temp(stream.str());
	reserve = temp + reserve;
	i++;
    }

    fat_num = fgetc(inFile);
    //skipBytes(5);
    skipBytes(19);

    for(int j = 0; j < 4; j++)
    {
	stream.str("");;
	stream << std::hex << fgetc(inFile);
	std::string temp(stream.str());
	fat_size = temp + fat_size;
	i++;
    }


    unsigned int reserveInt, fatInt;
    std::stringstream ss, sr;

    ss << std::hex << reserve.c_str();
    ss >> reserveInt;

    sr << std::hex << fat_size.c_str();
    sr >> fatInt;

    printf("reserve size: %d\n", reserveInt);
    printf("cluster size: %d\n", s_clus);
    printf("# of FATs: %d\n", fat_num);
    printf("size of each FAT: %d\n", fatInt);
}

char *getFilename(char *path){
    char *nameWithExt = basename(path);
    char *retVal = strtok(nameWithExt, "\.");
    return retVal;
}

void skipBytes(int numBytesToSkip){
    const int BUFF_SIZE = 1024;
    int buff;;
    size_t bytes;
    while (numBytesToSkip > 0){
	buff = fgetc(inFile);
	numBytesToSkip--;
	i++;
    }
}

std::vector<std::string> splitString(std::string s){
    std::stringstream ss(s);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    return vstrings;
}

void initHexCodes(){
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("1"), "DOS 12-bit FAT"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("4"), "DOS 16-bit FAT for partitions smaller than 32MB"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("5"), "Extended Partition"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("6"), "DOS 16-bit FAT for partitions larger than 32MB"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("7"), "NTFS"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("8"), "AIX bootable partition"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("9"), "AIX data partition"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("b"), "DOS 32-bit FAT"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("c"), "DOS 32-bit FAT for interrupt 13 support"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("17"), "Hidden NTFS partition (XP and earlier)"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("1b"), "Hidden FAT32 partition"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("1e"), "Hidden VFAT partition"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("3c"), "Parition Magic recovery partition"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("66"), "Novell partition"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("67"), "Novell parition"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("68"), "Novell parition"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("69"), "Novell parition"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("81"), "Linux"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("82"), "Linux swap partition (can also be associated with Solaris paritions"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("83"), "Linux native file systems (Ext2, Ext3, Reiser, xiafs)"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("86"), "FAT16 volume/stripe set (Windows NT)"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("87"), "High Performance File System (HPFS) fault-tolerant mirrored partition or NTFS volume/stripe set"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("a5"), "FreeBSD and BSD/386"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("a6"), "OpenBSD"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("a9"), "NetBSD"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("c7"), "Typical of a corrupted NTFS volume/stripe set"));
    hexCodes.insert(std::make_pair<std::string, const char *>(std::string("eb"), "BeOS"));
}

