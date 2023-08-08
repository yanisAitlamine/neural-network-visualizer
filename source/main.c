#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "neuralnet.h"
#include "errors.h"
#include "in_outNN.h"

int main()
{
	size_t len=6;
	if (len >10){
		ERROR("Too many layers");
		return 1;
	}
	size_t depths[len];
	for (int i=0;i<len;i++){ 
		depths[i]=3;
		if (depths[i] >5){
			ERROR("Too deep"); 
           		 return 1;
		}
	}
	depths[0]=2;
	depths[2]=4;
	depths[3]=5;
	char* toSave="NNtest.nn";
	char* toRead="NNtest.nn";
	nNetwork NN = createNN( len, depths);
	nNetwork* nn = &NN;
	if (nn==NULL||nn->failFlag){
		ERROR("NN is NULL!\n");
	}
	fillNN(&NN);
	printNN(nn);
	if (!writeNN (toSave, nn)){ERROR("failed to write");}
	freeNN(NN);
	printf("freed NN\n");
	nNetwork* nn2 = readNN(toRead);
	if (nn2==NULL||nn2->failFlag){
		ERROR("NN 2 is NULL!\n");
	}
	printNN(nn2);
	freeNN(*nn2);
	printf( "freed NN 2\n");
	return 0;
}
