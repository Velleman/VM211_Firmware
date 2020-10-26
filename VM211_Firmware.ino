/******************************************************************************
  EarthListener by Pieter Sijmons @ Velleman NV
  Based on examples by Adafruit, SparkFun and Tom Igoe
  See tab 'info_and_refs' for more documentation
******************************************************************************/
String SWversion = "v4.0";

/***************************************/
/* ---------- DECLARATIONS ----------- */
/***************************************/
/* --- Global Libraries --- */
#include <Wire.h>                               // Wire library
#include <SPI.h>                                // SPI library
#include <EEPROM.h>                             // Library to read & store info in EEPROM long term memory


/* --- Local Libraries --- */
#include "src\Adafruit-GFX-Library\Adafruit_GFX.h"     	  // Core graphics library by Adafruit
#include "src\Adafruit-GFX-Library\Fonts\FreeSans9pt7b.h" // Font FreeSans 9pts (as an alternative for basic font)
#include "src\MCUFRIEND_kbv\MCUFRIEND_kbv.h"           	  // TFT library by David Prentice
#include "src\SDmega\SDmega.h"                          	// SD library (mod by PSI to work with MEGA & TFT SD card shield)
#include "src\SparkFun_BME280\src\SparkFunBME280.h"    	  // BME280 library by SparkFun
#include "src\SparkFun_CCS811\src\SparkFunCCS811.h"    	  // CCS811 library by SparkFun
#include "src\SparkFun_AS3935\src\SparkFun_AS3935.h"          // AS3935 library by SparkFun
#include "src\TFTLCD-Library\Adafruit_TFTLCD.h"        	  // Hardware-specific library for TFT screen by Adafruit
#include "src\TouchScreen\TouchScreen.h"               	  // TouchScreen library by Adafruit
#include "bitmaps.h"                            			    // Icon library (local, hence the "")


/* --- first boot --- */
int firstBoot_EEPROMaddr = 5;   //EEPROM long term memory adress that we use to check if the EarthListener has been booted before (default will be true).


/* --- CCS811 sensor --- */
#define CCS811_ADDR 0x5B      //Default I2C Address of CCS811 sensor (secondary Address is 0x5A if jumper is closed)
CCS811 myCCS811(CCS811_ADDR);
int CO2;
int TVOC;


/* --- BME280 air sensor --- */
#define BME280_ADDR 0x77      //Default I2C Address of BME280 sensor, can be 0x76 if jumper closed
BME280 myBME280;
float HUMIDITY_BME280;
float AMBIENTPRESSURE_BME280;
float AMBIENTPRESSURE_BME280_c; //converted to mBar (= Pa / 100);
float ALTITUDE_BME280;
float TEMP_BME280;
float seaLevelPressure = 101325; //pressure at sea level (Pa), needs to be set daily for proper altitude value!
float TEMP_comp = -3.3;   //the EarthListener is warmed up from the TFT screen hence reporting a higher temperature. This compensation corrects this.
float HUMI_comp = 17;     //same for the humidity. Values are experimental and should be changed according to your findings.


/* --- AS3935 sensor --- */
//selection of IIC or SPI interface (also change the jumper JSI on the board!)
boolean AS3935_SPI = true;  //SPI= true & jumper JSI connected - IIC= false & jumper JSI disconnected
int AS3935_SPI_EEPROMaddr = 3;       // address to store this value in long term memory (EEPROM)

// Interrupt flag variable for AS3935 irqs that indicates interrupt has been triggered
// Variables that get changed in interrupt routines need to be declared volatile
// otherwise compiler can optimize them away, assuming they never get changed
volatile int AS3935IrqTriggered;
unsigned long AS3935IrqTriggeredTime = 0;     //time when last detection was triggered;
uint8_t int_src;                              //interrupt source:  This variable holds the number representing the lightning or non-lightning event issued by the lightning detector. 
int minutesSinceAS3935Trigger;                //time since last detection
String lastErrorLine1 =  "No Disturber (yet)...";   //Keep last error message (but set for when there is none)
String lastErrorLine2 =  "Keep Calm and Carry On";  //Keep last error message (but set for when there is none)

//defines
#define AS3935_ADD           0x03  // Default I2C Address of AS3935 sensor: x03 <-- you can use i2C scanner to see the i2c address
#define AS3935_IRQPIN        18    // interrupt pin on board (18 or 19 on MEGA, 2 or 3 on UNO)
#define spiCS                41    // chip select pin
#define INDOOR               0x12  // value for indoor use (do not change)   
#define OUTDOOR              0xE   // value for outdoor use (do not change)
#define LIGHTNING_INT        0x08  // value for lightning source (do not change)  
#define DISTURBER_INT        0x04  // value for disturber source (do not change)
#define NOISE_INT            0x01  // value for noise source (do not change)

boolean AS3935_OUTDOORS;            // Set to 1 to enabled for Outdoor Used, use 0 for Indoor Use
int AS3935_outdoor_EEPROMaddr = 0;          // address to store this value in long term memory (EEPROM)

// Values for modifying the AS3935's settings. All of these values are set in via the globalSense variable and controlled trough the setup page
byte noiseFloor;        // noise threshold value between 1-7, one being the lowest. Default setting is 2
byte watchDogVal;       // Watchdog threshold value between 1-10, one being the lowest. Default setting is 2
byte spike;             // Spike Rejection value between 1-11, one being the lowest. Default setting is 2. 
byte lightningThresh;   // lightning threshold value, can be 1, 5, 9 and 16. Default setting is 1.
boolean maskDisturbers; // Mask disturbers. Values can be true or false. Default setting is true.
byte globalSense;       // global sensitivity (sets values above + maskDisturber). Values are 1=low, 2=medium, 3=high. Default setting is 2.
int globalSense_EEPROMaddr = 4;       // address to store this value in long term memory (EEPROM)

SparkFun_AS3935 lightningSPI;              // define Lightning sensor for SPI
SparkFun_AS3935 lightningIIC(AS3935_ADD);  // define Lightning sensor on specific address for IIC
boolean AS3935_bootOK;                     // value to store OK boot status of the sensor


/* --- LCD panel with touch --- */
//be aware we use the TFT LCD on a Arduino Mega
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
#define touchPin 38 //pin to use for reading touchscreen
int Xpos; int Ypos; //global positions to store touchscreen location
unsigned long touchedTime = 0;     //time when last touchscreen interaction occurred;
int touchTimeout = 300;           // timeout between touch inputs (ms)

#define TS_MINX 100
#define TS_MAXX 920
#define TS_MINY 70
#define TS_MAXY 900
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 320 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 320);

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3         // Chip Select goes to Analog 3
#define LCD_CD A2         // Command/Data goes to Analog 2
#define LCD_WR A1         // LCD Write goes to Analog 1
#define LCD_RD A0         // LCD Read goes to Analog 0
#define LCD_RESET A4      // Normally A4, but can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
// http://www.barth-dev.de/online/rgb565-color-picker/
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY    0xD69A
#define GREYY   0xAD55 

//now declare tft item
MCUFRIEND_kbv tft;    //we use a DRIVER IC ILI9341
 

/* --- Speaker --- */
//pin setup for speaker
#define BuzzerPin A10
boolean BuzzerEnabled;      //1= Buzzer on, 0= Buzzer off => can be modified via TFT interface
int Buzzer_EEPROMaddr = 1;  // address to store this value in long term memory (EEPROM)


/* --- RGB LED --- */
#define LEDgnd A11
#define LEDg A12
#define LEDr A13
#define LEDb A14
boolean LEDenabled = 1;     //1= LED on, 0= LED off. Will also declare pins for output if turned on.


/* --- datalogging & timing --- */
// Modifications:
// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work. On our TFT SD card shield we use 10
const int chipSelect = 10;
boolean SDpresent = 0;        //boolean to store if SD card is present during setup
boolean logFileExists = 0;    //boolean to store if logfile exists on SD card
const char * logFileName = "datalog.csv";   //CSV so you can easy load the data into MS Excel
unsigned long allSeconds;
int runDays;
unsigned long secsRemaining;
int runHours;
int runMinutes;
int runSeconds;
int lastSecond = 99;        //we set this value so we can know when we just booted
int secondCounter = 0;      //counter to calculate the number of seconds that have been passed between loggings
int loggingInterval = 10;   //interval to log the values to the SD card. Default: 10, min: 1, maximum: 3600 (1 hour)


/* --- menus --- */
int currentScreenNr = 0;        //start with bootscreen
int previousScreenNr;
boolean slideShowPlaying;        //is the slide show playing? yes=1, no=0
int slideshowTimer = 5;         //time (in seconds) to show each slide
unsigned long timeStartSlide = 0;     //time when slide was first shown;


/* --- metric / imperial switch --- */
boolean MetricON;  //boolean to check if values of temperature and lightning distance are set in Celsius/km or Fahrenheit/miles => can be modified via TFT interface
int MetricON_EEPROMaddr = 2;  // address to store this value in long term memory (EEPROM)



/***************************************/
/* ----------- LOOP CODE ------------ */
/***************************************/
void loop(void)
{
  //timing: get time since boot: will write to global vars 
  getTimeSinceBoot();

  //get touch input (but only after XXX ms after last touchscreen event!)
  if((millis() - touchedTime) > touchTimeout)
  {
    if(touchedMe())
    {
      touchedTime = millis();
      //Serial.print("We have been touched! X=");Serial.print(Xpos);Serial.print(", Y="); Serial.println(Ypos);
      inputControl();
      showScreen(currentScreenNr);  //0:Boot, 1:Info screen, 2:Setup, 3:eCO², 4:Temperature 5:Pressure, 6:TVOC, 7:Humidity, 8:Lightning, 81:Lightning after interrupt
    }
  }

  //only do next code each second
  if(runSeconds != lastSecond)
  {
        //increase the secondCounter
        secondCounter++;
        
        /****** poll sensors & update vars + log to SD *****/
        // make a string for assembling the data to log on the SD card & add the current time:
        String dataString = "";
        char buf[17];
        sprintf(buf,"%02d %02d:%02d:%02d",runDays,runHours,runMinutes,runSeconds);
        dataString = buf;
        dataString += ",";

        //calculate minutes between current time & time when AS3935sensor was triggered
        minutesSinceAS3935Trigger = returnMinutes(millis() - AS3935IrqTriggeredTime);
        
        //read data from BME280
        HUMIDITY_BME280 = myBME280.readFloatHumidity();
        AMBIENTPRESSURE_BME280 = myBME280.readFloatPressure();
        AMBIENTPRESSURE_BME280_c = AMBIENTPRESSURE_BME280 / 100; //convert Pa to mBar
        ALTITUDE_BME280 = myBME280.readFloatAltitudeMeters();
        TEMP_BME280 = myBME280.readTempC();
        //compensate temp & humi data
        TEMP_BME280 = TEMP_BME280 + TEMP_comp;
        if(HUMIDITY_BME280 <= (100 - HUMI_comp))  //make sure we don't have values > 100%
        {
          HUMIDITY_BME280 = HUMIDITY_BME280 + HUMI_comp;
        }
        
        //print data from BME280 to serial
        /*
        Serial.print("BME280   data:");
        Serial.print("                                 ");
        Serial.print("Temp: ");
        Serial.print( TEMP_BME280, 2);
        Serial.print("°C");
        Serial.print("     Humidity: ");
        Serial.print(HUMIDITY_BME280, 2);
        Serial.print("%");
        Serial.print("     Pressure: ");
        Serial.print( AMBIENTPRESSURE_BME280_c, 2);
        Serial.print("mBar");
        Serial.print("     Altitude: ");
        Serial.print( ALTITUDE_BME280, 2);
        Serial.println("m");
        */
        
        //add data to dataString to write to SD
        dataString += TEMP_BME280,2;
        dataString += ",";
        dataString += HUMIDITY_BME280,2;
        dataString += ",";
        dataString += AMBIENTPRESSURE_BME280_c,2;
        dataString += ",";
        dataString += ALTITUDE_BME280,2;
        dataString += ",";

        //read data from CCS811 (or show error)
        if (myCCS811.checkForStatusError())
        {
          tft.fillRect(0,55,320,185,BLACK);  //clear part of the screen (startX, startY, width, height, color)
          printSensorError();
          delay(2000); //keep this info on screen for 2 seconds
          tft.fillScreen(BLACK); //clear screen for next run
        }
        else if (myCCS811.dataAvailable())
        {
          //Pass the temperature & humidity from BME280 sensor back into the CCS811 to compensate
          updateCCS811vars(TEMP_BME280,HUMIDITY_BME280);
          //read data from CCS811
          myCCS811.readAlgorithmResults(); //Calling this function updates the global tVOC and CO2 variables
          CO2 = myCCS811.getCO2();
          TVOC = myCCS811.getTVOC();

          //print data from CCS811 to serial
          /*
          Serial.print("CCS811   data:");
          Serial.print(" eCO²: ");
          Serial.print(CO2);
          Serial.print("ppm");
          Serial.print("     TVOC: ");
          Serial.print(TVOC);
          Serial.println("ppb");
          */
          
          //add data to dataString to write to SD
          dataString += CO2;
          dataString += ",";
          dataString += TVOC;
        }
    
        //write dataString to SD (if SD card is present & we have passed the interval to log)
        if(SDpresent && secondCounter >= loggingInterval)
        {
          secondCounter = 0;
          File dataFile = SD.open(logFileName, FILE_WRITE);
          // open the file. note that only one file can be open at a time,
          // so you have to close this one before opening another.
           
          // if the file is available, write to it:
          if (dataFile) 
          {
            if(!logFileExists)  //the logfile didn't exist, so first print headers
            {
              Serial.print("Logfile '");
              Serial.print(logFileName);
              Serial.println("' did not exist, so print titles first..."); 
              dataFile.println("Time since boot [DD HH:MM:SS],Temperature [°C],Humidity [%],Pressure [mBar],Altitude [m],eCO2 [ppm],TVOC [ppb]");
              logFileExists = 1;
            }
            dataFile.println(dataString);
            dataFile.close();

            // print to the serial port too:
            Serial.print("Written to file ");
            Serial.print(logFileName);
            Serial.print(" on SD card: ");
            Serial.println(dataString);
          }
          // if the file isn't open, pop up an error:
          else 
          {
            Serial.print("Error opening file ");
            Serial.print(logFileName);
            Serial.println(" on SD card! No data logged.");
          }
        }


        /****show screens****/
        //interrupt from lightning sensor! -> show lightning screen, otherwise show info screen
        //we only do this if AS3935_bootOK is true (otherwise noise from sensor)
        if (AS3935IrqTriggered && AS3935_bootOK)
        {
          Serial.println("Interrupt from lightning sensor");
          
          //check interrupt source
          delay(5); //wait so not to overflow the bus
          if(AS3935_SPI)
          {
            int_src = lightningSPI.readInterruptReg();
          }else
          {
            int_src = lightningIIC.readInterruptReg();
          }
          
          showScreen(81);   //0:Boot, 1:Info screen, 2:Setup, 3:eCO², 4:Temperature 5:Pressure, 6:TVOC, 7:Humidity, 8:Lightning, 81:Lightning after interrupt
          AS3935IrqTriggered = 0;
        }
        else
        {
          //check if slideshow is on
          if(slideShowPlaying)
          { 
            //automatically change the screen every time the slideshowTimer value has been reached
            if( ( millis() - timeStartSlide ) > (slideshowTimer * 1000) )
            {
                  previousScreenNr = currentScreenNr;
                  currentScreenNr++;
                  if(currentScreenNr > 8){currentScreenNr = 3;}
            }
          }
          
          //if a screen has been changed, reset the slide timer
          if(currentScreenNr != previousScreenNr)
          {
            timeStartSlide = millis();
          }
          showScreen(currentScreenNr);  //0:Boot, 1:Info screen, 2:Setup, 3:eCO², 4:Temperature 5:Pressure, 6:TVOC, 7:Humidity, 8:Lightning, 81:Lightning after interrupt
        }

      
        //update lastSecond so this loop can only be run at the next second
        lastSecond = runSeconds;
  }
}
