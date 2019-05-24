#! /bin/bash

# ./color_detect/color

./ckociemba/bin/kociemba $(cat color_detect/state.txt) > solution.txt

echo "$(cat solution.txt)#"
sudo stty 9600 -F /dev/ttyUSB0
sudo chmod o+rw /dev/ttyUSB0
sudo echo "$(cat solution.txt)#" > /dev/ttyUSB0
