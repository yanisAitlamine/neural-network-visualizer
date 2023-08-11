#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "errors.h"
#include "neuralnet.h"


double rand_double(){
    return (double)rand()/(double)RAND_MAX;
}

// Create an object neural network of a given length with given layer lenghts
nNetwork* createNN(size_t len, size_t* depths){
	printf("Creating Network of size %ld!\n",len);
	nNetwork* NN=(nNetwork*)malloc(sizeof(nNetwork));
	NN->failFlag=false;
	NN->len=len;
	NN->depths=(size_t*)malloc(len*sizeof(size_t));
	if (check_malloc(NN->depths,"Depths init failed!\n")){
	    NN->failFlag=true;
	    return NN;
	}
	NN->weights=(double***)malloc((len-1)*sizeof(double**));
	if (check_malloc(NN->weights,"Weights init failed!\n")){
	    NN->failFlag=true;
	    return NN;
	}
	NN->weightsGrd=(double***)malloc((len-1)*sizeof(double**));
	if (check_malloc(NN->weightsGrd,"WeightsGrd init failed!\n")){
	    NN->failFlag=true;
	    return NN;
	}
	NN->bias=(double**)malloc((len-1)*sizeof(double*));
	if (check_malloc(NN->bias,"Bias init failed!\n")){
	    NN->failFlag=true;
	    return NN;
	}
	NN->biasGrd=(double**)malloc((len-1)*sizeof(double*));
	if (check_malloc(NN->biasGrd,"biasGrd init failed!\n")){
	    NN->failFlag=true;
	    return NN;
	}
	NN->activations=(double***)malloc((len)*sizeof(double**));
	if (check_malloc(NN->activations,"activations init failed!\n")){
	    NN->failFlag=true;
	    return NN;
	}
	printf("depths\t ");
	NN->depths[0]=depths[0];
	printf ("layer %d:%ld\t",1,depths[0]);
	for (int i=0;i<len-1;i++){
	    printf ("layer %d:%ld\t",i+2,depths[i+1]);
	    NN->depths[i+1]=depths[i+1];
 	    if(alloc_mtrx(&(NN->weightsGrd[i]), NN->depths[i],NN->depths[i+1])) return NN;
	    if (alloc_table(&(NN->biasGrd[i]), NN->depths[i+1])) return NN;
	    if(alloc_mtrx(&(NN->weights[i]), NN->depths[i],NN->depths[i+1])) return NN;
	    if (alloc_table(&(NN->bias[i]), NN->depths[i+1])) return NN;
//the activation table store the activation, the activation without the smoothing function, the derivative of C with regard to that node activation
	    if (alloc_mtrx(&(NN->activations[i]), NN->depths[i],4)) return NN;
	}
	if (alloc_mtrx(&(NN->activations[len-1]), NN->depths[len-1],2)) return NN;
	printf("\nSuccesfully created!\n");
	return NN;
}

//Allocate space for a matrix
bool alloc_mtrx(double*** mtrx, size_t len, size_t depth){
    *mtrx=(double**)malloc(len*sizeof(double*));
    if (check_malloc(*mtrx,"Mtrx init failed!\n")){
	return true;
    }
    for (int y=0;y<len;y++){
	(*mtrx)[y]=(double*)malloc(depth*sizeof(double));
	if (check_malloc((*mtrx)[y],"Init failed at row: ")){
	    printf ("%ld,%d!\n",len,y);
	    return true;
	}
    }
    return false;
}

//Allocate space for a table
bool alloc_table(double** mtrx, size_t len){
    *mtrx=(double*)malloc(len*sizeof(double));
    if (check_malloc(*mtrx,"Mtrx init failed!\n")){
	return true;
    }
    return false;
}

//Initialize weights and bias with random numbers
void fillNN(nNetwork* NN){
    printf ("Filling neural net of size %ld!\n",NN->len);
    for (int i=0;i<NN->len-1;i++){
        for (int x=0;x<NN->depths[i];x++){
	    for (int y=0; y<NN->depths[i+1];y++){	
		NN->weights[i][x][y]=rand_double();
	    }
	}
	for (int y=0;y<NN->depths[i+1];y++){
	    NN->bias[i][y]=rand_double();
	}
    }
    printf ("Filling done!\n");
}

//Print weights and bias
void printNN(nNetwork* NN){
    printf ("Printing neural net of size %ld!\n",NN->len);
    for (int i=0;i<NN->len-1;i++){
	printf ("===================================================================\n");
	printf("Layer %d->%d\tlen: %ld\tdepth: %ld\n",i+1,i+2,NN->depths[i],NN->depths[i+1]);
	printf ("===================================================================\n");
    	for (int x=0;x<NN->depths[i];x++){
	    printf("[");
	    for (int y=0; y<NN->depths[i+1];y++){
		printf("%.1f",NN->weights[i][x][y]);
		if (y<NN->depths[i+1]-1) printf(", ");
	    }
	    printf("]");
	}    	
	printf("\n[");
    	for (int x=0;x<NN->depths[i+1];x++){
	    printf("%.1f",NN->bias[i][x]);
	    if (x<NN->depths[i+1]-1) printf(", ");
	}
	printf("]\n");
    } 
}


// Free a neural network object
void freeNN(nNetwork* NN){
    if (NN!=NULL){
	printf ("Free network of size %ld!\n",NN->len);
	if (NN->depths!=NULL){
	    if (NN->weights!=NULL){	
		for (int i=0;i<NN->len-1;i++){
		    free_mtrx(&(NN->weights[i]), NN->depths[i]);
		    free(NN->bias[i]);
		    free_mtrx(&(NN->weightsGrd[i]), NN->depths[i]);
		    free(NN->biasGrd[i]);
		    free_mtrx(&(NN->activations[i]),NN->depths[i]);
		}
		free_mtrx(&(NN->activations[NN->len-1]),NN->depths[NN->len-1]);
		free(NN->weights);
		free(NN->activations);
		free(NN->bias);
	    }
	    free(NN->depths);
	}
    free(NN);
    printf("Freed!\n");
    }
}

void free_mtrx(double ***data, size_t depth){
    if (*data!=NULL){
	printf ("Free matrix of size %ld!\n",depth);
	for (int i=0;i<depth;i++){
	    free((*data)[i]);
	} 
	free(*data);
    }
}
