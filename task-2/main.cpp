//
//  main.cpp
//  Task 2
//
//  Created by Gregory Moon on 4/17/15.
//  Copyright (c) 2015 Gregory Moon. All rights reserved.
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

std::unordered_map<std::string, const char*> hexCodes;
FILE *inFile;
int i;


int main(int argc, char * argv[]) {
	i = 0;
	initHexCodes();
	char *filepath;  //path to image file

	if(argc == 2)
		filepath = argv[1];
	else{
		//unable to open file
		printf("No filepath specified\n");
		exit(EXIT_FAILURE);
	}

	inFile = fopen(filepath, "rb");

	if(inFile != NULL){
		//	writeOutFile(filepath, MD5_HASH);
		//	writeOutFile(filepath, SHA1_HASH);

		readPartitionTable();
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
		readPartition();
	exit(0);
}

void readPartition(){
	printf("-------------------------\n");
	skipBytes(1);

	std::stringstream stream;

	int buff, cylinder, sector, head;
	std::string type, size("");

	head = fgetc(inFile);
	cylinder = fgetc(inFile);
	sector = fgetc(inFile);

	stream << std::hex << fgetc(inFile);
	type = std::string(stream.str());;
	i+=4;

	skipBytes(7);

	for(int j = 0; j < 4; j++){
		stream.str("");;
		stream << std::hex << fgetc(inFile);
		std::string temp(stream.str());

		size = temp + size;
		i++;
	}

	unsigned int sizeInt;   
	std::stringstream ss;
	ss << std::hex << size.c_str();
	ss >> sizeInt;
	printf("(%s) %s,\n", type.c_str(), hexCodes[type]);
	printf("head: %d\n", head);
	printf("cylinder: %d\n", cylinder);
	printf("sector: %d\n", sector);
	printf("size: %d\n", sizeInt);
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
	hexCodes.insert(std::make_pair<std::string, const char *>(std::string("a5"),  "FreeBSD and BSD/386"));
	hexCodes.insert(std::make_pair<std::string, const char *>(std::string("a6"), "OpenBSD"));
	hexCodes.insert(std::make_pair<std::string, const char *>(std::string("a9"), "NetBSD"));
	hexCodes.insert(std::make_pair<std::string, const char *>(std::string("c7"), "Typical of a corrupted NTFS volume/stripe set"));
	hexCodes.insert(std::make_pair<std::string, const char *>(std::string("eb"), "BeOS"));
}
