/******************************************************************************
  EarthListener by Pieter Sijmons @ Velleman NV
  Based on examples by Adafruit, SparkFun, J. Steinlage and Tom Igoe

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE 
  OR OTHER DEALINGS IN THE SOFTWARE.

  Special thanks to Marie-Suzanne for keeping me fed & sane while coding/debugging.
  Thanks to Brecht & Philip for their help and guidance.
  -------------------------------
  BasicReadings.ino
  by Nathan Seidle @ SparkFun Electronics  March 9th, 2018
  https://www.sparkfun.com/products/14348

  Get basic environmental readings from the BME280
  This example shows how to read humidity, pressure, and current temperature from the BME280 over I2C.

  Hardware connections:
  BME280 -> Arduino
  GND -> GND
  3.3 -> 3.3
  SDA -> A4 (UNO)
  SCL -> A5 (UNO)
  
  -------------------------------
  BasicReadings.ino
  by Marshall Taylor & Nathan Seidle @ SparkFun Electronics   April 4, 2017
  https://github.com/sparkfun/CCS811_Air_Quality_Breakout
  https://github.com/sparkfun/SparkFun_CCS811_Arduino_Library

  Read the TVOC and CO2 values from the SparkFun CSS811 breakout board
  A new sensor requires at 48-burn in. Once burned in a sensor requires
  20 minutes of run in before readings are considered good.

  Hardware Connections (Breakoutboard to Arduino):
  3.3V to 3.3V pin
  GND to GND pin
  SDA to A4 (UNO)
  SCL to A5 (UNO)
  
  -------------------------------
  Graphicstest.ino
  by Adafruit
  https://github.com/adafruit/Adafruit-GFX-Library
  
  // IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
  // CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
  // SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.
  -> we use shield, so modify the library!!!

  -------------------------------
  LightiningI2C.ino
  by J. Steinlage   2015 Jul 20
  https://github.com/PlayingWithFusion/PWFusion_AS3935_I2C/blob/master/Examples/as3935_lightning_i2c_nocal/as3935_lightning_i2c_nocal.ino
  
  APPLICATION SPECIFIC NOTES (READ THIS!!!):
  - This file configures then runs a program on an Arduino to interface with
     an AS3935 Franklin Lightning Sensor manufactured by AMS.
      - Configure Arduino
      - Perform setup for AS3935 chip
        --- capacitance registers for tuning (based on cal value provided)
        --- configurations for your application specifics (indoor/outdoor, etc)
      - read status/info from sensor
      - Write formatted information to serial port
   - Set configs for your specific needs using the #defines for wiring, and
     review the setup() function for other settings (indoor/outdoor, for example)
   - I2C specific note: This example uses the I2C interface via the I2C lib, not
     the 'Wire' lib included with the Arduino IDE.

   Circuit:
      Arduino Uno   Arduino Mega  -->  SEN-39001: AS3935 Breakout
      SDA:    SDA        SDA      -->  MOSI/SDA   (SDA is labeled on the bottom of the Arduino)
      SCLK:   SCL        SCL      -->  SCK/SCL    (SCL is labeled on the bottom of the Arduino)
      SI:     pin  9     pin 9    -->  SI (select interface; GND=SPI, VDD=I2C
      IRQ:    pin  2     pin 19    -->  IRQ
      GND:    GND        ''       -->  CS (pull CS to ground even though it's not used)
      GND:    GND        ''       -->  GND
      5V:     5V         ''       -->  Arduino I/O is at 5V, so power board from 5V. Can use 3.3V with Due, etc
  
  -------------------------------
  Speaker:  - to GND
            + to A10
            
  -------------------------------
  Optional RGB LED:       - to GND (or A11 on mega sensor shield)
                          G to A12
                          R to A13
                          B to A14

  -------------------------------           
  SD card datalogger on TFT shield on Arduino Mega
  Based on the example by Tom Igoe - 9 Apr 2012
  This example shows how to log data from three analog sensors (A11 -> A14) to an SD card using the SD library.
   
 The circuit:
 ** Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 4 (CS pin can be changed)
  and pin #52 (SS) must be an output
 => because we are using an TFT shield with SD card, some modifications needs to be done in the libary
 1. Download the SD Adafruit library from https://github.com/adafruit/SD and replace the file in your library 
 2. comment out #define USE_SPI_LIB from both Sd2Card.h and Sd2Card.cpp. 
 3. Modify Sd2Card.h to enable the softSPI for Mega 2560 by modifying this line: #define MEGA_SOFT_SPI 1  //change it to non-zero from 0     
   
 -------------------------------      
******************************************************************************/
