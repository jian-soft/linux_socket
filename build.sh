#!/bin/sh

SOURCE_FILE=main.c\ udp_server.c\ udp_client.c\ stat.c
SOURCE_FILE=${SOURCE_FILE}\ tcp_server.c\ tcp_client.c

gcc $SOURCE_FILE -o socket -lpthread -Wall

#arm-linux-gnueabihf-gcc --sysroot=/mnt $SOURCE_FILE -o socket -lpthread
#cp -v socket /mnt/home/nano
