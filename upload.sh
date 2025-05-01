#!/bin/sh

set -e

cd ./build

make

echo "Removing old EBOOT.PBP"
if (rm /run/media/areg/disk/PSP/GAME/Asteroids/EBOOT.PBP) then
    echo "Successfully Removed old EBOOT.PBP file"

    echo "Uploading new EBOOT.PBP file"
    if (cp ./EBOOT.PBP /run/media/areg/disk/PSP/GAME/Asteroids/EBOOT.PBP) then
        echo "Successfully uploaded, enjoy the game!"
    fi
fi



