#!/bin/bash
cd ../home/utnso
git clone https://github.com/sisoputnfrba/tp-2018-2c-Leaking-Bad.git
cd tp-2018-2c-Leaking-Bad/COM/lqvg
sudo make clean
sudo make sudo make install
cd ..
cd ..
cd FS/Debug
sudo make clean
sudo make
git clone https://github.com/sisoputnfrba/fifa-examples.git
cd ..
cd ..
cd ..
git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library
sudo make uninstall
sudo make clean
sudo make
sudo make install