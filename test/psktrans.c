/**
 * \file pskrecv.c
 * 
 * \brief Pulseaudio PSK receiver
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
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <pulse/simple.h>
#include <pulse/error.h>

#include "libpsk.h"

#define SAMPLES 1000

static int send_line(struct pa_simple * s, char * line);

int main(int argc, char ** argv) {
    static pa_sample_spec ss;
    bzero(&ss, sizeof(ss));
    ss.format = PA_SAMPLE_S16LE;
    ss.rate = 8000;
    ss.channels = 1;
    
    pa_simple * s = NULL;
    int pa_err;
    
    int ret = 0;
    
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    
    s = pa_simple_new(NULL, "psktrans", PA_STREAM_PLAYBACK, NULL, 
                      "psktrans output stream", &ss, NULL, NULL, &pa_err);
    if (!s) {
            fprintf(stderr, "pa_simple_new(): %s\n", pa_strerror(pa_err));
            ret = 1;
            goto fail;
    }
    
    puts("PSK Transmit Console. Will automatically transmit line-by-line.");
    puts("");
    
    char * line = NULL;
    size_t len = 0;
    for (;;) {
        printf("[psk31] > ");
        getline(&line, &len, stdin);
        printf("[transmitting...");
        if (send_line(s, line)) {
            ret = 1;
            goto fail;
        }
        printf("done]\n");
    }

    
fail:
    
    if (s) {
        pa_simple_free(s);
    }
        
    return ret;
}

static int send_line(struct pa_simple * s, char * line) {
    int ret = 0;
    int pa_err;
    PSK_MOD mod = psk_m_create(8000, 1000, PSK_MODE_PSK31, 0);
    psk_m_putchar(mod, '\n');
    psk_m_putchar(mod, '\n');
    psk_m_puts(mod, line);
    psk_m_putchar(mod, '\n');
    
    for (unsigned int i = 0;; i++) {
        int16_t buf[SAMPLES];
        psk_m_run(mod, buf, SAMPLES, 1);
        if (pa_simple_write(s, buf, sizeof(buf), &pa_err) < 0) {
            fprintf(stderr, "pa_simple_write(): %s\n", pa_strerror(pa_err));
            ret = 1;
            goto fail;
        }
        
        /* Over ~125000 prevents real-time and latency is unbounded */
        int delay = 100000;
        //usleep(delay);
        
        if (i % 40 == 0) {
            pa_usec_t latency;
            latency = pa_simple_get_latency(s, &pa_err);
            if (latency == (pa_usec_t) -1) {
                fprintf(stderr, "pa_simple_get_latency(): %s\n", pa_strerror(pa_err));
                ret = 1;
                goto fail;
            }
            
            /* Falling behind real-time (>5s latency) */
            if (latency > 5000000) {
                printf("\nLATENCY: %lf ", latency / 1000000.);
            }
        }
        /* Time to run-out then finish */
        if (!psk_m_active(mod)) {
            break;
        }
    }
    
    for (int i = 0; i < 8; i++) {
        int16_t buf[SAMPLES];
        psk_m_run(mod, buf, SAMPLES, 1);
        if (pa_simple_write(s, buf, sizeof(buf), &pa_err) < 0) {
            fprintf(stderr, "pa_simple_write(): %s\n", pa_strerror(pa_err));
            ret = 1;
            goto fail;
        }
    }
        

fail:
    if (mod) {
        psk_m_free(mod);
    }
    return ret;
}
