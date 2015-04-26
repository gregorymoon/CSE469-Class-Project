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

//my additions
void parType(char type);
void parStart(int start);
std::vector<const char *> types;

int parStart();
void testPrint();
void readVBR();



std::unordered_map<std::string, const char*> hexCodes;
FILE *inFile;
int i;

//my additions
int z = 0;
int y = 0;
char *par1[4];

unsigned int par2[4];

//unsigned int conv[4];
//unsigned char *par1[4] = {""};

//par1 = std::string(stream.str());;







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
	// writeOutFile(filepath, MD5_HASH);
	// writeOutFile(filepath, SHA1_HASH);
        //readSize();
        printf("----------------------MBR--------------------\n");
	readPartitionTable();
        printf("----------------------TEST--------------------\n");
        testPrint();
        //parType();
        printf("\n----------------------VBR--------------------\n");
	readVBR();
    }
    else{
	printf("Unable to open file at:\n%s\n", filepath);
	exit(EXIT_FAILURE);
    }
    fclose(inFile);
    return 0;
}








char *getHash(HashType hashType){
    const int BUFF_SIZE = 1024;
    int digestLength;
    char *retVal;
    unsigned char *digest, buff[BUFF_SIZE];
    size_t bytes;
    if(hashType == MD5_HASH){
	digestLength = MD5_DIGEST_LENGTH;
	retVal = (char *)malloc(sizeof(char) * MD5_DIGEST_LENGTH);
	digest = (unsigned char *)malloc(sizeof(unsigned char) * MD5_DIGEST_LENGTH);
	MD5_CTX context;
	MD5_Init(&context);
	while ((bytes = fread (buff, 1, BUFF_SIZE, inFile)) != 0)
	    MD5_Update (&context, buff, bytes);
	MD5_Final (digest,&context);
    }
    else{
	digestLength = SHA_DIGEST_LENGTH;
	retVal = (char *)malloc(sizeof(char) * SHA_DIGEST_LENGTH);
	digest = (unsigned char *)malloc(sizeof(unsigned char) * MD5_DIGEST_LENGTH);
	SHA_CTX context;
	SHA1_Init (&context);
	while ((bytes = fread (buff, 1, BUFF_SIZE, inFile)) != 0)
	    SHA1_Update (&context, buff, bytes);
	SHA1_Final (digest,&context);
    }
    for(int j = 0; j < digestLength; j++)
	sprintf(retVal, "%s%02x", retVal, digest[i]);
    rewind(inFile);
    return retVal;
}




void writeOutFile(char *path, HashType hashType){
    char *outFilename = dirname(path);
    strcat (outFilename, os_pathsep);
    if(hashType == MD5_HASH)
	strcat(outFilename, "MD5-");
    else
	strcat(outFilename, "SHA1-");
    strcat (outFilename, getFilename(path));
    strcat (outFilename, ".txt");
    FILE *outFile = fopen(outFilename, "w");
    char *hash = getHash(hashType);
    printf("%s\n", hash);
    fwrite(hash, sizeof(char), strlen(hash), outFile);
    fclose(outFile);
}



//==================================================================================


void readPartitionTable(){
    types.clear();
    skipBytes(446);
    for(int j = 0; j < 4; j++)
	readPartition();
          //printf("testing: %d", readPartition());
          //parStart(); 
          //exit(0);
}







//start position of each partition (1-4)
void readpar1(char* type)
{

 //type = type.c_str();
 //printf("type: %s\n", type);
 //par1[y] = (unsigned char*)type;
 par1[y] = type;
 y++;
}

void parStart(int start)
{ 
  par2[z] = start;
  z++;
}






//testing partition start sectors
void testPrint()
{
 printf("\npar1: %d\n", par2[0]);
 printf("par2: %d\n", par2[1]);
 printf("par3: %d\n", par2[2]);
 printf("par4: %d\n", par2[3]);

 //cout << "\npar1: \n" << par1[0] << endl;
 printf("\npar1: %s\n",par1[0]);
 /*printf("\npar1: %s\n",par1[0]);
 printf("\npar1: %s\n",par1[0]);*/
  
}



//char readpar1(){
//skipBytes(3);
//std::string type;
//std::stringstream stream;
//stream << std::hex << fgetc(inFile);
//type = std::string(stream.str());;

//return type;
//}





//int parStart()
//{
 // skipBytes(7);
 // std::stringstream stream;
 // std::string type, start("");
 // unsigned int startInt;
 // std::stringstream sr;

  //for(int j = 0; j < 4; j++)
   // {
     //  stream.str("");;
     //  stream << std::hex << fgetc(inFile);
     // std::string temp(stream.str());
     // start = temp + start;
    //} 
  //sr << std::hex << start.c_str();
  //sr >> startInt; 
  
  //printf("testing: %d", startInt);
  //return startInt; 
//}




void readPartition(){
    printf("-------------------------\n");
    skipBytes(1);
    std::stringstream stream;
    int cylinder, sector, head; // start;
    std::string type, size(""), start("");
    //std::string type, start("");
    head = fgetc(inFile);
    cylinder = fgetc(inFile);
    //skipBytes(1);
    sector = fgetc(inFile);
   
    stream << std::hex << fgetc(inFile);
    type = std::string(stream.str());;
 
    
    skipBytes(3);
   i+=4;
    for(int j = 0; j < 4; j++)
    {
       stream.str("");;
       stream << std::hex << fgetc(inFile);
       std::string temp(stream.str());
       start = temp + start;
       i++;
    }       

    //skipBytes(-5);
    //start = fgetc(inFile);
    
    
    //i+=4;
    //skipBytes(7);
    for(int j = 0; j < 4; j++){
	stream.str("");;
	stream << std::hex << fgetc(inFile);
	std::string temp(stream.str());
	size = temp + size;
        //printf("i size = %d\n", i);
	i++;
	
    }
    unsigned int sizeInt, startInt;
    std::stringstream ss, sr;
    ss << std::hex << size.c_str();
    ss >> sizeInt;
    
    sr << std::hex << start.c_str();
    sr >> startInt; 

    printf("(%s) %s,\n", type.c_str(), hexCodes[type]);
    types.push_back(type.c_str());
    printf("head: %d\n", head);
    printf("cylinder: %d\n", cylinder);
    //skipBytes(5);
    printf("sector: %d\n", sector);
    printf("size: %d\n", sizeInt);
    printf("start: %d\n", startInt);
    parStart(startInt);
    readpar1((char*)type.c_str());
    
    printf("i count - %d\n", i);
    
        
}

//================================**************
//================================**************
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
   printf("i count - %d\n", i);
}


//================================**************
//================================**************



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










//==================================================================================








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

