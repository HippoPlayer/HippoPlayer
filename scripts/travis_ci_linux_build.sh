#/bin/bash

sudo add-apt-repository ppa:beineri/opt-qt571-trusty --yes
sudo apt-get update
sudo apt-get install qt57base mesa-common-dev
sudo apt-get install -y libasound2-dev alsa-utils alsa-oss
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-6 90;
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 90;
git clone --recursive https://github.com/deplinenoise/tundra.git
cd tundra
make
sudo make install
cd ..
export QT5=/opt/qt57
g++ --version
tundra2 -v linux-gcc-debug
