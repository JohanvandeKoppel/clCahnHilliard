//
//  AridLands main.cpp
//
//  Created by Johan Van de Koppel on 03-09-14.
//  Copyright (c) 2014 Johan Van de Koppel. All rights reserved.
//

#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <math.h>

#include "Settings_and_Parameters.h"
#include "Device_Utilities.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

// Forward definitions from AridLandsKernel.cl functions
void print_platform_info(cl_platform_id);
void print_device_info(cl_device_id* , int);
void Query(cl_uint, cl_device_id*);
void Get_Build_Errors(cl_program, cl_device_id*, cl_int );

// Forward definitions from functions at the end of this code file
void randomInit(float*, int, int);

////////////////////////////////////////////////////////////////////////////////
// Main program code for Aridlands
////////////////////////////////////////////////////////////////////////////////

int main()
{
    
    /*----------Constant and variable definition------------------------------*/
    
    unsigned int Grid_Cemory = sizeof(float) * Grid_Size;
	unsigned int size_storegrid = Grid_Width * Grid_Height * MAX_STORE;
	unsigned int mem_size_storegrid = sizeof(float) * size_storegrid;
    
    /*----------Defining and allocating memeory on host-----------------------*/
    
    // Defining and allocating the memory blocks for P, W, and O on the host (h)
	float* h_C;
    h_C = (float*)malloc(Grid_Size*sizeof(float));
	
    // Defining and allocating storage blocks for P, W, and O on the host (h)
	float* h_store_C;
    h_store_C=(float*) malloc(mem_size_storegrid);
    
    /*----------Initializing the host arrays----------------------------------*/
    
    srand(50);
	randomInit(h_C, Grid_Width, Grid_Height);
    
    /*----------Printing info to the screen ----------------------------------*/

	//system("clear");
    printf("\n");
	printf(" * * * * * * * * * * * * * * * * * * * * * * * * * * * * * \n");
	printf(" * The Cahn-Hilliard equation for phase separation       * \n");
	printf(" * OpenCL implementation : Johan van de Koppel, 2014     * \n");
	printf(" * Following a paper by Cahn & Hilliard 1958             * \n");
	printf(" * * * * * * * * * * * * * * * * * * * * * * * * * * * * * \n\n");
    
	printf(" Current grid dimensions: %d x %d cells\n\n",
           Grid_Width, Grid_Height);
    
    /*----------Setting up the device and the kernel--------------------------*/
    
    cl_device_id* devices;
    cl_int err;
    
    cl_context context = CreateGPUcontext(devices);
    
    // Print the name of the device that is used
    printf(" Implementing PDE on device %d: ", Device_No);
    print_device_info(devices, (int)Device_No);
    printf("\n");
    
    // Create a command queue on the device
    cl_command_queue command_queue = clCreateCommandQueue(context, devices[Device_No], 0, &err);
    
    /* Create Buffer Objects */
	cl_mem d_C   = clCreateBuffer(context, CL_MEM_READ_WRITE, Grid_Cemory, NULL, &err);
    cl_mem d_Int = clCreateBuffer(context, CL_MEM_READ_WRITE, Grid_Cemory, NULL, &err);
    
	/* Copy input data to the memory buffer */
	err = clEnqueueWriteBuffer(command_queue, d_C, CL_TRUE, 0, Grid_Width*Grid_Height*sizeof(float), h_C, 0, NULL, NULL);
    
    /*----------Building the PDE kernel---------------------------------------*/
    
    cl_program program = BuildKernelFile("Computing_Kernel.cl", context, &devices[Device_No], &err);
    if (err!=0)  printf(" > Compile Program Error number: %d \n\n", err);
    
    /*----------Linking of kernel phase 1-------------------------------------*/
    
	/* Create AridLands OpenCL kernel */
	cl_kernel kernel_1 = clCreateKernel(program, "CahnHilliard_Kernel_Phase1", &err);
    if (err!=0) { printf(" > Create Kernel 1 Error : %d \n\n", err); exit(1); }

	/* Set OpenCL kernel arguments */
	err  = clSetKernelArg(kernel_1, 0, sizeof(cl_mem), (void *)&d_C);
    err |= clSetKernelArg(kernel_1, 1, sizeof(cl_mem), (void *)&d_Int);
    if (err!=0) { printf(" > Memory binding error: %d \n\n", err); exit(1); }
    
    /*----------Linking of kernel phase 2-------------------------------------*/
    
    // Create AridLands OpenCL kernel
	cl_kernel kernel_2 = clCreateKernel(program, "CahnHilliard_Kernel_Phase2", &err);
    if (err!=0) { printf(" > Create Kernel 2 Error : %d \n\n", err); exit(1); }
    
	// Set OpenCL kernel arguments
	err  = clSetKernelArg(kernel_2, 0, sizeof(cl_mem), (void *)&d_C);
    err |= clSetKernelArg(kernel_2, 1, sizeof(cl_mem), (void *)&d_Int);
    if (err!=0) { printf(" > Memory binding error: %d \n\n", err); exit(1); }
    
    /*----------Set op timer and progress bar---------------------------------*/
    
    /* create and start timer */
    struct timeval Time_Measured;
    gettimeofday(&Time_Measured, NULL);
    double Time_Begin=Time_Measured.tv_sec+(Time_Measured.tv_usec/1000000.0);

    /* Progress bar initiation */
    int RealBarWidth=std::min((int)NumFrames,(int)ProgressBarWidth);
    int BarCounter=0;
    double BarThresholds[RealBarWidth];
    for (int i=1;i<RealBarWidth;i++)
        { BarThresholds[i] = (double)(i+1)/(double)RealBarWidth*(double)EndTime; };
    
    /* Print the reference bar */
    printf(" Progress: [");
    for (int i=0;i<RealBarWidth;i++) { printf("-"); }
    printf("]\n");
    fprintf(stderr, "           >");
    
	// Calculate the times at which the simulation is stored
    double RecordTimes[MAX_STORE];
    #if StoreLog == ON
        // For logarithmic time storing
        double Step=pow((double)EndTime,((double)1.0/(double)NumFrames));
        for(int i=0;i<=NumFrames;i++)
            { RecordTimes[i]=(double)pow(Step,(double)i); }
    #else
        // For normal time storing
        for(int i=0;i<=NumFrames;i++)
            { RecordTimes[i]=(double)i/(double)NumFrames*(double)EndTime; }
    #endif
    
    double time_elapsed = Time;
    int Counter = 0;
    
    // -------------- Kernel initiation -----------
    
	size_t global_item_size = Grid_Width*Grid_Height;
	size_t local_item_size = Block_Number_X*Block_Number_Y;
    
	while((float)time_elapsed<=(float)EndTime)
    {
        /* Execute OpenCL kernel as data parallel */
        
        err = clEnqueueNDRangeKernel(command_queue, kernel_1, 1, NULL,
                                 &global_item_size, &local_item_size, 0, NULL, NULL);
        err = err + clEnqueueNDRangeKernel(command_queue, kernel_2, 1, NULL,
                                    &global_item_size, &local_item_size, 0, NULL, NULL);
        
        if (err!=0) { printf("Kernel execution Error: %d\n\n", err); exit(1);}
        
        if((float)time_elapsed>=(float)RecordTimes[Counter])
        {
            // Transfer result to host 
            err = clEnqueueReadBuffer(command_queue, d_C, CL_TRUE, 0,
                    Grid_Width*Grid_Height*sizeof(float), h_C, 0, NULL, NULL);

            if (err!=0) { printf("Read Buffer Error: %d\n\n", err); }
            
            //Store values at this frame.
            memcpy(h_store_C+(Counter*Grid_Size),h_C,Grid_Cemory);
            
            Counter=Counter+1;
        }
        
        // Progress the progress bar if time
        if ((float)(time_elapsed)>=BarThresholds[BarCounter]) {
            fprintf(stderr,"*");
            BarCounter = BarCounter+1;
        }
        
        time_elapsed=time_elapsed+(double)dT;
        
    }
    
    printf("<\n\n");
    
    /*---------------------Report on time spending----------------------------*/
    
    gettimeofday(&Time_Measured, NULL);
    double Time_End=Time_Measured.tv_sec+(Time_Measured.tv_usec/1000000.0);
	printf(" Processing time: %4.5f (s) \n\n", Time_End-Time_Begin);
    
    /*---------------------Write to file now----------------------------------*/
    
    // The location of the code is obtain from the __FILE__ macro
    const std::string SourcePath (__FILE__);
    const std::string PathName = SourcePath.substr (0,SourcePath.find_last_of("/")+1);
    const std::string DataPath = PathName + "clCahnHilliard.dat";
    
	FILE * fp=fopen(DataPath.c_str(),"wb");

    int width_matrix = Grid_Width;
    int height_matrix = Grid_Height;
    int NumStored = NumFrames+1;

	// Storing parameters
	fwrite(&width_matrix,sizeof(int),1,fp);
	fwrite(&height_matrix,sizeof(int),1,fp);
	fwrite(&NumStored,sizeof(int),1,fp);
    
    for(int i=0;i<=NumStored;i++)
    { fwrite(&RecordTimes[i],sizeof(double),1,fp); }
	
	for(int store_i=0;store_i<NumStored;store_i++)
    {
		fwrite(&h_store_C[store_i*Grid_Size],sizeof(float),Grid_Size,fp);
    }
    
	fclose(fp);
    
	/*---------------------Clean up memory------------------------------------*/
	
    // Freeing host space
	free(h_C);
	free(h_store_C);
 
	// Freeing kernel and block space
	err = clFlush(command_queue);
	err = clFinish(command_queue);
	err = clReleaseKernel(kernel_1);
	err = clReleaseKernel(kernel_2);
	err = clReleaseProgram(program);
	err = clReleaseMemObject(d_C);
    err = clReleaseMemObject(d_Int);
	err = clReleaseCommandQueue(command_queue);
	err = clReleaseContext(context);
    free(devices);
    
    #if defined(__APPLE__) && defined(__MACH__)
        system("say Simulation finished");
    #endif

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Allocates a matrix with random float entries
////////////////////////////////////////////////////////////////////////////////

/*-----------------Allocates a matrix with random float entries---------------*/
void randomInit(float* data, int x_siz, int y_siz)
{
	for(int i=0;i<y_siz;i++)
	{
		for(int j=0;j<x_siz;j++)
		{
			data[i*x_siz+j] = (rand() / (float)RAND_MAX)*2-1;
		}
	}
    
} // End randomInit
