#!/bin/bash

# per lanciare da remoto:
# ssh kdgi@147.163.5.232 screen -d -m ./runexperiments.sh

cd kdgi/py/tests



expname="experiment_abbadingo_inter_n_struct_sim"

echo "Experiment "$expname > exp_out/test.out
date >> exp_out/test.out

echo "Experiment "$expname > exp_out/error.out
date >> exp_out/error.out


echo "START " >> exp_out/test.out

  
outputfile=exp_out/${expname}.out

source /home/kdgi/virtualenvs/kdgi-py2/bin/activate kdgi-py2

python abbadingo_inter_n_struct_sim.py > ${outputfile} 2>> exp_out/error.out

echo "END" >> exp_out/test.out

