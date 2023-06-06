#!/bin/bash
sudo insmod encryptor.ko
sudo mknod /dev/encryptor c 302 0
sudo chmod 666 /dev/encryptor
