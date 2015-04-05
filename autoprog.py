#!/usr/bin/env python3

import os
import sys
import argparse
import subprocess

try:
    import usb.core
    import usb.util
except ImportError:
    print("*** ERROR: This command requires PyUSB ***")
    exit()


class EagleChat:
    """
    Simply holds constants. Explicit over implicit, right?
    """

    VENDOR = 0x03eb
    PRODUCT = 0x2fe7
    COMMAND = 'avrdude -p x192a3u -c flip2 -U application:w:{file}' + \
                ' -P usb:{bus:0>3}:{dev:0>3}'

    @classmethod
    def get_eaglechat_devices(cls):
        return usb.core.find(find_all=True,
                             idVendor=cls.VENDOR,
                             idProduct=cls.PRODUCT)

    @classmethod
    def program_device(cls, device, file):
        cmd = cls.COMMAND.format(file=args.file,
                                 bus=device.bus,
                                 dev=device.address) \
                         .split(' ')

        try:
            usb.util.dispose_resources(device)
            subprocess.check_call(cmd, stdout=subprocess.DEVNULL,
                                  stderr=subprocess.DEVNULL)
            return True
        except FileNotFoundError: # avrdude was not found
            print("*** ERROR: This command requires avrdude")
            sys.exit(1)
        except subprocess.CalledProcessError: # avrdude had an error
            print('*** ERROR: avrdude had an error')
            sys.exit(1)


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('file', metavar='FILE', help='The binary file to \
        download to the boards.')

    args = parser.parse_args()

    devices = list(EagleChat.get_eaglechat_devices())

    if len(devices) == 0:
        print('*** ERROR: No EagleChat boards found')
        sys.exit(1)

    print("Found {} EalgeChat board(s) in DFU mode.".format(len(devices)))

    for dev in devices:
        EagleChat.program_device(dev, args.file)

    devices = None

    print('autoprog exiting successfully')
