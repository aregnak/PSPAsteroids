# ASTEROIDS

### Description
Classic and very simple Asteroids game using PSPGU library in C.
Started as a project to learn more about C and SDKs, and to show love for a relic of a console :)

### Dependencies:
pspdev

### How To Compile:
#### Skip to CMAKE step if on Ubuntu, Debian, Fedora or Arch
#### OPTIONAL:
Enter docker:
```bash
sudo docker run -ti -v .:/source pspdev/pspdev:latest
cd /source
```

#### CMAKE:
```bash
mkdir build
cd build
psp-cmake ..
make
```

#### Using upload.sh file
The upload.sh file builds and uploads the EBOOT.PBP file to your PSP automatically.

MAKE SURE TO CHANGE THE `PSP_PATH` VARIABLE TO YOUR PSP MOUNT POINT!

Enjoy!

