/*
 * syslog_relay.c
 * Relay the syslog of a device to stdout
 *
 * Copyright (c) 2009 Martin Szulecki All Rights Reserved.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA 
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>

#include <libiphone/libiphone.h>
#include <usbmuxd.h>

static int quit_flag = 0;

void print_usage(int argc, char **argv);

/**
 * signal handler function for cleaning up properly
 */
static void clean_exit(int sig)
{
	fprintf(stderr, "Exiting...\n");
	quit_flag++;
}

int main(int argc, char *argv[])
{
	iphone_lckd_client_t control = NULL;
	iphone_device_t phone = NULL;
	iphone_error_t ret = IPHONE_E_UNKNOWN_ERROR;
	int i;
	char uuid[41];
	int port = 0;
	uuid[0] = 0;

	signal(SIGINT, clean_exit);
	signal(SIGQUIT, clean_exit);
	signal(SIGTERM, clean_exit);
	signal(SIGPIPE, SIG_IGN);

	/* parse cmdline args */
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug")) {
			iphone_set_debug_mask(DBGMASK_ALL);
			iphone_set_debug(1);
			continue;
		}
		else if (!strcmp(argv[i], "-u") || !strcmp(argv[i], "--uuid")) {
			i++;
			if (!argv[i] || (strlen(argv[i]) != 40)) {
				print_usage(argc, argv);
				return 0;
			}
			strcpy(uuid, argv[i]);
			continue;
		}
		else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			print_usage(argc, argv);
			return 0;
		}
		else {
			print_usage(argc, argv);
			return 0;
		}
	}

	if (uuid[0] != 0) {
		ret = iphone_get_device_by_uuid(&phone, uuid);
		if (ret != IPHONE_E_SUCCESS) {
			printf("No device found with uuid %s, is it plugged in?\n", uuid);
			return -1;
		}
	}
	else
	{
		ret = iphone_get_device(&phone);
		if (ret != IPHONE_E_SUCCESS) {
			printf("No device found, is it plugged in?\n");
			return -1;
		}
	}

	if (IPHONE_E_SUCCESS != iphone_lckd_new_client(phone, &control)) {
		iphone_free_device(phone);
		return -1;
	}

	/* start syslog_relay service and retrieve port */
	ret = iphone_lckd_start_service(control, "com.apple.syslog_relay", &port);
	if ((ret == IPHONE_E_SUCCESS) && port) {
		iphone_lckd_free_client(control);
		
		/* connect to socket relay messages */
		
		int sfd = usbmuxd_connect(iphone_get_device_handle(phone), port);
		if (sfd < 0) {
			printf("ERROR: Could not open usbmux connection.\n");
		} else {
			while (!quit_flag) {
				char *receive = NULL;
				uint32_t datalen = 0, bytes = 0, recv_bytes = 0;

				ret = usbmuxd_recv(sfd, (char *) &datalen, sizeof(datalen), &bytes);
				datalen = ntohl(datalen);

				if (datalen == 0)
					continue;

				recv_bytes += bytes;
				receive = (char *) malloc(sizeof(char) * datalen);

				while (!quit_flag && (recv_bytes <= datalen)) {
					ret = usbmuxd_recv(sfd, receive, datalen, &bytes);

					if (bytes == 0)
						break;

					recv_bytes += bytes;

					fwrite(receive, sizeof(char), bytes, stdout);
				}

				free(receive);
			}
		}
		usbmuxd_disconnect(sfd);
	} else {
		printf("ERROR: Could not start service com.apple.syslog_relay.\n");
	}

	iphone_free_device(phone);

	return 0;
}

void print_usage(int argc, char **argv)
{
	char *name = NULL;
	
	name = strrchr(argv[0], '/');
	printf("Usage: %s [OPTIONS]\n", (name ? name + 1: argv[0]));
	printf("Relay syslog of a connected iPhone/iPod Touch.\n\n");
	printf("  -d, --debug\t\tenable communication debugging\n");
	printf("  -u, --uuid UUID\ttarget specific device by its 40-digit device UUID\n");
	printf("  -h, --help\t\tprints usage information\n");
	printf("\n");
}

