//////////////////////////////////////////////////////////////////////
// PSKMod.cpp: implementation of the CPSKMod class.
//
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

#include <math.h>
#include <ctype.h>
#include <stddef.h>
#include "psktables.h"
//#include "Pskcore.h"
#include "pskmod.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

// local defines.................

#define BACK_SPACE_CODE 0x08	// define some control codes for

#define PHZ_0 0			//specify various signal phase states
#define PHZ_90 1
#define PHZ_180 2
#define PHZ_270 3
#define PHZ_OFF 4

#define SYM_NOCHANGE 0	//Stay the same phase
#define SYM_P90 1		//Plus 90  deg
#define SYM_P180 2		//Plus 180 deg
#define SYM_M90 3		//Minus 90 deg
#define SYM_OFF 4		//No output
#define SYM_ON 5		//constant output

#define TXOFF_CODE -1			// control codes that can be placed in the input
#define TXON_CODE -2			// queue for various control functions
#define TXTOG_CODE -3

#define TX_CNTRL_AUTOSTOP 1
#define TX_CNTRL_ADDCWID 2
#define TX_CNTRL_NOSQTAIL 3

#define TX_OFF_STATE 0			//TX is off
#define TX_SENDING_STATE 1		//TX is sending text
#define TX_PAUSED_STATE 2		//TX is paused
#define TX_PREAMBLE_STATE 3		//TX sending starting preamble
#define TX_POSTAMBLE_STATE 4	//TX sending ending posteamble
#define TX_CWID_STATE 5			//TX sending CW ID
#define TX_TUNE_STATE 6			//TX is tuning mode


#define SYMBOL_RATE31 31.25		// 31.25 Symbols per Second
#define SYMBOL_RATE63 62.5		// PSK63 62.50 Symbols per Second
#define SYMBOL_RATE125 125		// PSK125 125 Symbols per Second    //AA6YQ 1.21

//#define MAXRAMP_SIZE ( ((100*8160)/3125) +1) // max number of envelope ramp steps per symbol


#define TX_CONSTANT 22000.0		// TX Amplitude Factor
//#define TX_CONSTANT 750.0		// TX Amplitude Factor(threshold if noise ==16384/5

#define CW_SPEED 2	// wpm = 2.4*dotperiod(in msec) = 2.4*32/(2*CW_SPEED) = 38.4/CW_SPEED

#define TX_BUF_SIZE 2048

// The use of static's here preclude having multiple instantiations
// of this class but should not be an issue since only one soundcard.

//Ramp shape tables that are loaded with cosine shaped functions at init
/*
static double PSKShapeTbl_Z[MAXRAMP_SIZE];	// 0 
static double PSKShapeTbl_P[MAXRAMP_SIZE];	// +1
static double PSKShapeTbl_M[MAXRAMP_SIZE];	// -1
static double PSKShapeTbl_ZP[MAXRAMP_SIZE];	// 0 to +1
static double PSKShapeTbl_PZ[MAXRAMP_SIZE];	// +1 to 0
static double PSKShapeTbl_MZ[MAXRAMP_SIZE];	// -1 to 0
static double PSKShapeTbl_PM[MAXRAMP_SIZE];	// +1 to -1
static double PSKShapeTbl_MP[MAXRAMP_SIZE];	// -1 to +1
*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPSKMod::CPSKMod()
{
	m_2PI = 8.0 * atan(1.0);		//make constant 2 PI
	m_Fs = 8000;
	m_ClkError = 1.0;
	m_AmblePtr = 0;
	m_NeedCWid = 0;
	m_NeedShutoff = 0;
	m_NoSquelchTail = 0;
	m_TempNeedCWid = 0;
	m_TempNeedShutoff = 0;
	m_TempNoSquelchTail = 0;
	m_PSKmode = BPSK_MODE;
	m_CWSetSpeed = CW_SPEED;
	m_pTail = 0;
	m_pHead = 0;
	m_TxFreq = 1500.0;
//	m_CWIdString = "Call Not Set";
	m_pXmitQue = NULL;
	m_pXmitQue = new int[ TX_BUF_SIZE + 10];
//	InitializeCriticalSection(&m_CriticalSection);

	PSKShapeTbl_Z = NULL;
	PSKShapeTbl_P = NULL;
	PSKShapeTbl_M = NULL;
	PSKShapeTbl_ZP = NULL;
	PSKShapeTbl_PZ = NULL;
	PSKShapeTbl_MZ = NULL;
	PSKShapeTbl_PM = NULL;
	PSKShapeTbl_MP = NULL;
}

CPSKMod::~CPSKMod()
{
//	DeleteCriticalSection(&m_CriticalSection);
	if(m_pXmitQue)
		delete m_pXmitQue;
	DeleteRampBuffers();

	m_pXmitQue = NULL;
}
void CPSKMod::DeleteRampBuffers()
{
	if (PSKShapeTbl_Z) delete  PSKShapeTbl_Z;
	if (PSKShapeTbl_P) delete PSKShapeTbl_P;
	if (PSKShapeTbl_M) delete PSKShapeTbl_M;
	if (PSKShapeTbl_ZP) delete PSKShapeTbl_ZP;
	if (PSKShapeTbl_PZ) delete PSKShapeTbl_PZ;
	if (PSKShapeTbl_MZ) delete PSKShapeTbl_MZ;
	if (PSKShapeTbl_PM) delete PSKShapeTbl_PM;
	if (PSKShapeTbl_MP) delete PSKShapeTbl_MP;

}
void CPSKMod::CreateRampBuffers(int Fs)
{
	/* converted from static implementation to allow multiple instances with different sample rates 2E0NNB*/
	DeleteRampBuffers();
	m_rampsize = (100 * Fs) / 3125 + 1;

	PSKShapeTbl_Z = new double[m_rampsize];
	PSKShapeTbl_P = new double[m_rampsize];
	PSKShapeTbl_M = new double[m_rampsize];
	PSKShapeTbl_ZP = new double[m_rampsize];
	PSKShapeTbl_PZ = new double[m_rampsize];
	PSKShapeTbl_MZ = new double[m_rampsize];
	PSKShapeTbl_PM = new double[m_rampsize];
	PSKShapeTbl_MP = new double[m_rampsize];

	//Lookup table for determining the next ramp shape depending on the
	//  next symbol and the present output phase.
	// indexing format is [symbol][presentPhase]
	//returns the PSKSTRUCT containing the next phase and the I and Q
	//   ramp table pointers.

	// SYMBOL = 0 = SYM_NOCHANGE
	//   I ramp shape     Q ramp shape     Next Phase

	PSKPhaseLookupTable[0][0] = { PSKShapeTbl_P, PSKShapeTbl_P, PHZ_0 };	//present PHZ_0
	PSKPhaseLookupTable[0][1] = { PSKShapeTbl_M, PSKShapeTbl_P, PHZ_90 };	//present PHZ_90
	PSKPhaseLookupTable[0][2] = { PSKShapeTbl_M, PSKShapeTbl_M, PHZ_180 };	//present PHZ_180
	PSKPhaseLookupTable[0][3] = { PSKShapeTbl_P, PSKShapeTbl_M, PHZ_270 };	//present PHZ_270
	PSKPhaseLookupTable[0][4] = { PSKShapeTbl_Z, PSKShapeTbl_Z, PHZ_OFF };	//present PHZ_OFF
	// SYMBOL = 1 = SYM_P90 = Advance 90 degrees
	//   I ramp shape     Q ramp shape     Next Phase
	PSKPhaseLookupTable[1][0] = { PSKShapeTbl_PM, PSKShapeTbl_P, PHZ_90 };	//present PHZ_0
	PSKPhaseLookupTable[1][1] = { PSKShapeTbl_M, PSKShapeTbl_PM, PHZ_180 };	//present PHZ_90
	PSKPhaseLookupTable[1][2] = { PSKShapeTbl_MP, PSKShapeTbl_M, PHZ_270 };	//present PHZ_180
	PSKPhaseLookupTable[1][3] = { PSKShapeTbl_P, PSKShapeTbl_MP, PHZ_0 };	//present PHZ_270
	PSKPhaseLookupTable[1][4] = { PSKShapeTbl_ZP, PSKShapeTbl_ZP, PHZ_0 };	//present PHZ_OFF
	// SYMBOL = 2 = SYM_P180 = Advance 180 degrees
	//   I ramp shape     Q ramp shape     Next Phase
	PSKPhaseLookupTable[2][0] = { PSKShapeTbl_PM, PSKShapeTbl_PM, PHZ_180 };//present PHZ_0
	PSKPhaseLookupTable[2][1] = { PSKShapeTbl_MP, PSKShapeTbl_PM, PHZ_270 };//present PHZ_90
	PSKPhaseLookupTable[2][2] = { PSKShapeTbl_MP, PSKShapeTbl_MP, PHZ_0 };	//present PHZ_180
	PSKPhaseLookupTable[2][3] = { PSKShapeTbl_PM, PSKShapeTbl_MP, PHZ_90 };	//present PHZ_270
	PSKPhaseLookupTable[2][4] = { PSKShapeTbl_ZP, PSKShapeTbl_ZP, PHZ_0 };	//present PHZ_OFF
	// SYMBOL = 3 = SYM_M90	= retard 90 degrees
	//   I ramp shape     Q ramp shape     Next Phase
	PSKPhaseLookupTable[3][0] = { PSKShapeTbl_P, PSKShapeTbl_PM, PHZ_270 };	//present PHZ_0
	PSKPhaseLookupTable[3][1] = { PSKShapeTbl_MP, PSKShapeTbl_P, PHZ_0 };	//present PHZ_90
	PSKPhaseLookupTable[3][2] = { PSKShapeTbl_M, PSKShapeTbl_MP, PHZ_90 };	//present PHZ_180
	PSKPhaseLookupTable[3][3] = { PSKShapeTbl_PM, PSKShapeTbl_M, PHZ_180 };	//present PHZ_270
	PSKPhaseLookupTable[3][4] = { PSKShapeTbl_ZP, PSKShapeTbl_ZP, PHZ_0 };	//present PHZ_OFF
	// SYMBOL = 4 = SYM_OFF
	//   I ramp shape     Q ramp shape     Next Phase
	PSKPhaseLookupTable[4][0] = { PSKShapeTbl_PZ, PSKShapeTbl_PZ, PHZ_OFF };//present PHZ_0
	PSKPhaseLookupTable[4][1] = { PSKShapeTbl_MZ, PSKShapeTbl_PZ, PHZ_OFF };//present PHZ_90
	PSKPhaseLookupTable[4][2] = { PSKShapeTbl_MZ, PSKShapeTbl_MZ, PHZ_OFF };//present PHZ_180
	PSKPhaseLookupTable[4][3] = { PSKShapeTbl_PZ, PSKShapeTbl_MZ, PHZ_OFF };//present PHZ_270
	PSKPhaseLookupTable[4][4] = { PSKShapeTbl_Z, PSKShapeTbl_Z, PHZ_OFF };	//present PHZ_OFF
	// SYMBOL = 5 = SYM_ON	
	//   I ramp shape     Q ramp shape     Next Phase
	PSKPhaseLookupTable[5][0] = { PSKShapeTbl_P, PSKShapeTbl_P, PHZ_0 };	//present PHZ_0
	PSKPhaseLookupTable[5][1] = { PSKShapeTbl_MP, PSKShapeTbl_P, PHZ_0 };	//present PHZ_90
	PSKPhaseLookupTable[5][2] = { PSKShapeTbl_MP, PSKShapeTbl_MP, PHZ_0 };	//present PHZ_180
	PSKPhaseLookupTable[5][3] = { PSKShapeTbl_P, PSKShapeTbl_MP, PHZ_0 };	//present PHZ_270
	PSKPhaseLookupTable[5][4] = { PSKShapeTbl_ZP, PSKShapeTbl_ZP, PHZ_0 };	//present PHZ_OFF

}

///////////++++++++++++++++++++++++++++++++++////////////////
///////////   P S K 3 1   M O D U L A T O R  ////////////////
///////////++++++++++++++++++++++++++++++++++////////////////

/////////////////////////////////////////////////////////////
//Initialize the PSK/CW modulator with the sample frequncy Fs,
// the carrier frequency freq, and the PSK mode.
/////////////////////////////////////////////////////////////
void CPSKMod::InitPSKMod(int Fs, double MaxAmplitude)
{
	
	m_RMSConstant = MaxAmplitude;// sqrt(2.0);

	if (PSKShapeTbl_Z == NULL || Fs != m_Fs)
		CreateRampBuffers(Fs);

	m_Fs = Fs;
	m_SampleFreq = (double)m_Fs*m_ClkError;
//	m_hWnd = hWnd;
	m_PSKPhaseInc = m_2PI * m_TxFreq/m_SampleFreq;		//carrier frequency
	m_PSKSecPerSamp = 1.0/m_SampleFreq;
	m_PSKTime = 0.0;
	m_t = 0.0;
	m_Lastsymb = SYM_OFF;
	m_AddEndingZero = 1;
	m_CWState = 0;
	m_CWtimer = 0;
	SetTXMode(m_PSKmode);
	for(int i=0; i<16; i++)
		m_IQPhaseArray[i] = 1;
	m_VectLookup[SYM_NOCHANGE][0] = 0;
	m_VectLookup[SYM_NOCHANGE][1] = 1000;
	m_VectLookup[SYM_P90][0] = 1000;
	m_VectLookup[SYM_P90][1] = 0;
	m_VectLookup[SYM_P180][0] = 0;
	m_VectLookup[SYM_P180][1] = -1000;
	m_VectLookup[SYM_M90][0] = -1000;
	m_VectLookup[SYM_M90][1] = 0;
	m_VectLookup[SYM_OFF][0] = 0;
	m_VectLookup[SYM_OFF][1] = 0;
	m_VectLookup[SYM_ON][0] = 0;
	m_VectLookup[SYM_ON][1] = 1000;
	int i = 0;
	while(i<32)		//create post/preamble tables
	{
		m_Preamble[i] = TXTOG_CODE;
		m_Postamble[i++] = TXON_CODE;
	}
	m_Preamble[i] = 0;		// null terminate these tables
	m_Postamble[i] = 0;
	
	m_pPSKtxI = PSKShapeTbl_Z;
	m_pPSKtxQ = PSKShapeTbl_Z;
	m_PresentPhase = PHZ_OFF;
	m_TxShiftReg = 0;
	m_TxCodeWord = 0;

	}

//////////////////////////////////////////////////////////////////////
// Called to set the transmit mode
//////////////////////////////////////////////////////////////////////
void CPSKMod::SetTXMode(int mode)
{
	m_PSKmode = mode;
	m_Ramp = 0;
	if(m_PSKmode&(1<<3) )
	{
		m_SymbolRate = SYMBOL_RATE63;
		m_CWIDSpeed = m_CWSetSpeed*2;
	}
	else       //AA6YQ 1.21
		if(m_PSKmode&(1<<4) )
		{
			m_SymbolRate = SYMBOL_RATE125;
			m_CWIDSpeed = m_CWSetSpeed*4;
		}
		else
		{
			m_SymbolRate = SYMBOL_RATE31;
			m_CWIDSpeed = m_CWSetSpeed;
		}

	m_PSKPeriodUpdate = 1.0/m_SymbolRate;	//symbol period
	switch(m_PSKmode&0x07)
	{
		case BPSK_MODE:
		case QPSKU_MODE:
		case QPSKL_MODE:
			m_TXState = TX_PREAMBLE_STATE;
			break;
		case TUNE_MODE:
			m_TXState = TX_TUNE_STATE;
			break;
		case TUNE_MODE_WID:
			m_TXState = TX_TUNE_STATE;
			m_NeedCWid = 1;
			break;
		case CW_MODE:
			m_TXState = TX_PREAMBLE_STATE;
			break;
		default:
			m_TXState = TX_PREAMBLE_STATE;
			m_PSKmode = BPSK_MODE;
			break;
	}
// Generate cosine ramp envelope lookup tables
//	int RampSize =  (((100*(int)m_SampleFreq)/(int)(m_SymbolRate*100))+0); //  number of envelope ramp steps per symbol
	int RampSize =  (int)(m_SampleFreq/m_SymbolRate); //  number of envelope ramp steps per symbol
	for( int i=0; i<m_rampsize; i++)
	{
		PSKShapeTbl_Z[i] = 0.0;
		PSKShapeTbl_P[i] = 1.0;
		PSKShapeTbl_M[i] = -1.0;
		PSKShapeTbl_PM[i] = cos( (double)i*m_2PI/(RampSize*2) );
		PSKShapeTbl_MP[i] = -PSKShapeTbl_PM[i];

		if( i <RampSize/2 )
		{
			PSKShapeTbl_PZ[i] = cos( (double)i*m_2PI/(RampSize*2) );
			PSKShapeTbl_MZ[i] = -PSKShapeTbl_PZ[i];
			PSKShapeTbl_ZP[i] = 0.0;
		}
		else
		{
			PSKShapeTbl_ZP[i] = -cos( (double)i*m_2PI/(RampSize*2) );
			PSKShapeTbl_PZ[i] = 0.0;
			PSKShapeTbl_MZ[i] = 0.0;
		}

	}
}

//////////////////////////////////////////////////////////////////////
// Called to get the symbol vectors for display use
//////////////////////////////////////////////////////////////////////
void CPSKMod::GetVectorData(long *VectData)
{
	for(int i=0; i<16; i++)
		VectData[i] = m_IQPhaseArray[i];
}

/////////////////////////////////////////////////////////////
// generates n samples of psk31 waveform in data array pData
/////////////////////////////////////////////////////////////
void CPSKMod::CalcPSK(double *pData, int n, int stride)
{
int symbol;
int i;
int v = 0;
	//m_RMSConstant = TX_CONSTANT;
	for( i=0; i<n; i+=2 )		//calculate n samples of tx data stream
	{
		m_t += m_PSKPhaseInc;			// increment radian phase count
// create sample from sin/cos and shape tables
		pData[i * stride] = m_RMSConstant*( m_pPSKtxI[m_Ramp]*sin( m_t ));
		pData[i * stride + 1] = m_RMSConstant * ( m_pPSKtxQ[m_Ramp++] * cos(m_t));

		m_PSKTime += m_PSKSecPerSamp;
		if( m_PSKTime >= m_PSKPeriodUpdate )//if time to update symbol
		{
			m_PSKTime -= m_PSKPeriodUpdate;	//keep time bounded
			m_Ramp = 0;						// time to update symbol
			m_t = fmod(m_t,m_2PI);			//keep radian counter bounded
			switch( m_PSKmode&0x07 )				//get next symbol to send
			{
				case CW_MODE:
					symbol = GetNextCWSymbol();
					break;
				case BPSK_MODE:
					symbol = GetNextBPSKSymbol();
					break;
				case QPSKU_MODE:
					symbol = GetNextQPSKSymbol();
					break;
				case QPSKL_MODE:
					symbol = GetNextQPSKSymbol();
					if(symbol==SYM_P90)		//rotate vectors the opposite way
						symbol = SYM_M90;
					else
						if(symbol==SYM_M90)
						symbol = SYM_P90;
					break;
				case TUNE_MODE:
				case TUNE_MODE_WID:
					symbol = GetNextTuneSymbol();
					break;
			}
			//get new I/Q ramp tables and next phase
			m_pPSKtxI = PSKPhaseLookupTable[symbol][m_PresentPhase].iptr;
			m_pPSKtxQ = PSKPhaseLookupTable[symbol][m_PresentPhase].qptr;
			m_PresentPhase = PSKPhaseLookupTable[symbol][m_PresentPhase].next;
			m_IQPhaseArray[v++] = m_VectLookup[m_PresentPhase][0];
			m_IQPhaseArray[v++] = m_VectLookup[m_PresentPhase][1];
			v = v & 0x000F;	//keep bounded to 16
		}
	}
}
/*
void CPSKMod::CalcPSK(double* pData, int n, int stride)
{
	int symbol;
	int i;
	int v = 0;
	//m_RMSConstant = TX_CONSTANT;
	for (i = 0; i < n; i++)		//calculate n samples of tx data stream
	{
		m_t += m_PSKPhaseInc;			// increment radian phase count
// create sample from sin/cos and shape tables
		pData[i * stride] = m_RMSConstant * (m_pPSKtxI[m_Ramp] * sin(m_t) + m_pPSKtxQ[m_Ramp++] * cos(m_t));

		m_PSKTime += m_PSKSecPerSamp;
		if (m_PSKTime >= m_PSKPeriodUpdate)//if time to update symbol
		{
			m_PSKTime -= m_PSKPeriodUpdate;	//keep time bounded
			m_Ramp = 0;						// time to update symbol
			m_t = fmod(m_t, m_2PI);			//keep radian counter bounded
			switch (m_PSKmode & 0x07)				//get next symbol to send
			{
			case CW_MODE:
				symbol = GetNextCWSymbol();
				break;
			case BPSK_MODE:
				symbol = GetNextBPSKSymbol();
				break;
			case QPSKU_MODE:
				symbol = GetNextQPSKSymbol();
				break;
			case QPSKL_MODE:
				symbol = GetNextQPSKSymbol();
				if (symbol == SYM_P90)		//rotate vectors the opposite way
					symbol = SYM_M90;
				else
					if (symbol == SYM_M90)
						symbol = SYM_P90;
				break;
			case TUNE_MODE:
			case TUNE_MODE_WID:
				symbol = GetNextTuneSymbol();
				break;
			}
			//get new I/Q ramp tables and next phase
			m_pPSKtxI = PSKPhaseLookupTable[symbol][m_PresentPhase].iptr;
			m_pPSKtxQ = PSKPhaseLookupTable[symbol][m_PresentPhase].qptr;
			m_PresentPhase = PSKPhaseLookupTable[symbol][m_PresentPhase].next;
			m_IQPhaseArray[v++] = m_VectLookup[m_PresentPhase][0];
			m_IQPhaseArray[v++] = m_VectLookup[m_PresentPhase][1];
			v = v & 0x000F;	//keep bounded to 16
		}
	}
}
*/

/////////////////////////////////////////////////////////////
// called every symbol time to get next CW symbol and get the
// next character from the character Queue if no more symbols
// are left to send.
/////////////////////////////////////////////////////////////
unsigned char CPSKMod::GetNextCWSymbol(void)
{
unsigned char symb;
int ch;
	symb = m_Lastsymb;		//use last symbol unless it needs to change
	if( (m_TxShiftReg == 0 ) && (m_CWState == 0) )
	{
		ch = GetChar();			//get next character to xmit
		if( ch >=0 )			//if is not a control code
		{
			ch &= 0xFF;		
			ch = (int)toupper( (unsigned char)ch );	//make upper case
			if( ch>=' ' && ch<='Z')
				m_TxShiftReg = CW_TABLE[ ch-' '];	//look up pattern
		}
		else					// is a control code
		{
			if( ch == TXON_CODE )
				symb = SYM_ON;
			else
				symb = SYM_OFF;
			return symb;
		}
		m_CWState = 0;
	}
	switch( m_CWState )		// CW timing state machine
	{
		case 0:			//get next cw symbol state from pattern
			switch( m_TxShiftReg&0xC000 )
			{
				case 0x4000:	//dot
					m_CWState = 1;
					m_CWtimer = 1*m_CWIDSpeed;
					symb = SYM_ON;
					break;
				case 0x8000:	//dash
					m_CWState = 1;
					m_CWtimer = 3*m_CWIDSpeed;
					symb = SYM_ON;
					break;
				case 0xC000:	//inter char space 
					m_CWState = 2;
					m_CWtimer = 2*m_CWIDSpeed-1;
					symb = SYM_OFF;
					break;
				default:
					symb = SYM_OFF;
					break;
			}
			m_TxShiftReg = m_TxShiftReg<<2;	//
			break;
		case 1:		//On time state
			if( --m_CWtimer <= 0 )
			{
				m_CWtimer = 1*m_CWIDSpeed - 1;	//inter symbol time
				if(m_CWtimer>=1)
					m_CWState = 2;
				else
					m_CWState = 0;
				symb = SYM_OFF;
			}
			else
				symb = SYM_NOCHANGE;
			break;
		case 2:		//Off time state
			if( --m_CWtimer <= 0 )
				m_CWState = 0;
			break;
	}
	m_Lastsymb = symb;
	return symb;
}

/////////////////////////////////////////////////////////////
// called every symbol time to get next BPSK symbol and get the
// next character from the character Queue if no more symbols
// are left to send.
/////////////////////////////////////////////////////////////
unsigned char CPSKMod::GetNextBPSKSymbol(void)
{
unsigned char symb;
int ch;
	symb = m_Lastsymb;
	if( m_TxShiftReg == 0 )
	{
		if( m_AddEndingZero )		// if is end of code
		{
			symb = SYM_P180;		// end with a zero
			m_AddEndingZero = 0;
		}
		else
		{
			ch = GetChar();			//get next character to xmit
			if( ch >=0 )			//if is not a control code
			{						//get next VARICODE codeword to send
				m_TxShiftReg = VARICODE_TABLE[ ch&0xFF ];
				symb = SYM_P180;	//Start with a zero
			}
			else					// is a control code
			{
				switch( ch )
				{
				case TXON_CODE:
					symb = SYM_ON;
					break;
				case TXTOG_CODE:
					symb = SYM_P180;
					break;
				case TXOFF_CODE:
					symb = SYM_OFF;
					break;
				}
			}
		}
	}
	else			// is not end of code word so send next bit
	{
		if( m_TxShiftReg&0x8000 )
			symb = SYM_NOCHANGE;
		else
			symb = SYM_P180;
		m_TxShiftReg = m_TxShiftReg<<1;	//point to next bit
		if( m_TxShiftReg == 0 )			// if at end of codeword
			m_AddEndingZero = 1;		// need to send a zero nextime
	}
	m_Lastsymb = symb;
	return symb;
}

		
/////////////////////////////////////////////////////////////
// called every symbol time to get next QPSK symbol and get the
// next character from the character Queue if no more symbols
// are left to send.
/////////////////////////////////////////////////////////////
unsigned char CPSKMod::GetNextQPSKSymbol(void)
{
unsigned char symb;
int ch;
	symb = ConvolutionCodeTable[m_TxShiftReg&0x1F];	//get next convolution code
	m_TxShiftReg = m_TxShiftReg<<1;
	if( m_TxCodeWord == 0 )			//need to get next codeword
	{
		if( m_AddEndingZero )		//if need to add a zero
		{
			m_AddEndingZero = 0;	//end with a zero
		}
		else
		{
			ch = GetChar();			//get next character to xmit
			if( ch >=0 )			//if not a control code
			{						//get next VARICODE codeword to send
				m_TxCodeWord = VARICODE_TABLE[ ch&0xFF ];
			}
			else					//is a control code
			{
				switch( ch )
				{
				case TXON_CODE:
					symb = SYM_ON;
					break;
				case TXTOG_CODE:
					m_TxCodeWord = 0;
					break;
				case TXOFF_CODE:
					symb = SYM_OFF;
					break;
				}
			}
		}
	}
	else
	{
		if(m_TxCodeWord&0x8000 )
		{
			m_TxShiftReg |= 1;
		}
		m_TxCodeWord = m_TxCodeWord<<1;
		if(m_TxCodeWord == 0)
			m_AddEndingZero = 1;	//need to add another zero
	}
	return symb;
}

/////////////////////////////////////////////////////////////
// called every symbol time to get next Tune symbol
/////////////////////////////////////////////////////////////
unsigned char CPSKMod::GetNextTuneSymbol(void)
{
unsigned char symb;
int ch;
	ch = GetChar();			//get next character to xmit
	switch( ch )
	{
		case TXON_CODE:
			symb = SYM_ON;
			break;
		default:
			symb = SYM_OFF;
			break;
	}
	return symb;
}

/////////////////////////////////////////////////////////////
//get next character/symbol depending on TX state. 
/////////////////////////////////////////////////////////////
int CPSKMod::GetChar()
{
int ch;
// static test = '0';  //AA6YQ - not referenced
	switch( m_TXState )
	{
		case TX_OFF_STATE:		//is receiving
			ch = TXOFF_CODE;		//else turn off
			m_NeedShutoff = 0;
			break;
		case TX_TUNE_STATE:
			ch = TXON_CODE;				// steady carrier
			if(	m_NeedShutoff)
			{
				if(	m_NeedCWid  )
				{
					m_TXState = TX_CWID_STATE;
					m_SavedMode = m_PSKmode;
					m_PSKmode = CW_MODE;
					m_NeedCWid = 0;
					m_AmblePtr = 0;
					ch = TXOFF_CODE;
				}
				else
				{
					m_TXState = TX_OFF_STATE;
					m_AmblePtr = 0;
					ch = TXOFF_CODE;
					m_NeedShutoff = 0;
				}
			}
			break;
		case TX_POSTAMBLE_STATE:		// ending sequence
			if( !(ch = m_Postamble[m_AmblePtr++] ) || m_NoSquelchTail)
			{
				m_NoSquelchTail = 0;
				if(	m_NeedCWid  )
				{
					m_TXState = TX_CWID_STATE;
					m_SavedMode = m_PSKmode;
					m_PSKmode = CW_MODE;
					m_NeedCWid = 0;
					m_AmblePtr = 0;
					ch = TXOFF_CODE;
				}
				else
				{
					m_TXState = TX_OFF_STATE;
					m_AmblePtr = 0;
					ch = TXOFF_CODE;
					m_NeedShutoff = 0;
				}
			}
			break;
		case TX_PREAMBLE_STATE:			//starting sequence
			if( !(ch = m_Preamble[m_AmblePtr++] ))
			{
				m_TXState = TX_SENDING_STATE;
				m_AmblePtr = 0;
				ch = TXTOG_CODE;
			}
			break;
/*		case TX_CWID_STATE:				// id sendign CW ID
			if( m_AmblePtr >= m_CWIdString.GetLength() )
			{
				m_PSKmode = m_SavedMode;
				m_TXState = TX_OFF_STATE;
				m_AmblePtr = 0;
				ch = TXOFF_CODE;
				m_NeedShutoff = 0;
			}
			else
			{
				m_PSKmode = CW_MODE;
				ch = (int)m_CWIdString.GetAt(m_AmblePtr++);
			}
			break;*/
		case TX_SENDING_STATE:		//if sending text from TX window
			ch = GetTxChar();
//ch = 'e';
//ch = rand()&0x7F;
//ch = test++;
//if(test>'z')
//test = '0';
			if(	(ch == TXTOG_CODE) && m_NeedShutoff)
			{
				m_TXState = TX_POSTAMBLE_STATE;
			}
			else
			{
				if (ch > 0)
				{
					//	::PostMessage(m_hWnd, MSG_PSKCHARRDY,ch,-1);
				}
			}
			m_AmblePtr = 0;
			break;

	}
	return( ch );
}


////////////////////////////////////////////////////////////////////////////
//		TX Queueing routines
/////////////////////////////////////////////////////////////////////////////
int CPSKMod::PutTxQue(int txchar, int cntrl)
{
//	EnterCriticalSection(&m_CriticalSection);
	if( cntrl )	//is a tx control code
	{
		switch( txchar )
		{
			case TX_CNTRL_AUTOSTOP:
				m_TempNeedShutoff = 1;
				if( m_TXState==TX_TUNE_STATE )
					m_NeedShutoff = 1;
				break;
			case TX_CNTRL_ADDCWID:
				m_TempNeedCWid = 1;
				break;
			case TX_CNTRL_NOSQTAIL:
				m_TempNoSquelchTail = 1;
				break;
		}
	}
	else		//is a character to xmit
	{
		if( (txchar != BACK_SPACE_CODE) || (m_pHead==m_pTail) )
		{
			if (((m_pHead + 1) % TX_BUF_SIZE) == m_pTail) {
				return -1;
			}
			m_pXmitQue[m_pHead++] = txchar;	
			if( m_pHead >= TX_BUF_SIZE )
				m_pHead = 0;
		}
		else	//see if is a backspace and if can delete it in the queue
		{
			if(--m_pHead < 0 )		//look at last character in queue
				m_pHead = 0;
			if( m_pXmitQue[m_pHead] == BACK_SPACE_CODE)
			{								//if another backspace, leave it there
				if(++m_pHead >= TX_BUF_SIZE )
					m_pHead = 0;
				m_pXmitQue[m_pHead++] = txchar;
				if( m_pHead >= TX_BUF_SIZE )
					m_pHead = 0;
			}
		}
	}
//	LeaveCriticalSection(&m_CriticalSection);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CPSKMod::GetTxChar()
{
	//get next character from app callback


int ch;
//	EnterCriticalSection(&m_CriticalSection);
	if (getNextChar != nullptr)
	{
		ch = getNextChar(callbackContext);
		//if(ch<0)ch= TXTOG_CODE;
		
	}
	else
	{
		if (m_pHead != m_pTail)	//if something in Queue
		{
			ch = m_pXmitQue[m_pTail++] & 0x00FF;
			if (m_pTail >= TX_BUF_SIZE)
				m_pTail = 0;
		}
		else
			ch = TXTOG_CODE;		// if que is empty return TXTOG_CODE
	}
	if(m_TempNeedShutoff)
	{
		m_TempNeedShutoff = 0;
		m_NeedShutoff = 1;
	}
	if(m_TempNeedCWid)
	{
		m_TempNeedCWid = 0;
		m_NeedCWid = 1;
	}
	if(m_TempNoSquelchTail)
	{
		m_TempNoSquelchTail = 0;
		m_NoSquelchTail = 1;
	}
//	LeaveCriticalSection(&m_CriticalSection);
	return ch;
}

/////////////////////////////////////////////////////////////////////////////
void CPSKMod::ClrQue()
{
//	EnterCriticalSection(&m_CriticalSection);
	m_pTail = m_pHead = 0;
//	LeaveCriticalSection(&m_CriticalSection);
	m_NoSquelchTail = 0;
	m_TempNeedCWid = 0;
	m_TempNeedShutoff = 0;
	m_TempNoSquelchTail = 0;
}


int CPSKMod::GetTXCharsRemaining()
{
//	EnterCriticalSection(&m_CriticalSection);
	int num = m_pHead - m_pTail;
//	LeaveCriticalSection(&m_CriticalSection);
	if( num < 0 )
		num = num + TX_BUF_SIZE;
	return num;
}

void CPSKMod::SetCWIDSpeed(long speed)
{
	if( (speed>=1) && (speed<=4) )
		m_CWSetSpeed = speed;
	else
		m_CWSetSpeed = CW_SPEED;
}


