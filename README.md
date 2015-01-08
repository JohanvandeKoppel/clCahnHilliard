# clCahnHilliard

This repository contains the OpenCL code that runs the classic model describing phase-separation in for instance alloys, developed by John Cahn and John Hilliard in 1958:

J. W. Cahn and J. E. Hilliard. 1958. Free energy of a nonuniform system. I. Interfacial free energy. Journal of Chemical Physics 28, 258 (1958).

An video of the output of the model can be found on: https://www.youtube.com/watch?v=OYcXZ7Ho4o8 . This video has been produced by the code in this repository.

Running the code requires an OpenCL-compatible graphics card and OpenCL drivers installed. On a Mac, this is natively done when Xcode has been installed, and the current code has been developed in Xcode using a Macbook pro 2012 edition. You can also compile and run the code from the command line on a Mac. I run it using the cmake package (www.cmake.org) that handles the generation of a suitable makefile. See further explanation below.

On other computers, you have to install the drivers that come with a Nvidia/AMD or Intel graphics card, and use the sample or SDK files to build from.

If you run under Windows with a computer that has an Nvidia card, you have to install CUDA (which has OpenCL integrated) including SDK and the OpenCL samples within Visual Studio, and integrate the code files within a Visual studio project. The simplest way to do that is to copy one of the code samples into a new folder within the samples folder, and use an existing sample project file by replacing the code files. I cannot help you with this as I am a Mac user (I actually switched to Mac to avoid the visual studio headache, so be warned).

For Mac or Linux users: Presuming that cmake is installed, unzip the file somewhere, and open a command window in the build folder within the unzipped code folder. Now, run:

cmake ..; make; make run

If you want to clear the build, type clear; make destroy

The code itself does not show anything except for a progress bar, and saves the results to a binary file with a “dat” extension. To view the results, you have to use the PlotMussel.r R-project file (using R and RStudio I advice) or PlotMussel.m within Matlab. Note to set the folder settings correctly in the R script and to install the “fields” package within R (install.package(“fields”);)

Both scripts can make a movie. The R script is best for that, but you have to install the ffmpeg program, which you can find here: https://www.ffmpeg.org/

I hope the code it useful for you. Please drop me an email if you like to tell whether it worked for you or not.

Johan van de Koppel January 2015
