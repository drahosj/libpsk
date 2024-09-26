/* Copyright (C)
* 2000 - Moe Wheatley, AE4JY
* 2016 - Alan Hopper, 2E0NNB
* 2016 - John Melton, G0ORX/N6LYT
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PSKCORE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
//  functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef __cplusplus
extern "C" {
#endif

	typedef int(* getNextCharCallback)(void* context);

	 void* createPSKDet();
	 void* initPSKDet(int fs);
	 void setupPSKDet(void* det, int fs);

	 int runPSKDet(void* det, double * inp, int len, int stride, char* result, int resultLen);
	 void freePSKDet(void* det);
	 void SetSquelchThresh(void* det, int limit);
	 void SetRXFrequency(void* det, int frequency);
	 int GetRXFrequency(void* det);
	 int GetSignalLevel(void* det);
	 void SetAFCLimit(void* det, int limit);
	 void ResetDetector(void* det);
	 void SetRXPSKMode(void* det,int mode);

	 void* createPSKMod(int fs, double maxAmplitude);
	 void freePSKMod(void* mod);
	 void SetTXFrequency(void* mod, int frequency);
	 void CalcPSK(void* mod, double* pData, int n, int stride);
	 void PutTxQue(void* mod, int ch);
	 void SetCallback(void* mod, void* context, getNextCharCallback callback);
	 void SetMode(void* mod, int mode);


#ifdef __cplusplus
}
#endif
