// SPDX-License-Identifier: GPL-2.0
/*
 * (C) 2021 Carles Pey <cpey@pm.me>
 */

#ifndef HELPERS_H_
#define HELPERS_H_

typedef enum {false, true} bool;

void clear_warn_once(void);
void print_hex_buffer(char*, int);
int write_to_device(int, char**, int);
int read_from_device(int, char**, int);

#endif /* HELPERS_H_ */
