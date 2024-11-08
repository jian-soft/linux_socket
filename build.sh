#!/bin/sh

SOURCE_FILE=main.c\ udp_server.c

#arm-linux-gnueabihf-gcc --sysroot=/mnt main.c udp_server.c -o main
echo 123
echo ${SOURCE_FILE}

gcc $SOURCE_FILE -o main
#cp -v main /mnt/home/nano
