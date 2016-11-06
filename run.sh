#Script to build kernel module, insmod module, build library and build benchmark tests

if [ "$1" = "list" ]
then
	cd ./kernel_module
elif [ "$1" = "hash" ]
then
	cd ./kernel_module
else
	cd ./kernel_module
fi

sudo make clean
sudo make
sudo mkdir /usr/local/include/keyvalue
sudo make install
sudo rmmod keyvalue
sudo insmod keyvalue.ko

cd ../library/
sudo make clean
sudo make all
sudo make install

cd ../benchmark/
export LD_LIBRARY_PATH=/usr/local/lib/
sudo make clean
sudo make all
sudo ldconfig

