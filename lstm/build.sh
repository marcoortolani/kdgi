rm -r build
mkdir build
cd build
cmake -G "Unix Makefiles" -DPYTHON_LIBRARY=/home/marco/anaconda3/envs/alearn/lib/libpython3.6m.so  ..
make