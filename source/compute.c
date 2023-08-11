#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "compute.h"

double sigmoid(double n){
    return (1/(1+pow(EULER_NUMBER, -n)));
}

double sigmoidprime(double n){
    return (pow(EULER_NUMBER,-n)/pow(1+pow(EULER_NUMBER,-n),2));
}

void compute(double *input, double **output, nNetwork *NN){
    size_t maxsize=0;
    printf ("Computing with inputs: [");
    for ( int i=0;i<NN->depths[0];i++){
        printf ("%.1f",input[i]);
        if (i<NN->depths[0]-1){
         printf (", ");
        } else {
         printf ("]");
        }
    }
    printf ("=");
    for (int i=0;i<NN->depths[0];i++){NN->activations[0][i][0]=input[i];}
    for (int i=0;i<NN->len-1;i++){
        printf ("=");
        for (int y=0;y<NN->depths[i+1];y++){
            NN->activations[i+1][y][0]=0;
            for (int x=0;x<NN->depths[i];x++){
                NN->activations[i+1][y][1]+=(NN->activations[i][x][0]*NN->weights[i][x][y]); 
            }
            NN->activations[i+1][y][0]=sigmoid(NN->activations[i+1][y][1]+NN->bias[i][y]);
        }
    }
    for (int i=0;i<NN->depths[NN->len-1];i++){
        (*output)[i]=NN->activations[NN->len-1][i][0];
    }
    printf (">Finished\n");
}

//split data set if entries and expected are a simple list of activations
void splitData(int num_obj, int len_in, int len_out, double ***train_data, double*** input, double*** expected){
    	for (int i=0;i<num_obj;i++){
	    (*input)[i]=(double*)malloc(len_in*sizeof(double));
	    for (int y=0;y<len_in;y++){
                (*input)[i][y]=train_data[i][0][y];
            }
	}
	for (int i=0;i<num_obj;i++){
		(*expected)[i]=(double*)malloc(len_out*sizeof(double));
		for (int y=0;y<len_out;y++){(*expected)[i][y]=train_data[i][1][y];}
	}
}

// compute regression
double regression_cost(double expected, double output){
    return expected-output;
}

double sqr_regression(double expected, double output){
    return pow(expected-output,2);
}

double binary_prime(double expected, double output){
    return (expected/output)-((1-expected)/(1-output));
}

// compute binary cost
double binary_cost(double expected, double output){
    return -((expected*log(output))+((1-expected)*log(1-output)));
}

double cost (double expected, double output, int function){
    switch(function){
        case REGRESSION:
            return regression_cost(expected,output);
        break;
        case SQR_REG:
            return sqr_regression(expected,output);
            break;
        case BINARY:
            return binary_cost(expected,output);
        break;
    }
    return ERR;
}

double sum_cost(double *expected, double *output, int len, int function){
    double local_cost=0;
    for (int i=0;i<len;i++){
        local_cost+=cost(expected[i],output[i],function);
    }
    return local_cost;
}



double MSE_cost(double* expected, double* output, int len){
    return sum_cost(expected, output,len,SQR_REG)/len;
}

double MAE_cost(double* expected, double* output, int len){
    return sum_cost(expected, output,len,REGRESSION)/len;
}

double multiclass_cost(double* expected, double* output, int len){
    return sum_cost(expected, output,len,BINARY);
}

double avg_cost(double *expected, double *output, int len, int function){
    switch (function){
        case MSE:
            return MSE_cost(expected,output,len);
            break;
        case MAE:
            return MAE_cost(expected,output,len);
            break;
        case MULTICLASS:
            return multiclass_cost(expected,output,len);
            break;
    }
    return ERR;
}
