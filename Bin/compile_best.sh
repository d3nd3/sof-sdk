#!/bin/bash
source ~/.bash_aliases
MAP=$1
sofbsp.exe $MAP
sofvis.exe $MAP
sofarghrad.exe -gamedir "e:\sof\override\base" -extra -bounce 64 -chop 32 -chopsky 256 $MAP
cp $MAP.bsp "/mnt/e/sof/user-server/maps/test/"
cp $MAP.bsp "/mnt/e/sof/user/maps/test/"
uploadtest
