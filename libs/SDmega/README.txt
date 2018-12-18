
** SD - a slightly more friendly wrapper for sdfatlib **

This library aims to expose a subset of SD card functionality in the
form of a higher level "wrapper" object.

License: GNU General Public License V3
         (Because sdfatlib is licensed with this.)

(C) Copyright 2010 SparkFun Electronics

Now better than ever with optimization, multiple file support, directory handling, etc - ladyada!


The following changes have been made to work with an Arduino Mega

1. comment out #define USE_SPI_LIB from both Sd2Card.h and Sd2Card.cpp. 
2. Modify Sd2Card.h to enable the softSPI for Mega 2560 => #define MEGA_SOFT_SPI 1  //change it to non-zero from 0