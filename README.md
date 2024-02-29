# libneodencam

Driver for Neoden 4 **old generation** cameras with `VID=0x0828,PID=0x1002`, following the API of `NeodenCamera.dll` for compatibility with the corresponding OpenPNP Java class. Based on libusb-1.0.

Tested on Linux. Might also compile on Windows, probably depending on the availability and compatibility of libusb on that plattform.

**Note: This is not a v4l driver.** It still needs the Neoden4Camera java class in OpenPNP. Turning this into a v4l driver might be worth a shot though.

## Building

```sh
mkdir build
cd build
cmake ..
make
```

## Testing

Requires golang

```sh
# First, build the shared library. It will be placed in ./build
go run main.go
```

## Steps to convert a Neoden 4 to OpenPNP on Linux

1. Image the built-in SSD using [clonezilla](https://clonezilla.org/) from a bootable USB – just in case you mess up and want to revert to stock.
2. Replace the disk with a bigger (>=32GB) mSATA SSD. Keep the old SSD in a safe place.
3. While you're at it, upgrade the machine from its 2GB to 4GB RAM (single SO-DIMM DDR3 module)
4. Install a (lightweight) linux distro of your choice.
5. Install the following packages:
    - gcc
    - cmake
    - make
    - libusb
    - pkg-config
    - git
6. Check out this repository, build and install the camera driver:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
    ```
