# Raspberry Pi WiFi Lamp

Welcome! This guide details how to get your lamp up and running.

The main components of this repository are:

- `lamp_srv`: a central WebSocket server used to facilitate communication
between users and devices
- `lamp_dev_cli`: a program that runs on a Raspberry Pi and acts as a WebSocket
client that sends/receives messages to/from the paired device, and additionally
controls the actual lamp hardware
- `lamp_usr_cli`: an iOS application that acts as a WebSocket client that is
used to turn the lamp on and off
- `lamp_protocol`: a library containing common WebSocket utilities shared
between `lamp_srv` and `lamp_cli`
- `lamp_ios_lib`: an FFI extension of `lamp_protocol` that is cross-compiled to
iOS architectures for use in the `lamp_usr_cli` application
- `dbg_usr_cli`: a command-line program that emulates user client functionality
- `setup_wifi`: a script used to configure the WiFi connection on a device

# Developer Setup

## Raspberry Pi

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

Install Rust:

```bash
$ curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

### Set up SSH keys

```bash
$ ssh-copy-id -i keys/id_rsa.pub pi@raspberrypi.local
```

## Building the iOS Client

The iOS client uses the `lamp_protocol` library internally, which must be
cross-compiled for the iOS architectures.

Add the architecture to rustup, and install `cargo-lipo`:

```bash
$ rustup target add aarch64-apple-ios x86_64-apple-ios
$ cargo install cargo-lipo
```
