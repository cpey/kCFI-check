// SPDX-License-Identifier: GPL-2.0
/*
 * kCFI user space test program
 *
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

#define DEVICE		"kcfi_test"
#define DEBUGFS		"/sys/kernel/debug"
#define ERR_MSG		"Error: "
#define POINTER_SIZE 8

#define HELPER_IOCTL_NUM 0xFF
#define CALL_FN1   _IO(HELPER_IOCTL_NUM, 0)
#define CALL_FN2   _IO(HELPER_IOCTL_NUM, 1)
#define CALL_FN3   _IO(HELPER_IOCTL_NUM, 2)
#define CALL_FN4   _IO(HELPER_IOCTL_NUM, 3)

typedef struct func_table {
	void (*fn1)(void);
	void (*fn2)(char);
	char (*fn3)(char);
	char (*fn4)(char);
} func_table;

int overwrite_fn(int fd, char *msg, char idx_dest, char idx_orig) { 
	int n;

	if ((n = lseek(fd, (idx_dest - 1) * POINTER_SIZE, SEEK_SET)) < 0) {
		perror(ERR_MSG);
		return 1;	
	}

	if ((n = write(fd, msg + (idx_orig - 1) * POINTER_SIZE, 
			POINTER_SIZE)) < 0) {
		perror(ERR_MSG);	
		return 1;	
	}

	if ((n = lseek(fd, 0, SEEK_SET)) < 0) {
		perror(ERR_MSG);	
		return 1;
	}

	return 0;
}

int main(int argc, char** argv) {
	char device[50];
	char msg[200];
	int ret, fd, err = 0;
	int n;

	if (sprintf(device, "%s/%s", DEBUGFS, DEVICE) < 0) {
		perror("sprintf error\n");
		err = -1;
		goto out;
	}

	fd = open(device, O_RDWR);	
	if (fd < 0) {
		sprintf(msg, "Error opening the device: %s\n", strerror(errno));
		perror(ERR_MSG);
		err = 1;
		goto out;
	}

	printf("+ Calling fn2 function...\n");
	ioctl(fd, CALL_FN2, NULL);

	printf("+ Reading fn pointers...\n");
	if ((n = read(fd, msg, sizeof(func_table))) < 0) {
		perror(ERR_MSG);
		err = 1;
		goto out;
	}
	print_hex_buffer(msg, n);

	printf("+ Overwriting fn1 pointer with fn2 ...\n");
	if ((n = overwrite_fn(fd, msg, 1, 2)) < 0)  {
		err = 1;
		goto out;
	}

	printf("+ Overwriting fn2 pointer with fn3 ...\n");
	if ((n = overwrite_fn(fd, msg, 2, 3)) < 0)  {
		err = 1;
		goto out;
	}

	printf("+ Reading fn pointers...\n");
	if ((n = read(fd, msg, sizeof(func_table))) < 0) {
		perror(ERR_MSG);
		err = 1;
		goto out;
	}
	print_hex_buffer(msg, n);

	printf("+ Calling fn2 function...\n");
	ioctl(fd, CALL_FN2, NULL);

	printf("+ Overwriting fn2 pointer with fn4 ...\n");
	if ((n = overwrite_fn(fd, msg, 2, 4)) < 0)  {
		err = 1;
		goto out;
	}

	printf("+ Reading fn pointers...\n");
	if ((n = read(fd, msg, sizeof(func_table))) < 0) {
		perror(ERR_MSG);
		err = 1;
		goto out;
	}
	print_hex_buffer(msg, n);

	printf("+ Calling fn2 function...\n");
	ioctl(fd, CALL_FN2, NULL);

	close(fd);
out:
	return err;
}
