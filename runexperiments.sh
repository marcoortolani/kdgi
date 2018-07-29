#!/bin/bash

# per lanciare da remoto:
# ssh kdgi@147.163.5.232 screen -d -m ./runexperiments.sh

cd /home/kdgi
cd /kdgi/py/tests


expname="exp-abbadingo_struct_sim"

echo "Experiment "$expname > test.out
date >> test.out

echo "Experiment "$expname > error.out
date >> error.out


echo "START " >> test.out

  
outputfile=${expname}.out

workon kdgi-py2
python abbadingo_intra_n_struct_sim.py > ${outputfile} 2>> error.out

echo "END" >> test.out

