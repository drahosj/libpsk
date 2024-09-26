//////////////////////////////////////////////////////////////////////
// PSKMod.h: interface for the CPSKMod class.
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
//

#if !defined(AFX_PSKMod_H__ACB4CEA2_C356_11D2_88B9_444553540000__INCLUDED_)
#define AFX_PSKMod_H__ACB4CEA2_C356_11D2_88B9_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// used to specify which mode to use
#define BPSK_MODE 0
#define QPSKU_MODE 1
#define QPSKL_MODE 2
#define TUNE_MODE 3
#define TUNE_MODE_WID 4
#define CW_MODE 5

struct PSKStruct
{
	double* iptr;
	double* qptr;
	int	next;
};

typedef PSKStruct PSKSTATE;

typedef int(* getNextCharCallback)(void* context);

class CPSKMod  
{
public:
	CPSKMod();
	virtual ~CPSKMod();
// PSK31 and CW modulator
	void InitPSKMod(int Fs, double MaxAmplitude);
	void CalcPSK( double* pData , int n, int stride);
	void PutTxQue(int txchar, int cntrl );
	void ClrQue();
	void SetCWIDSpeed(long speed);
	int GetTXCharsRemaining();
//	void SetTXCWID( PCHAR lpszIDstrg){m_CWIdString = lpszIDstrg;}
	void SetTXFreq(int freq)
	{
		m_TxFreq = (double)freq;
		m_PSKPhaseInc = m_2PI * m_TxFreq / m_SampleFreq;
	}
	void SetTXMode(int mode);
	void SetAutoShutoff(int val){m_NeedShutoff = val;}
	void SetAutoCWID(int val){ m_NeedCWid = val;}
	int GetTXState(){return m_TXState;}
	void SetSampleClkAdj(int ppm){m_ClkError = 1.0;};
//	void SetSampleClkAdj(int ppm){m_ClkError = 1.0 - (double)ppm/1000000.0;};
	void GetVectorData(long* VectData);

	getNextCharCallback getNextChar = nullptr;

	void* callbackContext = nullptr;
//	HWND m_hWnd;

private:
	int m_PSK63Mode;
	int m_PSK125Mode;		//AA6YQ 1.21
	int m_SatMode;
	int m_NoSquelchTail;
	int m_NeedCWid;
	int m_NeedShutoff;
	int m_TempNoSquelchTail;
	int m_TempNeedCWid;
	int m_TempNeedShutoff;
	long m_IQPhaseArray[20];
	int m_Fs;
	int m_CWIDSpeed;
	int m_SavedMode;
	int m_Channel;
	int m_VectLookup[6][2];
	int m_pTail;
	int m_pHead;
	int GetTxChar();
	int m_AmblePtr;
	int m_Preamble[33];
	int m_Postamble[33];
	int m_TXState;
	int m_PSKmode;
	int m_CWState;
	int m_CWtimer;
	int m_Ramp;
	int m_CWSetSpeed;
	int * m_pXmitQue;
//	CString m_CWIdString;
// PSK31 and CW generator variables
	int m_AddEndingZero;
	unsigned char m_PresentPhase;
	unsigned char m_Lastsymb;
	unsigned short m_TxShiftReg;
	unsigned short m_TxCodeWord;
	double m_SymbolRate;
	double m_TxFreq;
	double m_SampleFreq;
	double m_ClkError;
	double m_t;
	double m_RMSConstant;
	double m_2PI;
	double m_PSKSecPerSamp;
	double m_PSKTime;
	double m_PSKPeriodUpdate;
	double m_PSKPhaseInc;
	double* m_pPSKtxI;
	double* m_pPSKtxQ;

	int m_rampsize;
	double* PSKShapeTbl_Z;
	double* PSKShapeTbl_P;
	double* PSKShapeTbl_M;
	double* PSKShapeTbl_ZP;
	double* PSKShapeTbl_PZ;
	double* PSKShapeTbl_MZ;
	double* PSKShapeTbl_PM;
	double* PSKShapeTbl_MP;

	PSKSTATE PSKPhaseLookupTable[6][5];

//	CRITICAL_SECTION m_CriticalSection;	// use for keeping threads
										// from stomping on each other


// PSK31 and CW modulator private functions
	int GetChar( );
	unsigned char GetNextBPSKSymbol(void);
	unsigned char GetNextQPSKSymbol(void);
	unsigned char GetNextCWSymbol(void);
	unsigned char GetNextTuneSymbol(void);

	void DeleteRampBuffers(void);
	void CreateRampBuffers(int Fs);

};

#endif // !defined(AFX_PSKMod_H__ACB4CEA2_C356_11D2_88B9_444553540000__INCLUDED_)
