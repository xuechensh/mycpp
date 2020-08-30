Plan1:
https://www.cnblogs.com/FengGeBlog/p/12994103.html

yum install gcc gcc-c++ -y

yum install -y gmp-devel mpfr-devel libmpc-devel

#already have at the same dir
wget ftp://gcc.gnu.org/pub/gcc/infrastructure/isl-0.15.tar.bz2

yum install -y bzip2

tar xf isl-0.15.tar.bz2
cd isl-0.15
./configure --prefix=/usr/local/isl-0.15
make
make install
ln -s /usr/local/isl-0.15 /usr/local/isl

#already have at the same dir
wget http://mirrors.nju.edu.cn/gnu/gcc/gcc-8.3.0/gcc-8.3.0.tar.gz

tar xf gcc-8.3.0.tar.gz

./configure --prefix=/usr/local/gcc8 --enable-language=c,c++,go --disable-multilib

#optional
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

make -j 4

make install

ln -sf /usr/local/gcc8/bin/gcc /usr/bin/gcc
ln -sf /usr/local/gcc8/bin/g++ /usr/bin/g++
ln -sf /usr/local/gcc8/include/c++/8.3.0 /usr/include/c++/8.3.0
ln -sf /usr/local/gcc8/lib64/libstdc++.so.6 /usr/lib64/libstdc++.so.6
