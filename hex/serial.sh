#!/bin/bash

echo "$@#"
sudo stty 9600 -F /dev/ttyUSB0
sudo chmod o+rw /dev/ttyUSB0
sudo echo "$@#" > /dev/ttyUSB0
