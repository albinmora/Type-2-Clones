#include <iterator>
#include <vector>
#include <sstream> 
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cuda.h>
#include <string>
#include <stdlib.h>


//Cuda error checking - non mandatory
void cudaCheckError() {
 cudaError_t e=cudaGetLastError();
 if(e!=cudaSuccess) {
   printf("Cuda failure %s:%d: '%s'\n",__FILE__,__LINE__,cudaGetErrorString(e));
   exit(0); 
 }
}

//GPU kernel
__global__
void doDetection_cudaVersion(int **vector_array, int N,  int* result_clones){
               *result_clones=210;   

    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int e = blockIdx.x * blockDim.x + threadIdx.x; 
    if(e > i){
        if(vector_array[i][0] == vector_array[e][0] &&
               vector_array[i][1] == vector_array[e][1] &&
               vector_array[i][2] == vector_array[e][2] &&
               vector_array[i][3] == vector_array[e][3] &&
               vector_array[i][4] == vector_array[e][4] &&
               vector_array[i][5] == vector_array[e][5] &&
               vector_array[i][6] == vector_array[e][6]){
               (*result_clones)++;   
         }
    }
}


std::vector<std::vector<int>> doDetection(std::vector<std::vector<int>> vector_array){

    int i, j;
    std::vector<std::vector<int>> result_array;

       
    for(i = 0; i < (int) vector_array.size(); ++i){

        result_array.push_back(std::vector<int>());
    }

    for(i = 0; i < (int) vector_array.size(); ++i){

        for(j = i+1; j < (int) vector_array.size(); ++j){

            if(vector_array[i][0] == vector_array[j][0] &&
               vector_array[i][1] == vector_array[j][1] &&
               vector_array[i][2] == vector_array[j][2] &&
               vector_array[i][3] == vector_array[j][3] &&
               vector_array[i][4] == vector_array[j][4] &&
               vector_array[i][5] == vector_array[j][5] &&
               vector_array[i][6] == vector_array[j][6]){


                result_array[i].push_back(j);

            }
        }
    }

    return result_array;
}



int main(int argc,char **argv){
    //initialize data
    std::vector<std::vector<int>> vector_array;
    std::string line;
    std::ifstream myfile ("./metricas.txt");
    getline(myfile, line);
    myfile.close();

    std::vector<int> vect;
    std::stringstream ss(line);

    int limitM;

    while (ss >> limitM)
    {
        vect.push_back(limitM);

        if (ss.peek() == ',')
            ss.ignore();
    }

    vector_array.resize(vect.size()/7);
    for(int vecI=0; vecI < (vect.size()/7); vecI++){
        vector_array[vecI].resize(7);
    }
    int vector_array_h[vect.size()/7][7];
    for(int x=0; x< (vect.size()/7);x++){
        for(int y=0; y<7; y++){
                vector_array[x][y]=vect[x*7+y];
                vector_array_h[x][y]=vect[x*7+y];
        }
    }

    printf("Begin \n");
    //Block size and number
    int block_size, block_no;

    //memory allocation
    int** vector_array_d;

    block_size = 100; //threads per block
    block_no = vector_array.size()/block_size;
    
    //Work definition
    dim3 dimBlock(vector_array.size(), vector_array.size(),1);
    dim3 dimGrid(1,1,1);


    int *numClones = (int*)malloc(sizeof(int));;
    *numClones=10;
    int numClones_d =0;
    int *numClones_dptr;

    printf("Allocating device memory on host..\n");
    //GPU memory allocation
    cudaMalloc((void **) &vector_array_d, vector_array.size()*7*sizeof(int));
    cudaMalloc(&numClones_dptr, sizeof(int));
    memset(numClones,0,sizeof(int));

    printf("Copying to device..\n");
    cudaMemcpy(vector_array_d, vector_array_h, vector_array.size()*7*sizeof(int),cudaMemcpyHostToDevice);
    cudaMemcpy(numClones_dptr, numClones, sizeof(int),cudaMemcpyHostToDevice);

    clock_t start_d=clock();
    printf("Doing GPU clone detection... \n");
    doDetection_cudaVersion<<<dimBlock, dimGrid>>>(vector_array_d,vector_array.size(), numClones_dptr);
    cudaCheckError();

    //Wait for kernel call to finish
    cudaThreadSynchronize();

    clock_t end_d = clock();
    

    printf("Doing CPU clone detection... \n");
    clock_t start_h = clock();
    std::vector<std::vector<int>> result_array  = doDetection(vector_array);
    clock_t end_h = clock();
    std::cout << "Cantidad de clones encontrados serial: " << result_array[0].size() << "\n";


	
    //Time computing
    double time_d = (double)(end_d-start_d)/CLOCKS_PER_SEC;
    double time_h = (double)(end_h-start_h)/CLOCKS_PER_SEC;

    //Copying data back to host, this is a blocking call and will not start until all kernels are finished
    cudaMemcpy(numClones, numClones_dptr, sizeof(int), cudaMemcpyDeviceToHost);
    std::cout << "Cantidad de clones encontrados paralelo: " << *numClones << "\n";
    printf("GPU time = %fs \t CPU time = %fs\n", time_d, time_h);

    //Free GPU memory
    free(numClones);
    cudaFree(numClones_dptr);
    cudaFree(vector_array_d);
    return 0;
}
