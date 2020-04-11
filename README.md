# Raspberry Pi WiFi Lamp

Welcome! This guide details how to get your lamp up and running.

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

Install the required Qt libraries:

```bash
$ sudo apt-get install -y build-essential
$ sudo apt-get install -y qt5-default
$ sudo apt-get install -y qtconnectivity5-dev
```

### Configure Bluetooth (BlueZ)

Edit `/etc/systemd/system/bluetooth.target.wants/bluetooth.service` and set
`ExecStart=/usr/lib/bluetooth/bluetoothd --noplugin=sap`.

Edit `/lib/systemd/system/bthelper@.service` and add `ExecStartPre=/bin/sleep 2`
