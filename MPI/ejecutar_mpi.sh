#!/bin/bash
clear
CWD=$(pwd)
mpicc -o mpi split-image.cpp `pkg-config --cflags --libs opencv` -lstdc++ -lmpi_cxx
clear
for file in $CWD/in/*
do
	for((i = 1; i <= 16; i = i + 1))
	do
    do  
      mpirun -np ${i} --hostfile ../../mpi-host ./mpi $(basename $file)
    done
done
