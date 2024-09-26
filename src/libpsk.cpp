#include <stdlib.h>

#include "libpsk.h"
#include "pskcoresdr.h"
#include "pskdet.h"

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
    return runPSKDet(det, new_buf, samples, 1, res, res_len);
}

}
