/**
 * (C) 2024 Jake Drahos <j@kedrahos.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
 *  02110-1301  USA
 *
 *  LibPSK API header.
 */

#ifndef _libpsk_h
#define _libpsk_h
#include <stdint.h>

#include "pskcoresdr.h"

#define PSK_MODE_PSK31 0
#define PSK_MODE_QPSKU 1
#define PSK_MODE_QPSKL 2
#define PSK_MODE_PSK63 8
#define PSK_MODE_PSK125 16

#define PSK_SQL_THRESH_DEF 50

#define PSK_SQL_SPEED_DEF 75
#define PSK_SQL_FAST 0
#define PSK_SQL_LOW 1

#define PSK_AFC_OFF 0
#define PSK_AFC_DEF -1

#ifdef __cplusplus
extern "C" {
#endif

/* Temporary typedefs for a minimal hacky wrapper */
typedef void * PSK_MOD;
typedef void * PSK_DET;

/* LibPSK API */
PSK_DET psk_d_create(int fs, 
			 int frequency,
			 int mode,
			 int squelch_thresh, 
			 int squelch_speed,
			 int afc_limit);

void psk_d_set_squelch(PSK_DET det, int limit);
void psk_d_set_afc(PSK_DET det, int limit);

int psk_d_run(PSK_DET det, int16_t * in_buf, int samples, int stride, char * res, int res_len);

#ifdef __cplusplus
}
#endif

#endif
