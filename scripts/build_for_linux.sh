#!/bin/bash

mkdir -p /code/build
cd /code/build
yum install -y openssl-devel wget xz
wget https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0-Linux-x86_64.sh
chmod +x cmake-3.17.0-Linux-x86_64.sh 
./cmake-3.17.0-Linux-x86_64.sh -y
wget https://gmplib.org/download/gmp/gmp-6.2.0.tar.xz
tar xf gmp-6.2.0.tar.xz
cd gmp-6.2.0
./configure
make
make install
cd ..
export PATH=/code/libff/build/cmake-3.17.0-Linux-x86_64/bin/:$PATH
cmake ..
make
cp libff/build/libff/libff.so pybn254
rm -f dist/pybn254*linux*.whl
/opt/python/cp37-cp37m/bin/python setup.py bdist_wheel

echo "Change the name of the resulting wheel to reflect the platform!"
