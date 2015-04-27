#include <getopt.h>
#include <iostream>
#include <stdlib.h>
#include <unordered_map>
#include <cstring>

std::unordered_map<char, int> includedOpts;
std::unordered_map<char, int> optVals;

void help(const char* message = NULL);
void initIncludedOpts(const char* validOpts);
void initOptVals();
void parseArgument(int arg, int argVal);
void pToL();
void cToL();
void lToL();
void lToP();
void cToP();
void pToP();
void lToC();
void pToC();
void cToC();

int main(int argc, char* argv[]){
	if(argc == 1)
		help();
		
    const char* validOptions = "LPCb:Bs:l:p:c:k:r:t:f:h";
    
	initOptVals();
	initIncludedOpts(validOptions);
	
	const struct option longopts[] = {
		{"logical",         no_argument, 		0, 'L'},
		{"physical",		no_argument, 		0, 'P'},
		{"cluster",         no_argument, 		0, 'C'},
		{"partition-start",	required_argument,	0, 'b'},
		{"byte-address",	no_argument,		0, 'B'},
		{"sector-size",		required_argument,	0, 's'},
		{"logical-known",	required_argument,	0, 'l'},
		{"physical-known",	required_argument,	0, 'p'},
		{"cluster-known",	required_argument,	0, 'c'},
		{"cluster-size",	required_argument,	0, 'k'},
		{"reserved",		required_argument,	0, 'r'},
		{"fat-tables",		required_argument,	0, 't'},
		{"fat-length",		required_argument,	0, 'f'},
		{"help",            no_argument,		0, 'h'},
		{0, 0, 0, 0}
	};
	
	int opt, optIdx = 0;
	
	while((opt = getopt_long(argc, argv, validOptions, longopts, &optIdx)) != -1){
		int arg;
		
		if(optarg)
			arg = atoi(optarg);
		else
			arg = -1;
					
		switch(opt){
			case 0:
				parseArgument(optIdx, arg);
				break;
			default:
				parseArgument(opt, arg);
				break;
		}
	}
    
    if(includedOpts['L']){
        if(includedOpts['c'])
            cToL();
        else if(includedOpts['p'])
            pToL();
        else
            lToL();
    }
    else if(includedOpts['P']){
        if(includedOpts['c'])
            cToP();
        else if(includedOpts['l'])
            lToP();
        else
            pToP();
    }
    else if(includedOpts['C']){
        if(includedOpts['l'])
            lToC();
        else if(includedOpts['p'])
            pToC();
        else
            cToC();
    }

	return 0;
}

void lToL(){
    int convertedVal;
    
    if(includedOpts['B'])
        convertedVal = optVals['l']*optVals['s'];
    else
        convertedVal = optVals['l'];
    
    printf("%d\n", convertedVal);

}

void cToL(){
    int convertedVal;
    
    if(includedOpts['B'])
        convertedVal = optVals['s']*((((optVals['c']-2)*optVals['k'])+optVals['r']+(optVals['t']*optVals['f']))-optVals['b']);
    else
        convertedVal = ((((optVals['c']-2)*optVals['k'])+optVals['r']+(optVals['t']*optVals['f']))-optVals['b']);
    
    printf("%d\n", convertedVal);

}

void pToL(){
    int convertedVal;
    
    if(includedOpts['B'])
        convertedVal = (optVals['p'] - optVals['b']) * optVals['s'];
    else
        convertedVal = optVals['p'] - optVals['b'];
    
    printf("%d\n", convertedVal);
}

void lToP(){
    int convertedVal;
    
    if(includedOpts['B'])
        convertedVal = (optVals['l']+optVals['b'])*optVals['s'];
    else
        convertedVal = optVals['l']+optVals['b'];
    
    printf("%d\n", convertedVal);
}

void pToP(){
    int convertedVal;
    
    if(includedOpts['B'])
        convertedVal = optVals['p']*optVals['s'];
    else
        convertedVal = optVals['p'];
    
    printf("%d\n", convertedVal);
}

void cToP(){
    int convertedVal;
    
    if(includedOpts['B'])
        convertedVal = optVals['s']*((((optVals['c']-2)*optVals['k'])+optVals['r']+(optVals['t']*optVals['f']))+optVals['b']);
    else
        convertedVal = ((((optVals['c']-2)*optVals['k'])+optVals['r']+(optVals['t']*optVals['f']))+optVals['b']);
    
    printf("%d\n", convertedVal);

}

void cToC(){
    int convertedVal;
    
    if(includedOpts['B'])
        convertedVal = optVals['c']*optVals['s'];
    else
        convertedVal = optVals['c'];
    
    printf("%d\n", convertedVal);
}

void lToC(){
    int convertedVal;
    if(includedOpts['B'])
        convertedVal = (((optVals['l']-(optVals['r']+optVals['t']*optVals['f']))/optVals['k'])+2)*optVals['s'];
    else
        convertedVal = (((optVals['l']-(optVals['r']+optVals['t']*optVals['f']))/optVals['k'])+2);
    
    printf("%d\n", convertedVal);

}

void pToC(){
    int convFactor, convertedVal;
    
    if(includedOpts['B']){
        convFactor = (optVals['p']-optVals['b']);
        convertedVal = (((convFactor-(optVals['r']+optVals['t']*optVals['f']))/optVals['k'])+2)*optVals['s'];
    }
    else{
        convFactor = (optVals['p']-optVals['b']);
        convertedVal = (((convFactor-(optVals['r']+optVals['t']*optVals['f']))/optVals['k'])+2);
    }
    
    printf("%d\n", convertedVal);

}

void parseArgument(int arg, int argVal){
	if(includedOpts[arg])
		help();
	else
		includedOpts[arg] = true;
		
	switch(arg){
		case 'L':
			if(includedOpts['C'] || includedOpts['P'] || argVal != -1)
				help();
			break;
		case 'P':
			if(includedOpts['L'] || includedOpts['C'] ||  argVal != -1)
				help();
			break;
		case 'C':
			if(includedOpts['L'] || includedOpts['P'] ||  argVal != -1)
				help();
			break;
		case 'b':
		case 's':
			optVals[arg] = argVal;
			break;
		case 'l':
			if(argVal < 0 || includedOpts['c'] || includedOpts['k'] || includedOpts['r'] || includedOpts['t'] || includedOpts['p'])
				if(argVal < 0)
					help("Invalid argument for -l flag");
				else
					help();
			else
				optVals[arg] = argVal;
			break;
		case 'p':
			if(argVal < 0 || includedOpts['c'] || includedOpts['k'] || includedOpts['r'] || includedOpts['t'] || includedOpts['l'])
				if(argVal < 0)
					help("Invalid argument for -p flag");
				else
					help();
			else
				optVals[arg] = argVal;
			break;
		case 'c':
		case 'k':
		case 'r':
		case 't':	
		case 'f':
			if(argVal < 0 || includedOpts['p'] || includedOpts['l'])
				if(argVal < 0)
					help("Invalid argument(s) for -c, -k, -r, -t,  or -f flag(s)");
				else
					help();

			else
				optVals[arg] = argVal;
			break;
		case 'h':
			help();
			break;
	}
}

void initOptVals(){
	optVals['b'] = 0;
	optVals['s'] = 512;
	optVals['l'] = -1;
	optVals['p'] = -1;
	optVals['c'] = -1;
	optVals['k'] = -1;
	optVals['r'] = -1;
	optVals['t'] = -1;
	optVals['f'] = -1;
	optVals['h'] = -1;
}

void initIncludedOpts(const char* validOpts){
    for (int i = 0; i < strlen(validOpts); i++) {
        if(validOpts[i] != ':')
            includedOpts.insert(std::make_pair(validOpts[i], false));
    }
}

void help(const char* message){
    const char* helpMessage = "address4forensics -L|-P|-C [–b offset] [-B [-s bytes]] [-l address] [- p address] [-c address -k sectors -r sectors -t tables -f sectors]\n-L, --logical\n\tCalculate the logical address from either the cluster address or the physical address. Either –c or –p must be given.\n-P, --physical\n\tCalculate the physical address from either the cluster address or the logical address. Either –c or –l must be given.\n-C, --cluster\n\tCalculate the cluster address from either the logical address or the physical address. Either –l or –p must be given.\n-b offset, --partition-start=offset\n\tThis specifies the physical address (sector number) of the start of the partition, and defaults to 0 for ease in working with images of a single partition. The offset value will always translate into logical address 0.\n-B, --byte-address\n\tInstead of returning sector values for the conversion, this returns the byte address of the calculated value, which is the number of sectors multiplied by the number of bytes per sector.\n-s bytes, --sector-size=bytes\n\tWhen the –B option is used, this allows for a specification of bytes per sector other than the default 512. Has no affect on output without –B.\n-l address, --logical-known=address\n\tThis specifies the known logical address for calculating either a cluster address or a physical address. When used with the –L option, this simply returns the value given for address.\n-p address, --physical-known=address\n\tThis specifies the known physical address for calculating either a cluster address or a logical address. When used with the –P option, this simply returns the value given for address.\n-c address, --cluster-known=address\n\tThis specifies the known cluster address for calculating either a logical address or a physical address. When used with the –C option, this simply returns the value given for address. Note that options –k, -r, -t, and –f must be provided with this option.\n-k sectors, --cluster-size=sectors\n\tThis specifies the number of sectors per cluster.\n-r sectors, --reserved=sectors\n\tThis specifies the number of reserved sectors in the partition.\n-t tables, --fat-tables=tables\n\tThis specifies the number of FAT tables, which is usually 2.\n-f sectors, --fat-length=sectors\n\tThis specifies the length of each FAT table in sectors.";
	if(message == NULL)
		printf("%s\n", helpMessage);
	else
		printf("%s\n", message);
	exit(1);
}
