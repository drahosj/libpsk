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

#ifdef __cplusplus
extern "C" {
#endif

/* Temporary typedefs for a minimal hacky wrapper */
typedef void * PSK_MOD;
typedef void * PSK_DET;


/* Legacy exported function names */
PSK_MOD createPSKMod(int fs, double maxAmplitude);
void freePSKMod(PSK_MOD mod);
void SetTXFrequency(PSK_MOD mod, int frequency);
void CalcPSK(PSK_MOD mod, double * pData, int n, int stride);
void PutTxQue(PSK_MOD mod, int ch);
void SetMode(PSK_MOD mod, int mode);
void SetCallback(PSK_MOD mod, void * ctx, int function(void * ctx) cb);

PSK_DET createPSKDet();
void setupPSKDet(PSK_DET det, int fs);
int runPSKDet(PSK_DET det, double * inp, int len, int stride, char * res, int
		resLen);
void freePSKDet(PSK_DET det);
void SetSquelchThresh(PSK_DET det, int limit);
void SetRXFrequency(PSK_DET det, int freq);
int GetRXFrequency(PSK_DET det);
int GetSignalLevel(PSK_DET det);
void SetAFCLimit(PSK_DET det, int limit);
void ResetDetector(PSK_DET det);
void SetRXPSKMode(PSK_DET det, int mode);

/* LibPSK API */

inline PSK_MOD psk_m_create(int f_s) {
	return createPSKMod(f_s, 1.1);
}

#ifdef __cplusplus
}
#endif

#endif
