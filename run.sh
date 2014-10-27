#!/bin/bash
for SWSC_SIZE in 16 32 64 128 256
do
	make clean	
	make SWSC_SPM_SIZE=$SWSC_SIZE
	make run
done
