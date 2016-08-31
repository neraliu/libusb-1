/*
 * libusb example program to list devices on the bus
 * Copyright © 2007 Daniel Drake <dsd@gentoo.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>

#include "libusb.h"

#define LIBUSB_MAXSTR 1024

static void print_devs(libusb_device **devs)
{
	libusb_device *dev;
	int i = 0, j = 0, err = 0;
	uint8_t path[8]; 
	unsigned char* str = calloc(LIBUSB_MAXSTR, sizeof(unsigned char));

	while ((dev = devs[i++]) != NULL) {
		libusb_device_handle *handle;
		struct libusb_device_descriptor desc;

		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			fprintf(stderr, "failed to get device descriptor");
			return;
		}

		err = libusb_open(dev, &handle);
		if (!err) {
			libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, str, LIBUSB_MAXSTR);
			printf("iManufacturer %s\n", str);
			libusb_get_string_descriptor_ascii(handle, desc.iProduct, str, LIBUSB_MAXSTR);
			printf("iProduct %s\n", str);
			libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, str, LIBUSB_MAXSTR);
			printf("iSerialNumber %s\n", str);
			libusb_close(handle);
		}

		printf("idVendor %04x: iProduct %04x (bus %d, device %d)",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));

		r = libusb_get_port_numbers(dev, path, sizeof(path));
		if (r > 0) {
			printf(" path: %d", path[0]);
			for (j = 1; j < r; j++)
				printf(".%d", path[j]);
		}
		printf("\n");
	}

	free(str);
}

int main(void)
{
	libusb_device **devs;
	int r;
	ssize_t cnt;

	r = libusb_init(NULL);
	if (r < 0)
		return r;

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0)
		return (int) cnt;

	print_devs(devs);
	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);
	return 0;
}

/*
Reference: http://libusb.sourceforge.net/api-1.0/structlibusb__device__descriptor.html
*/
