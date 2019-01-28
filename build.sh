rm -r build
mkdir build
cd build
#cmake -G "Unix Makefiles" -Dpyembed=on -DPYTHON_LIBRARY=/home/marco/anaconda3/envs/alearn/lib/libpython3.6m.so  ..
cmake -G "Unix Makefiles" -Dpyembed=on -Dtest=on ..
make
