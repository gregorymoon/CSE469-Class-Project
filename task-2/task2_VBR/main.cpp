//
// main.cpp
// Task 2
//
// Created by Gregory Moon on 4/17/15.
// additional work by Vito Palumbo II - VBR
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
void parType(int type);
void parStart(int start);
void parSize(int size);

void readVBR();
void readVBR_table();



std::unordered_map<std::string, const char*> hexCodes;
FILE *inFile;
int i;

//my additions
int x = 0;
int z = 0;
int y = 0;
int par_count = 0;
unsigned int par1[4];//type in decimal
unsigned int par2[4];//start sector per partition
unsigned int par3[4];//size of each partition


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
        
        printf("----------------------MBR--------------------\n");
	readPartitionTable();
        printf("----------------------VBR--------------------\n");
	readVBR_table();
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

void readPartitionTable(){
    
    skipBytes(446);
    for(int j = 0; j < 4; j++)
	{
         readPartition();
        }           
}

//runs through each partition to create VBR
void readVBR_table(){
skipBytes((par2[0] * 512) - i);
for(int j = 0; j < 4; j++)
	{       
           if(par1[par_count] == 4 || par1[par_count] == 6 || par1[par_count] == 11 
           || par1[par_count] == 12 || par1[par_count] == 27 || par1[par_count] == 134)
           {
             readVBR();   
           }
            par_count = j + 1;
        } 
}

//extracts each partition type in decimal
void parType(int type)
{
 par1[x] = type;
 x++; 
}

//start position of each partition (1-4)
void parStart(int start)
{ 
  par2[y] = start;
  y++;
}

//size of each partition
void parSize(int size)
{ 
  par3[z] = size;
  z++;
}

void readPartition(){
    printf("-------------------------\n");
    printf("first i count: %d\n", i);
    skipBytes(1);
    std::stringstream stream;
    int cylinder, sector, head; 
    std::string type, size(""), start("");
        
    head = fgetc(inFile);
    cylinder = fgetc(inFile);
    sector = fgetc(inFile);
   
    stream << std::hex << fgetc(inFile);
    type = std::string(stream.str());;
    skipBytes(3);

    for(int l = 0; l < 4; l++)
    {
       std::stringstream temp;
       temp << std::hex << fgetc(inFile);
       if(temp.str().size() == 1)
            start = std::string("0") + temp.str() + start;
       else
            start = temp.str() + start;
       i++;
    }
    
    for(int j = 0; j < 4; j++)
    {

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
    std::stringstream ss, sr, sx;
    ss << std::hex << size.c_str();
    ss >> sizeInt;
    
    sr << std::hex << start.c_str();
    sr >> startInt;     

    printf("(%s) %s,\n", type.c_str(), hexCodes[type]);

    sx << std::hex <<type.c_str();
    sx >> conv1;
    
    printf("head: %d\n", head);
    printf("cylinder: %d\n", cylinder);
    printf("sector: %d\n", sector);
    printf("size: %d\n", sizeInt);
    printf("start: %d\n", startInt);
    parStart(startInt);
    parType(conv1);
    parSize(sizeInt);
    
    i+=4;
}

void readVBR()
{  
  printf("==================================================\n");
      
   std::stringstream stream;
   int start, s_clus, fat_num, w_size, r_dir;
   std::string type, reserve(""), fat_size(""), r_dir_size("");
     
     if(par_count != 0)
       {
         start = (par2[par_count] * 512) - i; 
         skipBytes(start);
       } 
  
   skipBytes(13);
   s_clus = fgetc(inFile);   

   i+=2;
   for(int j = 0; j < 2; j++)
    {
      std::stringstream temp;
       temp << std::hex << fgetc(inFile);
       if(temp.str().size() == 1)
            reserve = std::string("0") + temp.str() + reserve;
       else
            reserve = temp.str() + reserve;
       i++;
    }
   
   fat_num = fgetc(inFile);
   
   for(int j = 0; j < 2; j++)
    {
      std::stringstream temp;
       temp << std::hex << fgetc(inFile);
       if(temp.str().size() == 1)
            r_dir_size = std::string("0") + temp.str() + r_dir_size;
       else
            r_dir_size = temp.str() + r_dir_size;
        
       i++;  
   }
   
   if (par1[par_count] == 11 || par1[par_count] == 12 || par1[par_count] == 27)
       {       
        skipBytes(17);
        w_size = 4;
        r_dir = 0;
       }
   else
       {
        skipBytes(3);
        w_size = 2; 
        r_dir = 1;
       }

   for(int j = 0; j < w_size; j++)
    {
      std::stringstream temp;
       temp << std::hex << fgetc(inFile);
       if(temp.str().size() == 1)
            fat_size = std::string("0") + temp.str() + fat_size;
       else
            fat_size = temp.str() + fat_size;
   
       i++;
    }
 

   unsigned int reserveInt, fatInt, rdInt;
   std::string par_type;
   std::stringstream sr, ss, st, su;
   
   sr << std::hex << fat_size.c_str();
   sr >> fatInt;
   
   ss << std::hex << reserve.c_str();
   ss >> reserveInt;
      
   st << std::hex << r_dir_size.c_str();
   st >> rdInt;
   
   su << std::hex << par1[par_count];
   par_type = std::string(su.str());; 


   printf("Partition %d(%s):\n",par_count, hexCodes[par_type]);
   printf("Reserved area: Start sector: 0 Ending sector: %d Size: %d sectors\n",reserveInt -1, reserveInt);
   printf("Sectors per cluster: %d sectors\n", s_clus);
   printf("FAT area: Start sector: %d Ending sector: %d\n", reserveInt, (fat_num * fatInt) + reserveInt -1);   
   printf("# of FATs: %d\n", fat_num);
   printf("The size of each FAT: %d sectors\n", fatInt);
   printf("The first sector of cluster 2: %d sectors\n", par2[par_count] + ((fat_num * fatInt) + reserveInt) + (rdInt * r_dir/16));
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

