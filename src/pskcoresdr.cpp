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

// PSKCoreSDR.cpp : Defines the exported functions for the DLL application.
//

#include "pskdet.h"
#include "pskmod.h"
#include "pskcoresdr.h"


 void* createPSKDet()
{
	CPSKDet* d = new CPSKDet();
	return d;
}
// depreciated
 void* initPSKDet(int fs)
{
	CPSKDet* d = new CPSKDet();
	d->Init(fs);
	return d;
}
 void setupPSKDet(void* det, int fs)
{
	((CPSKDet*)det)->Init(fs);
}
 int runPSKDet(void* det, double * inp, int len, int stride, char* result, int resultLen)
{
	return ((CPSKDet*)det)->ProcPSKDet(inp, len, stride, result, resultLen);
}
 void freePSKDet(void* det)
{
	delete ((CPSKDet*)det);
}
 void SetSquelchThresh(void* det, int limit)
{
	((CPSKDet*)det)->SetSquelchThresh(limit, 75);
}
 void SetRXFrequency(void* det, int frequency)
{
	((CPSKDet*)det)->SetRXFrequency(frequency);
}
 int GetRXFrequency(void* det)
{
	return ((CPSKDet*)det)->GetRXFrequency();
}
 int GetSignalLevel(void* det)
{
	return ((CPSKDet*)det)->GetSignalLevel();
}
 void SetAFCLimit(void* det, int limit)
{
	((CPSKDet*)det)->SetAFCLimit(limit);
}
 void ResetDetector(void* det)
{
	((CPSKDet*)det)->ResetDetector();
}
 void SetRXPSKMode(void* det, int mode)
{
	((CPSKDet*)det)->SetRXPSKMode(mode);
}

 void* createPSKMod(int fs, double maxAmplitude)
{
	CPSKMod* m = new CPSKMod();
	m->InitPSKMod(fs,maxAmplitude);
	return m;
}
 void freePSKMod(void* mod)
{
	delete ((CPSKMod*)mod);
}
 void SetTXFrequency(void* mod, int frequency)
{
	((CPSKMod*)mod)->SetTXFreq(frequency);
}
 void CalcPSK(void* mod, double* pData, int n, int stride)
{
	((CPSKMod*)mod)->CalcPSK(pData, n, stride);
}
 void PutTxQue(void* mod, int ch)
{
	((CPSKMod*)mod)->PutTxQue(ch, false);
}
 void SetCallback(void* mod, void* context, getNextCharCallback callback)
{
	((CPSKMod*)mod)->callbackContext = context;
	((CPSKMod*)mod)->getNextChar = callback;
}
 void SetMode(void* mod, int mode)
{
	((CPSKMod*)mod)->SetTXMode(mode);
}
