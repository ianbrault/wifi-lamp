#!/usr/bin/env python3
"""
setup_wifi.py
author: Ian Brault <ian.brault@engineering.ucla.edu>

connect to a Raspberry Pi over SSH (via a local USB connection) to:
  1. scan for available WiFi networks (from the RPi)
  2. prompt the user for a WPA2 password, if necessary
  3. setup and persist a WiFi connection on the RPi
"""

import getpass
import inspect
import os
import re
import subprocess as sp
import sys
import textwrap


_debug   = False
base_dir = os.path.dirname(os.path.abspath(__file__))
rpi_host = 'raspberrypi.local'
rpi_pwd  = 'blueberry'
idfile   = os.path.join(base_dir, 'rpi_id_rsa')
wiface   = 'wlan0'


def debug_log(msg):
    if _debug:
        print('%s: %s' % (inspect.stack()[1][3], msg))


class WifiNetwork:
    def __init__(self, mac):
        self.mac = mac
        # these will be assigned later
        self.essid = ''
        self.password_required = False


def ssh_cmd(rcmd):
    return ['ssh', '-i', idfile, 'pi@%s' % rpi_host, rcmd]


def scan_for_rpi():
    """
    Scan for the Raspberry Pi over the local network

    Returns the discovered IP address if successful, None otherwise
    """
    ip = None

    cmd = ['ping', '-c', '1', rpi_host]
    debug_log(' '.join(cmd))

    proc = sp.run(cmd, capture_output=True)
    debug_log('returned %d' % proc.returncode)

    if proc.returncode == 0:
        out = proc.stdout.decode('utf-8')
        debug_log(out)

        # extract IP address from ping output
        ip_re = r'\((\d{,3}\.\d{,3}\.\d{,3}\.\d{,3})\)'
        match = re.search('PING %s %s' % (rpi_host, ip_re), out)
        if match:
            ip = match.group(1)
            debug_log('found IP %s' % ip)

    else:
        debug_log(proc.stderr.decode('utf-8'))

    return ip


def parse_networks(output):
    """
    Parse the output of iwlist into WifiNetwork objects
    """
    networks = []
    nwk_names = set()  # track uniqueness of networks
    nwk = None

    for line in output.split('\n'):
        line = line.strip()

        if line.startswith('Cell'):
            # add the previous network to the list, if there is one
            if nwk is not None and nwk.essid not in nwk_names:
                networks.append(nwk)
                nwk_names.add(nwk.essid)

            # get the MAC address, create a new in-progress network
            mac = line.split(': ')[-1]
            debug_log('found network with MAC %s' % mac)
            nwk = WifiNetwork(mac)

        elif line.startswith('ESSID'):
            # strip the quotation marks
            essid = line.split(':')[-1][1:-1]
            debug_log('network %s ESSID "%s"' % (nwk.mac, essid))
            nwk.essid = essid

        elif line.startswith('Encryption key'):
            key_status = line.split(':')[-1]
            debug_log('network %s password %s' % (nwk.mac, key_status))
            nwk.password_required = key_status == 'on'

    if nwk is not None and nwk.essid not in nwk_names:
        networks.append(nwk)

    # sometimes iwlist returns a network full of null (\x00) bytes or a
    # network with an empty name
    return list(filter(lambda n: n.essid and '\\x00' not in n.essid, networks))


def get_network_list():
    """
    Get a list of available WiFi networks from the Raspberry Pi

    Returns a list of WifiNetwork objects if successful, [] otherwise
    """
    networks = []
    rcmd = 'echo %s | sudo iwlist %s scan' % (rpi_pwd, wiface)
    cmd = ssh_cmd(rcmd)
    debug_log(' '.join(cmd))

    proc = sp.run(cmd, capture_output=True)
    debug_log('returned %d' % proc.returncode)

    if proc.returncode == 0:
        out = proc.stdout.decode('utf-8')
        debug_log(out)

        networks = parse_networks(out)

    else:
        debug_log(proc.stderr.decode('utf-8'))

    return networks


def print_network_table(networks):
    sep = ' ' * 4
    rows = [['', 'ESSID', 'Password?']]
    for i, nwk in enumerate(networks, start=1):
        pwd = 'yes' if nwk.password_required else 'no'
        rows.append([str(i), nwk.essid, pwd])

    colsz = [len(rows[-1][0]), 0, len(rows[0][-1])]
    colsz[1] = max(map(lambda r: len(r[1]), rows))

    for row in rows:
        fmt = [str.rjust, str.ljust, str.ljust]
        cols = [f(s, z) for (s, z, f) in zip(row, colsz, fmt)]
        print('  ' + sep.join(cols))


def upload_wpa_file(nwk, pwd):
    """
    Upload a WPA supplicant file to the Raspberry Pi that contains the provided
    network details

    Returns a boolean indicating success
    """
    # prepare the WPA supplicant entry
    pwd_entry = 'key_mgmt=NONE' if pwd is None else ('psk="%s"' % pwd)
    entry = textwrap.dedent('''\
    ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
    update_config=1
    country=US
    network={
        ssid="%s"
        %s
    }
    ''') % (nwk.essid, pwd_entry)
    entry = entry.replace('\n', '\\n').replace('\t', '\\t')

    wpa_file = '/etc/wpa_supplicant/wpa_supplicant.conf'
    rcmd = 'echo -en \'%s\' | sudo tee %s > /dev/null' % (entry, wpa_file)
    cmd = ssh_cmd(rcmd)
    debug_log(' '.join(cmd))

    proc = sp.run(cmd, capture_output=True)
    debug_log('returned %d' % proc.returncode)
    if proc.returncode == 0:
        debug_log(proc.stdout.decode('utf-8'))
        return True
    else:
        debug_log(proc.stderr.decode('utf-8'))
        return False


def reconfigure_wiface(wiface):
    """
    Reconfigure the Raspberry Pi wireless interface

    Returns a boolean indicating success
    """
    # note the /sbin prefix: the command fails without it for some reason
    rcmd = '/sbin/wpa_cli -i %s reconfigure' % wiface
    cmd = ssh_cmd(rcmd)
    debug_log(' '.join(cmd))

    proc = sp.run(cmd, capture_output=True)
    debug_log('returned %d' % proc.returncode)

    if proc.returncode == 0:
        # have to check stdout, will print FAIL if configuration failed
        out = proc.stdout.decode('utf-8')
        debug_log(out)
        return out.strip() == 'OK'

    else:
        debug_log(proc.stderr.decode('utf-8'))
        return False


def connect_rpi_to_network(nwk, pwd):
    """
    Connect the Raspberry Pi to the provided network

    Returns a boolean indicating success
    """
    # upload the WPA supplicant file
    ok = upload_wpa_file(nwk, pwd)
    if not ok:
        return False

    # reconfigure the wireless interface to use the new network details
    return reconfigure_wiface(wiface)


def main():
    print('Scanning for the device... ', end='')
    ip_addr = scan_for_rpi()
    if ip_addr:
        print('DONE')
    else:
        print('ERROR\nCould not find the device: ensure that it is connected '
              'to your computer and Internet sharing is enabled')
        sys.exit(1)

    print('Scanning for WiFi networks... ', end='')
    networks = get_network_list()
    if networks:
        print('DONE\nFound %d Wifi networks' % len(networks))
    else:
        print('ERROR\nCould not scan for WiFi networks')
        sys.exit(1)

    print_network_table(networks)

    # prompt the user to select a network
    nwk_n = -1
    while not (0 <= nwk_n < len(networks)):
        nwk_s = input(
            'Which network would you like to connect to? (1-%d): '
            % len(networks))

        try:
            nwk_n = int(nwk_s) - 1
            if not (0 <= nwk_n < len(networks)):
                print('%d is out of range, try again' % (nwk_n + 1))
        except ValueError:
            print('"%s" is not a number, try again' % nwk_s)

    nwk = networks[nwk_n]

    # prompt the user for the network password (if required)
    pwd = None
    if nwk.password_required:
        pwd = getpass.getpass(
            prompt=('Enter the password for %s: ' % nwk.essid))

    print('Connecting the device to %s... ' % nwk.essid, end='')
    ok = connect_rpi_to_network(nwk, pwd)
    if ok:
        print('DONE')
    else:
        print('ERROR\nCould not connect the device to the network')
        sys.exit(1)


if __name__ == '__main__':
    if '-d' in sys.argv or '--debug' in sys.argv:
        _debug = True

    # FIXME: re-generate the SSH keys to support this script
    print('ERROR: the SSH keys are not currently configured, this script will '
          'fail to run')
    sys.exit(1)

    main()

