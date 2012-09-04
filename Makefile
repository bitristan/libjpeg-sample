# Makefile for compiling the sample application demosntrating programming with libjpeg.
# To compile the program, just type make in the sample directory.
# 
# Junaed Sattar
# October 30, 2005.

CC=gcc

#
# Note: the -g3 flag is used to incorporate debugging information into the binary executables.
# This is done so that debugging the program is easier. Binaries compiled with -gN (N being 1,
# 2 or 3) are usually larger and slower than programs without debugging information.
# 
# Once your program is debugged and seems to be working properly, you can (ofcourse you don't 
# have to) change the -g3 flags to -O2, which optimizes your code for faster execution times
# and/or smaller binaries.
#

#
# The -ljpeg flag is used to tell the compiler to use the libjpeg library to compile and link
# your program against. If omitted, you will get a few "unresolved symbol" errors.
#
# 
jpeg_sample:jpeg_sample.o
	$(CC) -o jpeg_sample jpeg_sample.o -g3 -ljpeg
	
jpeg_sample.o: jpeg_sample.c
	$(CC) -g3 -c jpeg_sample.c -o jpeg_sample.o 

clean:
	rm -rf *.o jpeg_sample
