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
	FF(NN)=false;
	LEN(NN)=len;
	DPTH(NN)=(size_t*)malloc(len*sizeof(size_t));
	if (check_malloc(DPTH(NN),"Depths init failed!\n")){
	    FF(NN)=true;
	    return NN;
	}
	W(NN)=(double***)malloc((len-1)*sizeof(double**));
	if (check_malloc(W(NN),"Weights init failed!\n")){
	    FF(NN)=true;
	    return NN;
	}
	WGRD(NN)=(double***)malloc((len-1)*sizeof(double**));
	if (check_malloc(WGRD(NN),"WeightsGrd init failed!\n")){
	    FF(NN)=true;
	    return NN;
	}
	B(NN)=(double**)malloc((len-1)*sizeof(double*));
	if (check_malloc(B(NN),"Bias init failed!\n")){
	    FF(NN)=true;
	    return NN;
	}
	BGRD(NN)=(double**)malloc((len-1)*sizeof(double*));
	if (check_malloc(BGRD(NN),"biasGrd init failed!\n")){
	    FF(NN)=true;
	    return NN;
	}
	ACT(NN)=(double***)malloc((len)*sizeof(double**));
	if (check_malloc(ACT(NN),"activations init failed!\n")){
	    FF(NN)=true;
	    return NN;
	}
	printf("depths\t ");
	DPTH(NN)[0]=depths[0];
	printf ("layer %d:%ld\t",1,depths[0]);
	for (int i=0;i<len-1;i++){
	    printf ("layer %d:%ld\t",i+2,depths[i+1]);
	    DPTH(NN)[i+1]=depths[i+1];
 	    if(alloc_mtrx(&(WGRD(NN)[i]), DPTH(NN)[i],NN->depths[i+1])) return NN;
	    if (alloc_table(&(BGRD(NN)[i]), DPTH(NN)[i+1])) return NN;
	    if(alloc_mtrx(&(W(NN)[i]), DPTH(NN)[i],NN->depths[i+1])) return NN;
	    if (alloc_table(&(B(NN)[i]), DPTH(NN)[i+1])) return NN;
//the activation table store the activation, the activation without the smoothing function, the derivative of C with regard to that node activation
	    if (alloc_mtrx(&(ACT(NN)[i]), DPTH(NN)[i],4)) return NN;
	}
	if (alloc_mtrx(&(ACT(NN)[len-1]), DPTH(NN)[len-1],4)) return NN;
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
	if (alloc_table(&((*mtrx)[y]),depth)){
	    printf ("Failed init at row %d!\n",y);
	    return true;
	}
    }
    return false;
}

//Allocate space for a table
bool alloc_table(double** mtrx, size_t len){
    *mtrx=(double*)malloc(len*sizeof(double));
    if (check_malloc(*mtrx,"Table init failed!\n")){
	return true;
    }
    return false;
}

//Initialize weights and bias with random numbers
void fillNN(nNetwork* NN){
    printf ("Filling neural net of size %ld!\n",LEN(NN));
    for (int i=0;i<LEN(NN)-1;i++){
        for (int x=0;x<DPTH(NN)[i];x++){
	    for (int y=0; y<DPTH(NN)[i+1];y++){	
		W(NN)[i][x][y]=rand_double();
	    }
	}
	for (int y=0;y<DPTH(NN)[i+1];y++){
	    B(NN)[i][y]=rand_double();
	}
    }
    printf ("Filling done!\n");
}

//Update weights and bias with Grd and learing rate
void updateNN(nNetwork* NN, double learning_rate, bool debug){
    if (debug)printf ("updating neural net of size %ld with LR %f!\n",LEN(NN),learning_rate);
    for (int i=0;i<LEN(NN)-1;i++){
        for (int x=0;x<DPTH(NN)[i];x++){
	    for (int y=0; y<DPTH(NN)[i+1];y++){	
		W(NN)[i][x][y]+=WGRD(NN)[i][x][y]*learning_rate;
	    }
	}
	for (int y=0;y<DPTH(NN)[i+1];y++){
	    B(NN)[i][y]+=BGRD(NN)[i][y]*learning_rate;
	}
    }
}

//Print weights and bias
void printNN(nNetwork* NN){
    printf ("Printing neural net of size %ld!\n",LEN(NN));
    for (int i=0;i<LEN(NN)-1;i++){
	printf ("===================================================================\n");
	printf("Layer %d->%d\tlen: %ld\tdepth: %ld\n",i+1,i+2,DPTH(NN)[i],NN->depths[i+1]);
	printf ("===================================================================\n");
    	for (int x=0;x<DPTH(NN)[i];x++){
	    printf("[");
	    for (int y=0; y<DPTH(NN)[i+1];y++){
		printf("%.1f",W(NN)[i][x][y]);
		if (y<DPTH(NN)[i+1]-1) printf(", ");
	    }
	    printf("]");
	}    	
	printf("\n[");
    	for (int x=0;x<DPTH(NN)[i+1];x++){
	    printf("%.1f",B(NN)[i][x]);
	    if (x<DPTH(NN)[i+1]-1) printf(", ");
	}
	printf("]\n");
    } 
}

//Print weights and bias Grd
void printNNGrd(nNetwork* NN){
    printf ("Printing neural net Grd of size %ld!\n",LEN(NN));
    for (int i=0;i<LEN(NN)-1;i++){
	printf ("===================================================================\n");
	printf("Layer %d->%d\tlen: %ld\tdepth: %ld\n",i+1,i+2,DPTH(NN)[i],NN->depths[i+1]);
	printf ("===================================================================\n");
    	for (int x=0;x<DPTH(NN)[i];x++){
	    printf("[");
	    for (int y=0; y<DPTH(NN)[i+1];y++){
		printf("%f",WGRD(NN)[i][x][y]);
		if (y<DPTH(NN)[i+1]-1) printf(", ");
	    }
	    printf("]");
	}    	
	printf("\n[");
    	for (int x=0;x<DPTH(NN)[i+1];x++){
	    printf("%.1f",BGRD(NN)[i][x]);
	    if (x<DPTH(NN)[i+1]-1) printf(", ");
	}
	printf("]\n");
    } 
}

// Free a neural network object
void freeNN(nNetwork* NN){
    if (NN==NULL){return;}
    printf ("Free network of size %ld!\n",LEN(NN));
    free3D_mtrx(W(NN),LEN(NN)-1,DPTH(NN));
    free3D_mtrx(WGRD(NN),LEN(NN)-1,DPTH(NN));
    free_mtrx(B(NN),LEN(NN)-1);
    free_mtrx(BGRD(NN),LEN(NN)-1);
    free3D_mtrx(ACT(NN),LEN(NN),DPTH(NN));
    free(DPTH(NN));
    free(NN);
}

void free3D_mtrx(double ***data, size_t len, size_t* depths){
    if (data==NULL) return;
    for (int i=0;i<len;i++){
	for (int y=0;y<depths[i];y++){
	    if (data[i][y]!=NULL) free(data[i][y]);
	}
	if (data[i]!=NULL)free(data[i]);
    }
    free (data);
}

void free_mtrx(double **data, size_t depth){
    if (data!=NULL){
	for (int i=0;i<depth;i++){
	    if (data[i]!=NULL)free(data[i]);
	}
	free(data);
    }
}

void multiply_grd(nNetwork* NN, double value){
	for (int i=0;i<LEN(NN)-1;i++){
		for (int x=0;x<DPTH(NN)[i+1];x++){   
		    BGRD(NN)[i][x]*=value;
		}
		for (int x=0;x<DPTH(NN)[i];x++){	
			for (int y=0;y<DPTH(NN)[i+1];y++){
				WGRD(NN)[i][x][y]*=value;
			}
		}
	}
}
