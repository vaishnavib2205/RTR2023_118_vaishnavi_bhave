// header files
// standard headers

#include<stdio.h>
#include<math.h>

// cuda headers

#include<CL/opencl.h>
#include"helper_timer.h"

// macros

#define BLOCK_WIDTH 32

// global variables 

cl_context oclContext;
cl_command_queue oclCommandQueue;

cl_program oclProgram;
cl_kernel oclkernel;

int *hostA = NULL;
int *hostB = NULL;
int *hostC = NULL;
int *gold = NULL;

cl_men deviceA = NULL;
cl_men deviceB = NULL;
cl_men deviceC = NULL;

float timeOnCPU = 0.0f;
float timeOnGPU = 0.0f;

const char *oclSourceCode = 
"__kernel void matMulGPU(__global A , __global B , __global C , int numARows , int numAColumns , int numBColumns , int numCColumns)"\
"{"\
"int row = get_global_id(0);"\
"int column = get_global_id(1);"\
"if((row < numARows) && (column < numBColumns))"\
"{"\
"int value = 0;"\
"for(int k = 0; k < numAColumns ; k++)"\
"{"\
"int a = A[row * numAColumns + k];"\
"int b = B[k * numBColumns + column];"\
"value += a*b;"\
"}"\
"C[row * numCColumns + column] = value;"\
"}"\
"}";


int main(int argc , char *argv[])
{
    // function declaration

    void InitA(int *data , int  , int);
    void InitB(int *data , int  , int);
    void matMulCPU(int* , int* , int* , int , int , int , int);
    void cleanup();

    // variable declaration

    int numARows = BLOCK_WIDTH;
    int numAColumns = BLOCK_WIDTH;
    int numBRows = BLOCK_WIDTH;
    int numBColumns = BLOCK_WIDTH;

    int numCRows = numARows;
    int numCColumns = numBColumns;

    int numGoldRows = numARows;
    int numGoldColumns = numBColumns;

    int sizeA = numARows * numAColumns * sizeof(int);
    int sizeB = numBRows * numBColumns * sizeof(int);
    int sizeC = numCRows * numCColumns * sizeof(int);
    int sizeGold = numGoldRows * numGoldColumns * sizeof(int);

    cl_int result;

    // code 
    // host memory allocation

    hostA = (int*)malloc(sizeA);
    if(hostA == NULL)
    {
        printf("Host Memory allocation is failed for hostA array. \n");
        cleanup();
        exit(EXIT_FAILURE); 
    }

    hostB = (int*)malloc(sizeB);
    if(hostB == NULL)
    {
        printf("Host Memory allocation is failed for hostB array. \n");
        cleanup();
        exit(EXIT_FAILURE); 
    }

    hostC = (int*)malloc(sizeC);
    if(hostC == NULL)
    {
        printf("Host Memory allocation is failed for hostC array. \n");
        cleanup();
        exit(EXIT_FAILURE); 
    }

    gold = (int*)malloc(sizeGold);
    if(gold == NULL)
    {
        printf("Host Memory allocation is failed for gold array. \n");
        cleanup();
        exit(EXIT_FAILURE); 
    }

    // printing matrix dimensions and sizes
    printf("The Dimensions Of Matrix 'hostA' Are : %d x %d :\n", numARows , numAColumns);
    printf("The Dimensions Of Matrix 'hostB' Are : %d x %d :\n", numBRows , numBColumns);
    printf("The Dimensions Of Matrix 'hostC' Are : %d x %d :\n", numCRows , numCColumns);
    printf("The Dimensions Of Matrix 'gold' Are : %d x %d :\n", numGoldRows , numGoldColumns);

    printf("Size Of Matrix hostA = %d \n", sizeA);
    printf("Size Of Matrix hostB = %d \n", sizeB);
    printf("Size Of Matrix hostC = %d \n", sizeC);
    printf("Size Of Matrix gold = %d \n", sizeGold);

    // fill source matrices

    InitA(hostA , numARows , numAColumns);
    InitB(hostB , numBRows , numBColumns);

        result = clGetPlatformIDs(1 , &oclPlatformID , NULL);
    if(result != CL_SUCCESS)
    {
        printf("clGetPlatformIDs() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // get opencl supporting CPU device's ID
    result = clGetDeviceIDs(oclPlatformID ,CL_DEVICE_TYPE_GPU , 1 , &oclDeviceID , NULL);
    if(result != CL_SUCCESS)
    {
        printf("clGetDeviceIDs() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }    

    // create OpenCL compute context
    oclContext = clCreateContext(NULL , 1 , &oclDeviceID , NULL , NULL , &result);
    if(result != CL_SUCCESS)
    {
        printf("clCreateContext() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // create command queue
    oclCommandQueue = clCreateCommandQueue(oclContext , oclDeviceID , 0 , &result);
    if(result != CL_SUCCESS)
    {
        printf("clCreateCommandQueue() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // create OpenCL program from .cl
    oclProgram = clCreateProgramWithSource(oclContext , 1 , (const char **)&oclSourceCode , NULL , &result);
    if(result != CL_SUCCESS)
    {
        printf("clCreateProgramWithSource() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }          

    // build OpenCl Program
    result = clBuildProgram(oclPlatform , 0 , NULL, NULL, NULL, NULL  );
    if(result != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];
        clGetProgramBuildInfo(oclProgram , oclDeviceID , CL_PROGRAM_BUILD_LOG , sizeof(buffer) , buffer , &len);
        printf("Program Build Log : %s\n", buffer);
        printf("clBuildProgram() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }   

    // create OpenCL kernel by passing kernel function name that we used in .cl file

    oclkernel = clCreateKernel(oclProgram , "vecAddGPU" , &result);
    if(result != CL_SUCCESS)
    {
        printf("clCreateKernel() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

        // device memory allocation

    deviceA = clCreateBuffer(oclContext , CL_MEM_READ_ONLY , size , NULL , &result);
    if(result != CL_SUCCESS)
    {
        printf("clCreateBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }          

    deviceB = clCreateBuffer(oclContext , CL_MEM_READ_ONLY , size , NULL , &result);
    if(result != CL_SUCCESS)
    {
        printf("clCreateBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    deviceC = clCreateBuffer(oclContext , CL_MEM_WRITE_ONLY , size , NULL , &result);
    if(result != CL_SUCCESS)
    {
        printf("clCreateBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 0th argument i.e deviceInput1
    result = clSetKernelArg(oclkernel , 0 , sizeof(cl_mem) , (void *)&deviceA );
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 1 argument i.e deviceInput2
    result = clSetKernelArg(oclkernel , 1 , sizeof(cl_mem) , (void *)&deviceB );
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 2 argument i.e deviceOutput
    result = clSetKernelArg(oclkernel , 2 , sizeof(cl_mem) , (void *)&deviceC);
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 3 argument i.e deviceOutput
    result = clSetKernelArg(oclkernel , 3 , sizeof(cl_mem) , (void *)&numARows);
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 4 argument i.e deviceOutput
    result = clSetKernelArg(oclkernel , 4 , sizeof(cl_mem) , (void *)&numAColumns);
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 5 argument i.e deviceOutput
    result = clSetKernelArg(oclkernel , 5 , sizeof(cl_mem) , (void *)&numBColumns);
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 6 argument i.e deviceOutput
    result = clSetKernelArg(oclkernel , 6 , sizeof(cl_mem) , (void *)&numCColumns);
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // readback result from the device (i.e from deviceOutput) into CPU variable 

    result = clEnqueueReadBuffer(oclCommandQueue , deviceC , CL_TRUE , 0 , size , hostOutput , 0 , NULL , NULL);
    if(result != CL_SUCCESS)
    {
        printf("clEnqueueReadBuffer() Failed: %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }  


    // matrix multiplication on host

    matMulCPU(hostA , hostB , hostC , numARows , numAColumns , numBColumns , numCColumns);

    // comparison

    int breakValue = 1;
    bool bAccuracy = true;
    for(int i = 0; i<numCRows * numCColumns; i++)
    {
        int val1 = gold[i];
        int val2 = hostC[i];
        if(val1 != val2)
        {
            bAccuracy = false;
            breakValue = i;
            break;
        }
    }

    char str[128];
    if(bAccuracy == false)
    {
        sprintf(str , "Comparision of CPU and GPU Matrix Multiplication is not accurate at array index %d", breakValue);
    }
    else
    {
        sprintf(str , "Comparision of CPU and GPU Matrix Multiplication is accurate");
    }

    printf("Time Taken for Matrix Multiplication on CPU = %.6f \n" , timeOnCPU);
    printf("Time Taken for Matrix Multiplication on GPU = %.6f \n" , timeOnGPU);
    printf("%s\n" , str);   

    // cleanup 

    cleanup();

    return 0;

}


void InitA(int *data , int row , int col)
{
    int num = 1;
    //code

    for(int i = 0; i<row;i++)
    {
        for(int j = 0; j<col;j++)
        {
            *(data + i * col + j) = num;
            num++;
        }
    }
}

void InitB(int *data , int row , int col)
{
    int num = BLOCK_WIDTH;
    //code

    for(int i = 0; i<row;i++)
    {
        for(int j = 0; j<col;j++)
        {
            *(data + i * col + j) = num;
            num--;
        }
    }
}


void matMulCPU(int* A , int* B , int* C , int numARows , int numAColumns , int numBColumns , int numCColumns)
{
    // code
    StopWatchInterface* timer = NULL;
    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);

    for(int i = 0; i<numARows;i++)
    {
        for(int j = 0; j<numBColumns;j++)
        {
            
                int value = 0.0f;
                for(int k = 0; k<numAColumns;k++)
                {
                    int a = A[i * numAColumns + k];
                    int b = B[i * numBColumns + j];
                    value += a*b;
                }
                C[i * numCColumns + j] = value;

            }
        }

        sdkStopTimer(&timer);
        timeOnCPU = sdkGetTimerValue(&timer);
        sdkDeleteTimer(&timer);
        timer = NULL;
}



void cleanup(void)
{
    // code
    if(deviceC)
    {
        clReleaseMemObject(deviceC);
        deviceC = NULL;
    }

    if(deviceB)
    {
        clReleaseMemObject(deviceB);
        deviceB = NULL;
    }

    if(deviceA)
    {
        clReleaseMemObject(deviceA);
        deviceA = NULL;
    }

    if(oclkernel)
    {
        clReleaseMemObject(oclkernel);
        oclkernel = NULL;
    }
    if(oclProgram)
    {
        clReleaseMemObject(oclProgram);
        oclProgram = NULL;
    }
    if(oclCommandQueue)
    {
        clReleaseMemObject(oclCommandQueue);
        oclCommandQueue = NULL;
    }
    if(oclContext)
    {
        clReleaseMemObject(oclContext);
        oclContext = NULL;
    }

    if(gold)
    {
        free(gold);
        gold = NULL;
    }

    if(hostC)
    {
        free(hostC);
        hostC = NULL;
    }
    if(hostB)
    {
        free(hostB);
        hostB = NULL;
    }
    if(hostA)
    {
        free(hostA);
        hostA = NULL;
    }



}

