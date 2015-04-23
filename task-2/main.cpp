//
//  main.cpp
//  Task 2
//
//  Created by Gregory Moon on 4/17/15.
//  Copyright (c) 2015 Gregory Moon. All rights reserved.
//

#include <iostream>
#include <string>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <libgen.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
    const char* os_pathsep = "\\";
#else
    const char* os_pathsep = "/";
#endif

enum HashType{SHA1_HASH, MD5_HASH};
char *getHash(FILE *inFile, HashType);
char *getFilename(char *path);
void writeOutFile(char *path, HashType hashType, FILE *inFile);

int main(int argc, char * argv[]) {
    FILE *inFile;
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
        writeOutFile(filepath, MD5_HASH, inFile);
        writeOutFile(filepath, SHA1_HASH, inFile);
    }
    else{
	printf("Unable to open file at:\n%s\n", filepath);
        exit(EXIT_FAILURE);
    }
    
    
    fclose(inFile);

    return 0;
}

char *getHash(FILE *inFile, HashType hashType){
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
    
    
    for(int i = 0; i < digestLength; i++)
        sprintf(retVal, "%s%02x", retVal, digest[i]);
    
    rewind(inFile);
    
    return retVal;
}
              
void writeOutFile(char *path, HashType hashType, FILE *inFile){
    char *outFilename = dirname(path);
    strcat (outFilename, os_pathsep);

    if(hashType == MD5_HASH)
        strcat(outFilename, "MD5-");
    else
        strcat(outFilename, "SHA1-");

    strcat (outFilename, getFilename(path));
    strcat (outFilename, ".txt");
    
    FILE *outFile = fopen(outFilename, "w");
    char *hash = getHash(inFile, hashType);
    printf("%s\n", hash);
    fwrite(hash, sizeof(char), strlen(hash), outFile);
    fclose(outFile);
}

char *getFilename(char *path){
    char *nameWithExt = basename(path);
    char *retVal = strtok(nameWithExt, "\.");
    
    return retVal;
}

