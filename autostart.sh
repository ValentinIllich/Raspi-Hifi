#!/bin/bash
## Remount /dev/shm to prevent memory allocation errors
sudo mount -o remount,size=128M /dev/shm
#
sudo chmod a+w /sys/class/i2c-adapter/i2c-1/new_device
sudo echo ds1307 0x68 > /sys/class/i2c-adapter/i2c-1/new_device
sudo hwclock --adjust
sudo hwclock -s
#
sudo /home/pi/Raspi-Hifi-V1.0.2/Raspi-Hifi-V1 &
sudo /bin/mount -t vfat -o uid=pi,gid=pi /dev/sda1 /home/pi/usbstick/
