//
//  AridLands.cpp
//
//  Created by Johan Van de Koppel on 03-09-14.
//  Copyright (c) 2014 Johan Van de Koppel. All rights reserved.
//

// Compiler directives
#define ON              1
#define OFF             0

#define Print_All_Devices OFF
#define Give_Error_Numbers OFF
#define Device_No       1    // 0: CPU; 1: Intel 4000; 2: Nvidia
#define ProgressBarWidth 45

#define StoreLog        OFF

// Thread block size
#define Block_Size_X	32                // 32
#define Block_Size_Y	32                // 32

// Number of blox
/* I define the Block_Number_ensions of the matrix as product of two numbers
Makes it easier to keep them a multiple of something (16, 32) when using CUDA*/
#define Block_Number_X	16               // 32
#define Block_Number_Y	16               // 32

// Matrix Block_Number_ensions
// (chosen as multiples of the thread block size for simplicity)
#define Grid_Width  (Block_Size_X * Block_Number_X)			// Matrix A width
#define Grid_Height (Block_Size_Y * Block_Number_Y)			// Matrix A height
#define Grid_Size (Grid_Width*Grid_Height)                   // Grid Size

// DIVIDE_INTO(x/y) for integers, used to determine # of blocks/warps etc.
#define DIVIDE_INTO(x,y) (((x) + (y) - 1)/(y))

// Definition of spatial parameters
#define dX          1                // 1     The size of each grid cell in X direction
#define dY          1                // 1     The size of each grid cell in Y direction

// Process parameters            Original value   Explanation and Units
#define	D           1.0;             // 1    - The diffusivity parameter
#define Gamma       0.5;             // 0.5  - The non-parameterical density-dependence parameter

#define Time		1                // 1     Start time of the simulation
#define dT          0.025            // 0.025 TIME STEP
#define NumFrames	300              // 100   Number of times the data is stored
#define MAX_STORE	(NumFrames+2)    // Determines the size of the storage array
#define EndTime		6000             // 1000  End time of the simulation

/* High definition setting

Block_Size_X&Y   = 32
Block_Number_X&Y = 16
dX/dY            = 0.5
dT               = 0.0025

 */



