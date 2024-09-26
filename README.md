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

`make` in the root of the directory to build libpsk.

Test programs are included (psktrans/pskrecv). To build these, run
`make test` or `make` from the `test/` directory.

To execute the test programs, `cd` to `test/` and run `./pskrecv` to start
monitoring and `./psktrans` to get a transmit console. PTT is not supported
and center frequency is fixed at 1000 Hz.

These are compatible with FLDIGI if using audio loopback. Audio loopback works
virtually or physically (mic near speaker). FLDIGI mode BPSK31, place waterfall
center marker at 1000 Hz.

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
