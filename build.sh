if [ -d ./build ]; then
	rm -r build
fi

mkdir build
cd build
cmake -G "Unix Makefiles" -Dpython=on -Dpyembed=on -Dtest=on -DPYTHON_LIBRARY=/home/kdgi/anaconda3/envs/alearn/lib/libpython3.6m.so -DPYTHON_INCLUDE_DIR=/home/kdgi/anaconda3/envs/alearn/include/python3.6m ..
#cmake -G "Unix Makefiles" -Dpyembed=on ..
make
