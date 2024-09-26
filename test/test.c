/**
 * \file test.c
 * 
 * \brief Simple example test
 * 
 * \copyright
 * 
 * MIT License
 * 
 * Copyright (C) 2024 Jake Drahos <j@kedrahos.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include "libpsk.h"

/* Ignore invalid syntax in comments; ported from D */

/*
void runTx(void * mod, int sec)
{
    double[2000] obuf;
    for(auto i = 0; i < (8 * sec); i++) {
        mod.CalcPSK(obuf.ptr, obuf.length, 1);
        byte[2000] obuf2;
        for(auto j = 0; j < obuf.length; j += 2) {
            short n = cast(short)(10000 * obuf[j]);
            obuf2[j] = cast(byte) (n & 0xff);
            obuf2[j + 1] = cast(byte) ((n >> 8) & 0xff);
        }
        write(cast(string)obuf2);
    }
}

*/

/**
 * \brief Run test
 * 
 * This test currently only supports RX mode.
 * 
 */
int main(int argc, char ** argv)
{
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    if (argc > 1 && !strcmp(argv[1], "tx")) {
        PSK_MOD mod = psk_m_create(8000, 1000, PSK_MODE_PSK31, 0);
        int16_t buf[1000];
        for (int i = 0; i < 10; i++) {
            psk_m_run(mod, buf, 1000, 1);
            fwrite(buf, 1, 1000, stdout);
        }
        psk_m_puts(mod, "Hello ");
        for (int i = 0; i < 10; i++) {
            psk_m_run(mod, buf, 1000, 1);
            fwrite(buf, 1, 1000, stdout);
        }
        psk_m_puts(mod, "This is a fairly long string and probably won't all go at once");
        for (int i = 0; i < 10; i++) {
            psk_m_run(mod, buf, 1000, 1);
            fwrite(buf, 1, 1000, stdout);
        }
        psk_m_puts(mod, ". More stuff");
        for (int i = 0; i < 100; i++) {
            psk_m_run(mod, buf, 1000, 1);
            fwrite(buf, 1, 1000, stdout);
        }
        
        psk_m_free(mod);
    } else {
        /* Instantiate  detector with 8kHz sample rate,
         * 800 Hz center frequency, and sane defaults
         */
        PSK_DET det = psk_d_create(8000, 800, PSK_MODE_PSK31, 
                                   PSK_SQL_THRESH_DEF, PSK_SQL_SPEED_DEF, 
                                   PSK_AFC_DEF);

        /* Create 1000 sample buffer for input data */
        int16_t ibuf[1000];
        for (int i = 0; i < 64; i++) {
            fread(ibuf, sizeof(ibuf), 1, stdin);
            /* ibuf now contains 1000 samples of s16 native-endian sound */
            
            /* 64 characters should be more than enough */
            char obuf[64];
            
            /* Process 1000 samples */
            int r = psk_d_run(det, ibuf, 1000, 1, obuf, 64);
            
            /* Handle overrun (probably impossible to occur) */
            if (r < 0) {
                printf("XRUN");
                r = 64;
            }
            
            /* Print decoded characters */
            for(int j = 0; j < r; j++) {
                putchar(obuf[j]);
            }
        }
        
        psk_d_free(det);
        
    }

    return 0;
}
