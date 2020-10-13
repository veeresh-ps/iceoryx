#!/bin/bash

#Script is to monitor the process information

pidof $1 $2 > temp

while pid=" ," read a b; do
	pid_1=$a
	pid_2=$b
done < temp

rm temp

echo $pid_1
echo $pid_2

top -d 0.1 -p $pid_1 -p $pid_2 -b -n 3 > 1.txt
