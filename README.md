# libneodencam

Driver for Neoden 4 **old generation** cameras with `VID=0x0828,PID=0x1002`, implementing the API of [NeodenCamera.dll](https://github.com/jrobgh/NewNDCamera/) for compatibility with the corresponding OpenPNP Java class. Uses libusb; protocol copied from [ripe909/NeodenCamera64](https://github.com/ripe909/NeodenCamera64).

Tested on Linux. Might or might not compile on Windows, probably depending on the availability and compatibility of libusb on that plattform.

**Note: This is not a v4l driver.** It still needs the special Neoden4Camera driver class in OpenPNP. Turning this code into a v4l driver might be an interesting project – anyone...? ;P

## Build & Install

```sh
mkdir build
cd build
cmake ..
make
sudo make install
```

## Testing

Requires golang. First build the shared library. It will be placed in ./build. Then run the following command. It will take a photo with each connected camera and save it to cwd.

```sh
go run main.go
```

---

**FOLLOWING SECTION MASSIVELY WIP!**

---

## Neoden 4 OpenPNP Linux How-to

### 1. Hardware & OS

1. Image the built-in SSD using [clonezilla](https://clonezilla.org/) from a bootable USB – in case you mess up and want to revert back. Remove the SSD and put it in a safe place
2. Install a bigger (>=16GB) mSATA SSD. Available on eBay for < 10€. You could get away with the built-in 8GB SSD by using a very slim Linux distro
3. While you're at it, upgrade the machine from its 2GB to 4GB RAM (single SO-DIMM DDR3 module)
4. Install a (lightweight) Linux distro of your choice. I used Lubuntu 23.10, but something even more lightweight wouldn't hurt.

### 2. Software

1. Install the following packages:
    - gcc
    - cmake
    - make
    - pkg-config
    - git
    - libusb-1.0
2. Check out this repository, build and install the camera driver according to the instructions above
3. Install OpenJDK v8. Serial communication between OpenPNP and the Neoden controller board did not work for me on Java v11
4. Install OpenPNP as per the official instructions

### 3. Configuration

This is rather complex. The OpenPNP Wiki recommends to configure a machine by following the _Issues and Solutions Wizard_.

#### Serial Port

On the stock mainboard, the machine interface is connected to `/dev/ttyS1` and talks at `115200` baud. Your user account must be granted access to the serial ports. In Lubuntu, this is done by adding your account to the `dialout` group (`sudo usermod -a -G dialout USERNAME`).

#### Cameras

In order to use both cameras, you need to add a `Neoden4Camera` and then two `Neoden4SwitcherCamera` instances, one for the head and one for the bottom camera. They both reference the Neoden4Camera instance in their config and each have an individual ID, configured under _Switcher Number_ in _Driver Settings_.

-   Camera IDs: `0` and `1` (contrary to 1 and 5 in the Windows driver)
-   Resolution: `1024x1024`
-   Exposure and Gain: Start with values around `20` – then adjust from there
