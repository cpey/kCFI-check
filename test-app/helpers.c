// SPDX-License-Identifier: GPL-2.0
/*
 * (C) 2021 Carles Pey <cpey@pm.me>
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "helpers.h"

#define WARN_ONCE  "/sys/kernel/debug/clear_warn_once"

void clear_warn_once() {
	int fd;
	char value = 1; 

	fd = open(WARN_ONCE, O_WRONLY);
	if (fd < 0) {
		printf("Unable open %s. Continuing...\n", WARN_ONCE);
		return;
	}
	if (write(fd, &value, 1) < 0) {
		printf("Unable to clear WARN_ONCE. Continuing...\n");
		return;
	}
	close(fd);
}

void print_hex_buffer(char* buf, int len) {
	int i;

	for (i = 0; i < len; i++) {
		if (i && !(i % 16)) {
			printf("\n"); 
		} else if (i && !(i % 8)) {
			printf(" "); 
		}
		printf("%02X", buf[i]); 
	}
	printf("\n"); 
}

/*
 * On success returns the number of bytes successfully written
 * On error returns -1
 */ 
int write_to_device(int fd, char** buf, int len) {
	bool alloc = false;
	int ret = 0;
	char err[200];

	if (*buf == NULL) {
		*buf = malloc(len);
		if (*buf == NULL) {
			perror("Error allocating memory buffer\n");
			ret = -1;
			goto out;
		}
		alloc = true;
	}

	memset(*buf, 'A', len);
	if ((ret = write(fd, *buf, len)) < 0) {
		sprintf(err, "Invalid length argument: %s\n", strerror(errno));
		perror(err);
		ret = -1;
		goto out_m;
	}

	goto out;

out_m:
	if (alloc) {
		free(*buf);
	}
out:
	return ret;
}

/*
 * On success returns the number of bytes successfully read
 * On error returns -1
 */ 
int read_from_device(int fd, char** buf, int len) {
	bool alloc = false;
	int ret = 0; 
	char err[200];

	if (*buf == NULL) {
		*buf = malloc(len);
		if (*buf == NULL) {
			perror("Error allocating memory buffer\n");
			ret = -1;
			goto out;
		}
		alloc = true;
	}

	memset(*buf, '0', len);
	if ((ret = read(fd, *buf, len)) < 0) {
		sprintf(err, "Error reading from the device: %s\n", strerror(errno));
		perror(err);
		ret = -1;
		goto out_m;
	}

	goto out;

out_m:
	if (alloc) {
		free(*buf);
	}
out:
	return ret;
}


