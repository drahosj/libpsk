//////////////////////////////////////////////////////////////////////
// PskDet.h: interface for the CPSKDet class.
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// PSK31Core Library for transmission and reception of PSK31 signals
//    using a PC soundcard  or .wav files.
//	               Copyright 2000, Moe Wheatley, AE4JY
//
//This library is free software; you can redistribute it and/or
//modify it under the terms of the GNU Lesser General Public
//License as published by the Free Software Foundation; either
//version 2.1 of the License, or (at your option) any later version.
//
//This library is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public
//License along with this library; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// http://www.gnu.org/copyleft/lesser.html
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PskDet_H__0C099641_E9AB_11D2_88B9_444553540000__INCLUDED_)
#define AFX_PskDet_H__0C099641_E9AB_11D2_88B9_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <math.h>
#include "pskdettbl.h"
#include "calcimd.h"

#define BPSK_MODE 0
#define QPSKU_MODE 1
#define QPSKL_MODE 2

#define PSK63_MODE (1<<3)
#define PSK125_MODE (1<<4)	//AA6YQ 1.21

#define SQMODEFAST 0
#define SQMODESLOW 1

struct SurvivorStates
{
	double Pathdistance;	// sum of all metrics for a given survivor path
	long BitEstimates;		// the bit pattern estimate associated with given survivor path
};

class CPSKDet  
{
public:
	CPSKDet();
	virtual ~CPSKDet();

	void Init(int Fs );
	int ProcPSKDet(double* pIn, int nSamples, int stride, char* result, int resultLen);
	void GetVectorData(long* VectData);
	void GetSyncData(long* SyncData);
	void ResetDetector();
	void SetRXFrequency(int freq);

	void SetRXPSKMode(int mode)
	{
		//char buf[200];
		//sprintf( buf, " Mode=%32u ",mode);
		//AfxMessageBox( buf );

		m_PSK63Mode = 0;		//AA6YQ 1.21
		m_PSK125Mode = 0;

		if(mode&PSK63_MODE) 			
			m_PSK63Mode = 1;

		if(mode&PSK125_MODE)  
			m_PSK125Mode = 1;
		
		m_RxMode = mode & (~PSK63_MODE) & (~PSK125_MODE);
	};

	void SetAFCLimit(int limit);
	void SetSampleClkAdj(int ppm);
	int GetRXFrequency(){return m_RxFrequency;};
	int GetSignalLevel()
	{ 
		if(m_SQLevel>0)
			return m_SQLevel;
		else
			return 0;
	};
	void SetSquelchThresh(int thresh,int speed)
	{
		m_SQThresh = thresh;
		m_SquelchSpeed = speed;
	};
//public variables
	

private:
//methods
	void CalcBitFilter(  _complex Samp);
	void CalcAGC( _complex Samp );
	char DecodeSymb( _complex newsamp );
	void CalcQuality(double angle );
	int ViterbiDecode( double newangle);
	int SymbSync(_complex sample);
	double CalcDistance( _complex newsymb, unsigned char symb);
	void CalcFreqError( _complex IQ );
	void CalcFFreqError( _complex IQ );
// other classes
	CCalcIMD CalcIMD;
//variables
	int m_FastAFCMode;
	int m_AFCCaptureOn;
	int m_IMDValid;
	int m_SQOpen;
	int m_LastBitZero;
	int m_PSK63Mode;
	int m_PSK125Mode;				//AA6YQ 1.21
	unsigned char m_VaricodeDecTbl[2048];
	unsigned short m_BitAcc;
	long m_IQPhaseArray[20];
	long m_SyncArray[20];
	int m_AFCTimer;
	int m_AFCmode;
	int m_RxMode;
	int m_SampleClkAdj;
	int m_AFCcounter;
	int m_IMDValue;
	int m_IQPhzIndex;
	int m_SquelchSpeed;
//	int m_RxChannel;
	int m_SQLevel;
	int m_SQThresh;
	int m_ClkErrTimer;
	int m_ClkErrCounter;
	int m_ClkError;
	int m_LastPkPos;
	int m_OnCount;
	int m_OffCount;
	int m_TrigInhib;
	int m_SampCnt;
//	int m_BlockSize;
	int m_RxFrequency;
	int m_Fs;
	int m_Fir1State;
	int m_Fir2State;
	int m_Fir3State;
	double m_FreqError;
	double m_QPSKprob[4];
	double m_DevAve;
	double m_I0;		// 4 stage I/Q delay line variables
	double m_I1;
	double m_Q0;
	double m_Q1;
	double m_BitPhaseInc;
	double m_BitPhasePos;
	double m_SyncAve[21];
	double m_NCOphzinc;
	double m_SampleFreq;
	double m_AFClimit;
	double m_AFCmax;
	double m_AFCmin;
	double m_NLPk;
	_complex m_pQue1[DEC4_LPFIR_LENGTH];
	_complex m_pQue2[DEC4_LPFIR_LENGTH];
	_complex m_pQue3[BITFIR_LENGTH];
	_complex m_FreqSignal;
	_complex m_BitSignal;

	struct SurvivorStates m_SurvivorStates[16]; /* Survivor path trellis */

// Local variables for various functions that need to be saved between calls
	int	m_PkPos;
	int	m_NewPkPos;
	int m_BitPos;
	int m_Pcnt;
	int m_Ncnt;
	double m_AGCave;
	double m_FperrAve;
	double m_FferrAve;
	double m_QFreqError;
	double m_VcoPhz;
	_complex m_z1;
	_complex m_z2;
};

#endif // !defined(AFX_PskDet_H__0C099641_E9AB_11D2_88B9_444553540000__INCLUDED_)
