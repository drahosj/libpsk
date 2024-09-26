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

void runRx(void * det, int sec)
{
    for(auto i = 0; i < (8 * sec); i++) {
        byte[2000] _ibuf;
        auto ibuf = stdin.rawRead(_ibuf);
        assert(ibuf.length % 2 == 0, "Odd number of samples");
        double[1000] _data;
        auto data = _data[0..ibuf.length / 2];
        for(auto j = 0; j < ibuf.length; j += 2) {
            auto s = cast(short) ((ibuf[j] & 0xff) | (ibuf[j + 1] << 8));
            data[j / 2] = cast(double) s;
        }
        char[1000] _res;
        auto res = _res[0..data.length];
        auto len = det.runPSKDet(data.ptr, cast(int) data.length, 1, 
                res.ptr, cast(int) res.length);
        write(res[0..len]);
        stdout.flush();
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
    if (argc > 1 && strcmp(argv[1], "tx")) {
        /* IGNORE - Old code 
        void * mod = createPSKMod(8000, 1.0);
        mod.SetTXFrequency(800);

        mod.runTx(2);

        foreach(c; "Test PSK Message\n"){
            mod.PutTxQue(c);
        }

        mod.runTx(8);
        */
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
        
    }

    return 0;
}
