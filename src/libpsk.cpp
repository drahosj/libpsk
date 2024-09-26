#include <stdlib.h>
#include <string.h>

#include "libpsk.h"
#include "pskcoresdr.h"
#include "pskdet.h"
#include "pskmod.h"

extern "C" {
    
void psk_d_set_squelch(PSK_DET det, int squelch_thresh, int squelch_speed) {
	((CPSKDet *) det)->SetSquelchThresh(squelch_thresh, squelch_speed);
}    
    
PSK_DET psk_d_create(int fs, 
			 int frequency,
             int mode,
			 int squelch_thresh, 
             int squelch_speed,
			 int afc_limit) {
    void * det = initPSKDet(fs);
    SetRXFrequency(det, frequency);
    /* Hack, better to set PSK_AFC_DEF to the underlying default value 
     * and always call this */
    if (afc_limit != PSK_AFC_DEF) {
        SetAFCLimit(det, afc_limit);
    }
    
	psk_d_set_squelch(det, squelch_thresh, squelch_speed);
    return (PSK_DET) det;
}

int psk_d_run(PSK_DET det, int16_t * in_buf, int samples, int stride, char * res, int res_len) {
    double * new_buf = (double *) malloc(sizeof(double) * samples);
    for (int i = 0; i < samples; i++) {
        new_buf[i] = (double) (in_buf[i * stride]);
        
    }
    int ret = runPSKDet(det, new_buf, samples, 1, res, res_len);
    free(new_buf);
    return ret;
}

#define TX_CNTRL_AUTOSTOP 1
#define TX_CNTRL_ADDCWID 2
#define TX_CNTRL_NOSQTAIL 3

PSK_MOD psk_m_create(int fs, int frequency, int mode, int flags) {
    void * mod = createPSKMod(fs, 1.0);
    
    ((CPSKMod *) mod)->SetTXFreq(frequency);
    ((CPSKMod *) mod)->SetTXMode(mode);
    
    if (flags & PSK_MOD_AUTOSTOP) {
        ((CPSKMod *) mod)->PutTxQue(0, TX_CNTRL_AUTOSTOP);
    }
    
    if (flags & PSK_MOD_ADDCWID) {
        ((CPSKMod *) mod)->PutTxQue(0, TX_CNTRL_ADDCWID);
    }
    
    if (flags & PSK_MOD_NOSQLTAIL) {
        ((CPSKMod *) mod)->PutTxQue(0, TX_CNTRL_NOSQTAIL);
    }
    
    return (PSK_MOD) mod;
}

int psk_m_putchar(PSK_MOD mod, int c) {
    return ((CPSKMod *) mod)->PutTxQue(c, 0);
}

int psk_m_puts(PSK_MOD mod, char * s) {
    size_t i;
    for (i = 0; i < strlen(s); i++) {
        if (psk_m_putchar(mod, s[i]) < 0) {
            break;
        }
    }
    return i;
}

void psk_m_run(PSK_MOD mod, int16_t * buf, int samples, int stride) {
    /* Hack: underlying stride is whack for some reason; has a stride of 2...*/
    double * new_buf = (double *) malloc(sizeof(double) * samples * 2);
    CalcPSK(mod, new_buf, samples, 1);
    for (int i = 0; i < samples; i++) {
        buf[i * stride] = (int16_t) (new_buf[i * 2] * 30000.);
    }
    free(new_buf);
    return;
}

void psk_m_free(PSK_MOD mod) {
    freePSKMod(mod);
}

}
