// draw a clock on the screen
// (C) 2020 Kristian Wiklund
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//

#include "config.h"
#include <Math.h>

void clockscreen() {

  DateTime now = rtc.now();

  // this clears the screen in the caller
  // tft.fillRect(10,60,310,180,BLACK); 
  // which means that 10,60,10+310,60+180 is the area where we can play

  // draw a large circle with a small circle inside
  tft.drawCircle(CLOCK_X,CLOCK_Y,CLOCK_R, WHITE);
  tft.fillCircle(CLOCK_X, CLOCK_Y, CLOCK_CR, WHITE);

  // draw hour marks  
  // replace the trig calls with precalculated values for more efficiency, if needed

  for(int i=0;i<12;i++) {
    int x1,y1,x2,y2;
    
    x1 = sin(i*2*PI/12)*CLOCK_R+CLOCK_X;
    x2 = sin(i*2*PI/12)*(CLOCK_R-CLOCK_H_LEN)+CLOCK_X;
    y1 = cos(i*2*PI/12)*CLOCK_R+CLOCK_Y;
    y2 = cos(i*2*PI/12)*(CLOCK_R-CLOCK_H_LEN)+CLOCK_Y;

    tft.drawLine(x1,y1,x2,y2,WHITE);
  }

}
