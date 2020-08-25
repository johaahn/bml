# bml
Binary Markup Language

For more details: http://www.gnu-log.net/home/bml



# bml library bindings


The bml library can be compiled and used in other languages. 

The language binding for Matlab, Octave and Python are provided.


## Matlab (Windows)

### Overview

The Matlab binding of the bml library creates mex functions to manipulate the bml files that can be called from Matlab.

The following instructions describes how to compile and use the bml library in Matlab.


### Prerequisites

A Visual Studio or MinGW C/C++ compiler installation is required for generating the bml Matlab mex functions. 


### Getting the required Source Code  and Build

1. Checkout bml library from Github:
	
	* ``git clone https://github.com/johaahn/bml.git``

2. Build bml:

	* Open Matlab at the bml root directory

	* Move to the directory <*bml root dir*>/*octave* 
		
		``>> cd octave``

	* Run the script *compile_matlab.m*
		
		``>> compile_matlab``

If the mex compiling process completed successfully the following mex functions are generated in the octave directory:

*	*bml_open.mexw64*
*	*bml_read.mexw64*
*	*bml_write.mexw64*
*	*bml_close.mexw64*


### Matlab Mex Compiler Configuration (Optional)

The Matlab mex compiler can compile C/C++ code using the Visual Studio and MinGW C++ compilers. 
	
To check which compiler is selected open Matlab and use the command:

``>> mex -setup``
	
To manually select the compilers type the following command line:

For Visual Studio 2017 compiler: 

``>> mex -setup:'<path to the MATLAB dir>\R2020a\bin\win64\mexopts\msvc2017.xml' ``

For MinGW compiler:

``>> mex -setup:'<path to the MATLAB dir>\R2020a\bin\win64\mexopts\mingw64.xml'``


### Run the sample test

* Go to the directory <*bml root dir*>/*octave*/*test*
	
	``>> cd test``

* Run the script test_bml.m 
	
	``>> test_bml``



	
## Octave (Linux, Windows)


### Overview

The Octave binding of the bml library creates mex functions to manipulate the bml files that can be called from Octave.

The following instructions describes how to compile and use the bml library in Octave. 


### Getting the required Source Code  and Build

1. Checkout bml library from Github:
	
	* ``git clone https://github.com/johaahn/bml.git``

2. Build bml:

	* Open a terminal shell at the bml root directory

	* Move to the directory <*bml root dir*>/*octave* 
		
		``cd octave``

	* Run the command line to execute the Octave script *compile.m* 
	
		Linux:
		``octave-cli --eval compile`` 
		
		Windows 
		``octave-cli.exe --eval compile``

If the Octave mex compiling completed successfully the following mex functions are generated in the octave directory:

*	*bml_open.mex*
*	*bml_read.mex*
*	*bml_write.mex*
*	*bml_close.mex*


### Run the sample test

* Open from a terminal shell and go to the directory <*bml root dir*>/*octave* 
	
	``cd test``

* Open Octave or Octave command line 

	Linux:
	``octave-cli`` 
		
	Windows 
	``octave-cli.exe``

* Run the script *test_bml.m* 
	
	``>> test_bml``



## Python

1. Checkout bml library from Github:
	
	* ``git clone https://github.com/johaahn/bml.git``
	
	* TO DO
	