#!/bin/bash

if [ -z $1 ]; then
	echo "Usage: ./setup.sh abs_path_to_gilearning_lib"
	exit 1
fi

libname=$(basename $1)
basedir=$(dirname $1)
basedir=${basedir%lib}

echo $libname
if [[ ! $libname =~ "libgigipy" ]]; then
	echo "$1 doesn't appear to be a proper lib name!"
	exit 1
fi

if [ ! -f $basedir/../src/gilearning/gi_dfa.h ]; then
	echo "$basedir is not a proper GI-learning source dir!"
	exit 1
fi

echo "Found library $libname in dir $basedir"

if [ -z "$VIRTUAL_ENV" ]; then
	echo "Virtual Environment needs to be activated first"
	exit 1
fi

echo "Installing for $VIRTUAL_ENV ..."

python setup.py install

if [ ! -f $VIRTUAL_ENV/lib/python2.7/site-packages/gi_gipy.so ]; then
	ln -s $basedir/lib/$libname $VIRTUAL_ENV/lib/python2.7/site-packages/gi_gipy.so
fi

echo "Done!"
