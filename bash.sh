#!/bin/bash
cd /COM
sudo make clean
sudo make 
sudo make install
cd ..
cd ..
git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library
sudo make uninstall
sudo make clean
sudo make
sudo make install
cd ..
cd tp-2018-2c-Leaking-Bad/FS/Debug
sudo make clean
sudo make
git clone https://github.com/sisoputnfrba/fifa-examples.git
cd ..
cd ..
cd ..
