// header files
// standard files 

#include<stdio.h>

// OpenCL header files

#include<CL/opencl.h>

// global variables 

const int iNumberOfArraysElements = 5;

cl_platform_id oclPlatformID;
cl_device_id oclDeviceID;

cl_context oclContext;
cl_command_queue oclCommandQueue;

cl_program oclProgram;
cl_kernel oclkernel;

float *hostInput1 = NULL;
float *hostInput2 = NULL;
float *hostOutput = NULL;

cl_men deviceInput1 = NULL;
cl_men deviceInput2 = NULL;
cl_men deviceOutput = NULL;

// opencl kernel

const char *oclSourceCode = 
"__kernel void vecADDGPU(__global float *in1, __global float *in2, __global float *out, int len)"\
"{"\
"int i=get_global_id(0);"\
"if(i < len)"\
"{"\
"out[i]=in1[i]+in2[i];"\
"}"\
"}";


// entry point function

int main(void)
{
    //function declaration
    void cleanup(void);

    // variable declaration
    int size = iNumberOfArraysElements * sizeof(float);
    cl_int result;

    // code
    // host memory allocation

    hostInput1 = (float*)malloc(size);
    if(hostInput1 == NULL)
    {
        printf("Host Memory allocation is failed for hostInput1 array.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    hostInput2 = (float*)malloc(size);
    if(hostInput2 == NULL)
    {
        printf("Host Memory allocation is failed for hostInput2 array.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    hostOutput = (float*)malloc(size);
    if(hostOutput == NULL)
    {
        printf("Host Memory allocation is failed for hostOutput array.\n");
        cleanup();
        exit(EXIT_FAILURE);
    }

    // filling values into host arrays

    hostInput1[0] = 101.0;
    hostInput1[1] = 102.0;
    hostInput1[2] = 103.0;
    hostInput1[3] = 104.0;
    hostInput1[4] = 105.0;

    hostInput2[0] = 201.0;
    hostInput2[1] = 202.0;
    hostInput2[2] = 203.0;
    hostInput2[3] = 204.0;
    hostInput2[4] = 205.0;

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

    deviceInput1 = clCreateBuffer(oclContext , CL_MEM_READ_ONLY , size , NULL , &result);
    if(result != CL_SUCCESS)
    {
        printf("clCreateBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }          

    deviceInput2 = clCreateBuffer(oclContext , CL_MEM_READ_ONLY , size , NULL , &result);
    if(result != CL_SUCCESS)
    {
        printf("clCreateBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    deviceOutput = clCreateBuffer(oclContext , CL_MEM_WRITE_ONLY , size , NULL , &result);
    if(result != CL_SUCCESS)
    {
        printf("clCreateBuffer() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 0th argument i.e deviceInput1
    result = clSetKernelArg(oclkernel , 0 , sizeof(cl_mem) , (void *)&deviceInput1 );
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 0th argument i.e deviceInput2
    result = clSetKernelArg(oclkernel , 1 , sizeof(cl_mem) , (void *)&deviceInput2 );
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 0th argument i.e deviceOutput
    result = clSetKernelArg(oclkernel , 2 , sizeof(cl_mem) , (void *)&deviceOutput);
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // set 0 based 0th argument i.e len
    result = clSetKernelArg(oclkernel , 3 , sizeof(cl_mem) , (void *)&iNumberOfArraysElements);
    if(result != CL_SUCCESS)
    {
        printf("clSetKernelArg() Failed : %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }

    // write above 'input' device buffer to device memory

    result = clEnqueueWriteBuffer(oclCommandQueue , deviceInput1 , CL_FALSE , 0 , size , hostInput1 , 0 , NULL , NULL);
    if(result != CL_SUCCESS)
    {
        printf("clEnqueueWriteBuffer() Failed for first input buffer: %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }    

    result = clEnqueueWriteBuffer(oclCommandQueue , deviceInput2 , CL_FALSE , 0 , size , hostInput2 , 0 , NULL , NULL);
    if(result != CL_SUCCESS)
    {
        printf("clEnqueueWriteBuffer() Failed for second input buffer: %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    } 

    // kernel configration

    size_t global_size = 5;
    result = clEnqueueNDRangeKernel(oclCommandQueue , oclkernel , 1 , NULL , &global_size , NULL , 0 , NULL , NULL);
     if(result != CL_SUCCESS)
    {
        printf("clEnqueueNDRangeKernel() Failed: %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }  

    //finish OpenCL command queue 

    clFinish(oclCommandQueue);

    // readback result from the device (i.e from deviceOutput) into CPU variable 

    result = clEnqueueReadBuffer(oclCommandQueue , deviceOutput , CL_TRUE , 0 , size , hostOutput , 0 , NULL , NULL);
    if(result != CL_SUCCESS)
    {
        printf("clEnqueueReadBuffer() Failed: %d\n", result);
        cleanup();
        exit(EXIT_FAILURE);        
    }       

    // display results

    int i;
    for(i = 0 ; i < iNumberOfArraysElements; i++)
    {
        printf("%f + %f = %f\n" , hostInput1[i] , hostInput2[i] , hostOutput[i]);
    }

    cleanup();

    return 0;
}

void cleanup(void)
{
    // code

    if(deviceOutput)
    {
        clReleaseMemObject(deviceOutput);
        deviceOutput = NULL;
    }
    if(deviceInput2)
    {
        clReleaseMemObject(deviceInput2);
        deviceInput2 = NULL;
    }
    if(deviceInput1)
    {
        clReleaseMemObject(deviceInput1);
        deviceInput1 = NULL;
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
    if(hostOutput)
    {
        clReleaseMemObject(hostOutput);
        hostOutput = NULL;
    }
    if(hostInput2)
    {
        clReleaseMemObject(deviceInput2);
        hostInput2 = NULL;
    }
    if(hostInput1)
    {
        clReleaseMemObject(hostInput1);
        hostInput1 = NULL;
    }

}