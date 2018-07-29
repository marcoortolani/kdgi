#!/bin/bash

# per lanciare da remoto:
# ssh kdgi@147.163.5.232 screen -d -m ./runexperiments.sh

<<<<<<< HEAD
cd /home/kdgi
cd /kdgi/py/tests
=======
cd kdgi/py/tests

>>>>>>> 0f1ba218728be9e6db7f225c104343c2846b19fd


expname="exp-abbadingo_struct_sim"

echo "Experiment "$expname > exp_out/test.out
date >> exp_out/test.out

echo "Experiment "$expname > exp_out/error.out
date >> exp_out/error.out


echo "START " >> exp_out/test.out

  
outputfile=exp_out/${expname}.out

<<<<<<< HEAD
workon kdgi-py2
python abbadingo_intra_n_struct_sim.py > ${outputfile} 2>> error.out
=======
source /home/kdgi/virtualenvs/kdgi-py2/bin/activate kdgi-py2

python abbadingo_intra_n_struct_sim.py > ${outputfile} 2>> exp_out/error.out
>>>>>>> 0f1ba218728be9e6db7f225c104343c2846b19fd

echo "END" >> exp_out/test.out

