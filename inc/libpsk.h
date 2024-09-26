/**
 * \file libpsk.h
 * 
 * \brief libpsk.h: Header for LibPSK C API
 *
 * \copyright 
 * 
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

#define PSK_SQL_THRESH_DEF 0

#define PSK_SQL_SPEED_DEF 10
#define PSK_SQL_FAST 20
#define PSK_SQL_LOW 75

#define PSK_AFC_OFF 0
#define PSK_AFC_DEF -1

#ifdef __cplusplus
extern "C" {
#endif

/* Temporary typedefs for a minimal hacky wrapper */
typedef void * PSK_MOD;
typedef void * PSK_DET;

/* LibPSK API */

/**
 * \brief Create RX
 *
 * Create and configure a PSK detector (rx)
 *
 * \param fs Sample rate in samples/s
 * \param frequency Center frequency in Hz
 * \param mode (default PSK_MODE_PSK31)
 * \param squelch_thresh (default PSK_SQL_THRESH_DEF)
 * \param squelch_speed (default PSK_SQL_SPEED_DEF)
 * \param afc_limit (default PSK_AFC_DEF)
 * 
 * \return New PSK_DET
 */
PSK_DET psk_d_create(int fs, 
			 int frequency,
			 int mode,
			 int squelch_thresh, 
			 int squelch_speed,
			 int afc_limit);

/**
 * \brief Set squelch
 * 
 * Squelch can be set at runtime probably without breaking anything
 * 
 * \param det Detector instance
 * \param thresh Threshold (0-100ish), or PSK_SQL_THRESH_DEF
 * \param speed Squelch speed (0-100ish), or PSK_SQL_SPEED_ FAST, SLOW, DEF.
 */
void psk_d_set_squelch(PSK_DET det, int thresh, int speed);

/**
 * \brief Set Automatic Frequency Correction
 * 
 * \param det Detector instance
 * \param limit AFC limit
 */
void psk_d_set_afc(PSK_DET det, int limit);

/**
 * \brief Run a batch of samples
 * 
 * Note that samples is an array of 16-bit values. Expected number of samples
 * is samples * stride. The total number of bytes read is 2 * samples * stride.
 * 
 * Calculate the output array size to agree with the maximum number of samples
 * per batch to avoid overruns. 
 * 
 * \param det Detector instance
 * \param in_buf Buffer of signed 16 bit samples (native endianness).
 * \param sapmples Number of samples
 * \param stride Interlace samples for eg. stereo (default 1 for mono)
 * \param res Result buffer of decoded characters
 * \param res_len Maximum capacity of result buffer
 * 
 * \return Number of characters written to res, or -1 if overrun. In 
 * case of overrun, the number of valid in result buffer is necessarily res_len
 */
int psk_d_run(PSK_DET det, int16_t * in_buf, int samples, 
				int stride, char * res, int res_len);

#ifdef __cplusplus
}
#endif

#endif
