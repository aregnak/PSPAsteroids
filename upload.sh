#!/bin/sh

set -e

echo "Removing old EBOOT.PBP"
if (rm /run/media/areg/disk/PSP/GAME/Asteroids/EBOOT.PBP) then
    echo "Successfully Removed old EBOOT.PBP file"

    echo "Uploading new EBOOT.PBP file"
    if (cp ./build/EBOOT.PBP /run/media/areg/disk/PSP/GAME/Asteroids/EBOOT.PBP) then
        echo "Successfully uploaded, enjoy the game!"
    fi
fi



