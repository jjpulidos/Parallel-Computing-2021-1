#!/bin/bash
clear
CWD=$(pwd)
mpicc -o mpi mpi.cpp `pkg-config --cflags --libs opencv` -lstdc++ -lmpi_cxx
clear
for file in $CWD/in/*
do
    for((i = 1; i <= 16; i = i + 1))
    do
      echo  "numero de procesos= ${i}, con imagen=$(basename $file)" >> logfile.txt
      mpirun -np ${i} --hostfile ../../mpi-host ./mpi in/$(basename $file) >>logfile.txt
    done
done
