#!/bin/bash 
CC="msp430-gcc -mmcu=msp430f1611" CXX="msp430-c++ -mmcu=msp430f1611" cmake -DARITH=msp-asm -DCMAKE_SYSTEM_NAME=Generic -DALIGN=2 -DARCH=MSP -DBENCH=1 "-DBN_METHD=BASIC;MULTP;MONTY;BASIC;BASIC;BASIC" -DCHECK=OFF -DCOLOR=OFF "-DCOMP:STRING=-O2 -g -mmcu=msp430f1611 -ffunction-sections -fdata-sections -fno-inline -mdisable-watchdog" -DDOCUM=OFF -DEP_DEPTH=3 -DEP_PLAIN=ON -DEP_ENDOM=OFF "-DFP_METHD=BASIC;COMBA;COMBA;QUICK;LOWER;BASIC" -DFP_PMERS=ON "-DLINK=-Wl,--gc-sections" -DSEED=ZERO -DSHLIB=OFF -DSTRIP=ON -DTESTS=1 -DTIMER=CYCLE -DVERBS=OFF -DWORD=16 -DFP_PRIME=160 -DBN_PRECI=160 -DMD_METHD=SHONE "-DWITH=FP;EP;EC;DV;CP;MD;BN" -DEC_ENDOM=OFF -DEC_METHD=PRIME -DRAND=FIPS $1
