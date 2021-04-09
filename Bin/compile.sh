#!/bin/bash
source ~/.bash_aliases
MAP=$1
sofbsp.exe $MAP
sofvis.exe -fast $MAP
sofarghrad.exe -gamedir "e:\sof\override\base" -glview -chop 128 -chopsky 256 $MAP
cp $MAP.bsp "/mnt/e/sof/user-server/maps/"
cp $MAP.bsp "/mnt/e/sof/user/maps/"
uploadtest
