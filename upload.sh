#!/bin/sh

set -e

# Replace with your PSP mount path
PSP_PATH="/run/media/areg/disk/"

cd ./build

make

echo "Removing old EBOOT.PBP"
if (rm $PSP_PATH/PSP/GAME/Asteroids/EBOOT.PBP) then
    echo "Successfully Removed old EBOOT.PBP file"

    echo "Uploading new EBOOT.PBP file"
    if (cp ./EBOOT.PBP $PSP_PATH/PSP/GAME/Asteroids/EBOOT.PBP) then
        echo "Successfully uploaded, enjoy the game!"
    fi
fi



