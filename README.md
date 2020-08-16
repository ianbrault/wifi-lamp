# Raspberry Pi WiFi Lamp

Welcome! This guide details how to get your lamp up and running.

The main components of this repository are:

- `setup_wifi`: a script used to configure the WiFi connection on a device
- `lamp_cli`: a program which runs on a device and acts as a WebSocket client
which sends/receives messages to/from the paired device, and additionally
controls the actual lamp LEDs on the device
- `lamp_srv`: a central WebSocket server used to facilitate communication
between devices

## Initial Setup (for the developer)

### Set up the Raspberry Pi Zero to emulate Ethernet over USB

1. Mount the micro SD card and install Raspbian using `Raspberry Pi Imager`

2. Add the following line to the bottom of `/Volumes/boot/config.txt`:

```
dtoverlay=dwc2
```

3. Add the following to `/Volumes/boot/cmdline.txt`, following the `rootwait`
parameter:

```
modules-load=dwc2,g_ether
```

4. Run `touch /Volumes/boot/ssh` to enable SSH access

### Configure Raspbian

Update the package lists and upgrade all out-of-date packages:

```bash
$ sudo apt-get -y update
$ sudo apt-get -y upgrade
```

Install `git`:

```bash
$ sudo apt-get install -y git
```

### Set up SSH keys

```bash
$ ssh-copy-id -i keys/id_rsa.pub pi@raspberrypi.local
```

