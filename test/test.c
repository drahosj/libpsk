#include <stdio.h>
#include <string.h>
#include "libpsk.h"

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

int main(int argc, char ** argv)
{
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    if (argc > 1 && strcmp(argv[1], "tx")) {
        /*
        void * mod = createPSKMod(8000, 1.0);
        mod.SetTXFrequency(800);

        mod.runTx(2);

        foreach(c; "Test PSK Message\n"){
            mod.PutTxQue(c);
        }

        mod.runTx(8);
        */
    } else {
        PSK_DET det = psk_d_create(8000, 800, PSK_MODE_PSK31, 
                                   0, PSK_SQL_SPEED_DEF, PSK_AFC_DEF);

        int16_t ibuf[1000];
        for (int i = 0; i < 64; i++) {
            fread(ibuf, sizeof(ibuf), 1, stdin);
            char obuf[64];
            int r = psk_d_run(det, ibuf, 1000, 1, obuf, 64);
            for(int j = 0; j < r; j++) {
                putchar(obuf[j]);
            }
        }
        
    }

    return 0;
}
