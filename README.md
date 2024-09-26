LibPSK
===========================================

## Introduction

A cross platfrom library for modulation and demodulation of psk31 signals.

This is a streamlined fork of 
[PSKCoreSDR](https://github.com/ahopper/PSKCoreSDR), originally based on
core components from the 
[pskcore](http://www.moetronix.com/ae4jy/pskcoredll.htm) library written by
Moe Wheatley, AE4JY.

## Improvements

- C API instead of C++ (C++ features were not relevant)
- Documentation
- Integer API (eventual transition to fixed-point; easier optimizations)
- Sane defaults

## Quickstart
`make test` and playback BPSK31 centered at 800 Hz into the default
audio input device.

Receive is supported. Build library, then cd to test and `make run`

This will receive 8s of audio and print characters. Center frequency is 800.

To generate test output, transmit from fldigi in bpsk31 centered at 800 Hz
and loopback (physical loopback works). `make run` test listens on the default
audio input device.

The detector still takes some time to stabilize, and stabilizes best on the
idle output of FLDIGI. To test, first enable transmit in FLDIGI then `make
run` the test. Once the test is running, enter text into the fldigi buffer.

## Documentation
See test/test.c for an example. `make doc` to build doxygen documentation in
doc/html.

## API
The C API is fully defined in libpsk.h. In the rest of the headers/code,
there be dragons.

## License
LibPSK is licensed under the LGPL version 2.1, as with the orignial pskcore.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
    02110-1301  USA
    
Other, non-library code is released under the MIT license. This includes 
tests which link against the library and other language bindings.
