// configuration file to hold some basic constants to avoid magic numbers - refactoring work in progress


#ifndef CONFIG_H
#define CONFIG_H

#define WITH_CLOCK

#ifdef WITH_CLOCK

// RTC on board.
// possible improvement is to autodetect and enable only if the RTC is found

#define MAXSCREEN 9 // change to 8 to disable clock display
#define INITIAL_SLIDESHOW_STATUS 1 // change to 0 to not run slideshow from start. Using the slideshow from start will show the config screens, before getting into the loop correctly.

// clock settings

#define CLOCK_X 160 // center x
#define CLOCK_Y 130 // center y
#define CLOCK_R 60  // radius
#define CLOCK_CR 2  // centre blob radius
#define CLOCK_H_LEN 4 // length of hour marks
#define CLOCK_H_HAND 0.7 // length of hour hand (as fraction of the clock radius)
#else
// no RTC on board
#define MAXSCREEN 8

#endif 
#endif
