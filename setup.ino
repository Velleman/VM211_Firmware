/***************************************/
/* ----------- SETUP CODE ------------ */
/***************************************/

void setup(void)
{
  /* --- Start serial & print sketch info --- */
  Serial.begin(9600);
  Serial.println("----------------------------------");
  Serial.print("Arduino is running Sketch: ");
  Serial.println(__FILE__);
  Serial.print("Compiled on: ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.println(__TIME__);
  Serial.println("----------------------------------");
  Serial.println();


  /* --- LCD screen feedback --- */
  Serial.println("***LCD screen feedback***");
  Serial.println(F("TFT LCD test"));
  #ifdef USE_ADAFRUIT_SHIELD_PINOUT
    Serial.println(F("Using Adafruit 2.4\" TFT Arduino Shield Pinout"));
  #else
    Serial.println(F("Using Adafruit 2.4\" TFT Breakout Board Pinout"));
  #endif
    Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
  tft.reset();

  uint16_t identifier = tft.readID();
  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  } else if (identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x7783) {
    Serial.println(F("Found ST7781 LCD driver"));
  } else if (identifier == 0x8230) {
    Serial.println(F("Found UC8230 LCD driver"));
  }
  else if (identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if (identifier == 0x0101)
  {
    identifier = 0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  } else
  {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier = 0x9341;
  }

  tft.begin(identifier);
  /*
  //Benchmark filling screen R/G/B
  Serial.println(F("Benchmark                Time (microseconds)"));
  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  delay(500);
  Serial.println(F("Done!"));
  */
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  pinMode(touchPin, OUTPUT);  //pin to control reading of touchscreen


  /* --- RGB LED pins --- */
  if(LEDenabled)  //set pins for RGB LED & set everything low => black, but only if LEDenabled = true
  {
    pinMode(LEDgnd, OUTPUT);   digitalWrite(LEDgnd, LOW);
    pinMode(LEDg, OUTPUT);     digitalWrite(LEDg, LOW);
    pinMode(LEDr, OUTPUT);     digitalWrite(LEDr, LOW);
    pinMode(LEDb, OUTPUT);     digitalWrite(LEDb, LOW);
  }


  /* --- DIGITAL outputs --- */
  pinMode(GreenOutPin, OUTPUT);     digitalWrite(GreenOutPin, LOW);
  pinMode(YellowOutPin, OUTPUT);    digitalWrite(YellowOutPin, LOW);
  pinMode(RedOutPin, OUTPUT);       digitalWrite(RedOutPin, LOW);
  

  /* --- Write LCD fields to show boot info --- */
  Serial.println("Show EarthListener boot animation");
  Serial.println();
  showScreen(0);


  /* --- Check first boot & if true, set values to default settings --- */
  if(EEPROM.read(firstBoot_EEPROMaddr))
  {
    //it's the first time the EarthListener has booted! Set default values
    EEPROM.write(AS3935_SPI_EEPROMaddr, false);   //set AS3935 interface to I2C
    EEPROM.write(AS3935_outdoor_EEPROMaddr, false);       //set AS3935 to indoor use
    EEPROM.write(globalSense_EEPROMaddr, 2);      //set AS3935 sensitivity to 2 (medium)
    EEPROM.write(Buzzer_EEPROMaddr, true);        //set buzzer on
    EEPROM.write(MetricON_EEPROMaddr, true);      //set values to metric
    EEPROM.write(firstBoot_EEPROMaddr, false);    //set firstboot on false, this will not be run again
  }


  /* --- Tuya --- */
  //If user does not use a pin: set the pin as PIN_UNUSED
  Serial.println("***TUYA interface***");
  Serial.println("TUYA init started");
  //tuyav.setDebug(true);
  tuyav.setDigitalInputs(30, 31, 32);                                                     //Set DigitalInputs: needs 3 pins
  tuyav.setAnalogInputs(PIN_UNUSED, PIN_UNUSED, PIN_UNUSED);                              //Set AnalogInputs
  tuyav.setDigitalOutputs(PIN_UNUSED, PIN_UNUSED, PIN_UNUSED, PIN_UNUSED, PIN_UNUSED);    //SetDigitalOutputs
  tuyav.setAnalogOutputs(PIN_UNUSED, PIN_UNUSED, PIN_UNUSED);                             //Set AnalogOutputs
  //UNCOMMENT NEXT LINE TO PAIR THE MODULE, COMMENT THE LINE AFTER PAIRING AND REFLASH THE ARDUINO
  //Next line should only be executed once
  //Serial.println("TUYA AP active"); tuyav.setWifiMode(SMART_CONFIG_STATE);
  tuyav.initialize();
  Serial.println();


  /* --- CCS811 sensor feedback --- */
  Serial.println("***CCS811 sensor***");
  CCS811Core::status returnCode = myCCS811.begin();
  Serial.print("CCS811 begin exited with: ");
  printDriverError( returnCode );
  Serial.println();
  Serial.println();


  /* --- AS3935 sensor feedback --- */
  Serial.println("***AS3935 sensor feedback***");
  Serial.print("Checking interface...");
  
  tft.setCursor(15, 135);
  tft.setTextColor(WHITE); 
  tft.setTextSize(2);
  tft.print("AS3935 status: ");
  
  //read value for AS3935_SPI out of EEPROM memory
  AS3935_SPI = EEPROM.read(AS3935_SPI_EEPROMaddr);
  //Serial.print("AS3935_SPI = "); Serial.println(AS3935_SPI);
  if(AS3935_SPI)
  {
    //SPI interface 
    Serial.println(" SPI");
    AS3935_SPI = true;    //set in case it's not properly set

    SPI.begin(); // For SPI
    if(!lightningSPI.beginSPI(spiCS, 2000000) ) 
    { 
      AS3935_bootOK = false;
    }
    else
    {
      //double check to be sure (read a value)
      int spikeValTest = lightningSPI.readSpikeRejection();
      if(spikeValTest != 0)
      {
        AS3935_bootOK = true;
      }
      else
      {
        AS3935_bootOK = false;
      }
    }

  }
  else
  {
    //IIC interface
    Serial.println(" IIC");
    AS3935_SPI = false;  //set in case it's not properly set

    Serial.print("Scanning at address 0x");
    if(AS3935_ADD < 10){Serial.print("0");}
    Serial.print(AS3935_ADD,HEX);
    Serial.println(", please wait...");
    
    Wire.begin(); // Begin Wire before lightning sensor. 
    if( !lightningIIC.begin() ){ // Initialize the sensor. 
      AS3935_bootOK = false;
    }
    else
    {
      //double check to be sure (read a value)
      int spikeValTest = lightningIIC.readSpikeRejection();
      if(spikeValTest != 0)
      {
        AS3935_bootOK = true;
      }
      else
      {
        AS3935_bootOK = false;
      }
    }
  }

  if(AS3935_bootOK)
  {
      Serial.println("Lightning sensor ready! Starting setup procedure...");
      tft.setTextColor(GREEN); 
      tft.print("DETECTED");
  
      updateLightningSense(); //set the sensitivity of the sensor
      setupAS3935();  //run setup of AS3935 sensor
    
      // enable interrupt (hook IRQ pin to Arduino Uno/Mega interrupt to AS3935_IRQPIN -> 18)
      pinMode(AS3935_IRQPIN, INPUT);   // See http://arduino.cc/en/Tutorial/DigitalPins
      attachInterrupt(digitalPinToInterrupt(AS3935_IRQPIN), interruptFunction, CHANGE);
      Serial.println("Setup AS3935 sensor done.");
      Serial.println();  
  }
  else
  {
      Serial.println("Lightning sensor did not start up!");
      Serial.println(); 
      tft.setTextColor(RED); 
      tft.print("ERROR");
      delay(1000);
  }

  
  /* --- BME280 sensor feedback --- */
  Serial.println("***BME280 Sensor feedback***");
  myBME280.setI2CAddress(BME280_ADDR);        //The I2C address must be set before .begin() otherwise the cal values will fail to load
  
  tft.setCursor(15, 165);
  tft.setTextColor(WHITE); 
  tft.setTextSize(2);
  tft.print("BME280 status: ");
  
  if(myBME280.beginI2C() == false)
  {
    Serial.println("BME280 Sensor connection failed!");
    tft.setTextColor(RED); 
    tft.print("NOT FOUND");
  }
  else
  {
    Serial.println("BME280 Sensor connection successful!");
    tft.setTextColor(GREEN); 
    tft.print("DETECTED");
  }
  myBME280.setFilter(1); //0 to 4 is valid. Filter coefficient. See 3.4.4
  myBME280.setStandbyTime(0); //0 to 7 valid. Time between readings. See table 27.
  myBME280.setTempOverSample(1); //0 to 16 are valid. 0 disables temp sensing. See table 24.
  myBME280.setPressureOverSample(1); //0 to 16 are valid. 0 disables pressure sensing. See table 23.
  myBME280.setHumidityOverSample(1); //0 to 16 are valid. 0 disables humidity sensing. See table 19.
  myBME280.setMode(MODE_NORMAL); //MODE_SLEEP, MODE_FORCED, MODE_NORMAL is valid. See 3.3
  myBME280.setReferencePressure(seaLevelPressure); //Adjust the sea level pressure used for altitude calculations. This should be a variable, not fixed!
  //If you do not set the correct sea level pressure for your location FOR THE CURRENT DAY it will not be able to calculate the altitude accurately!
  //Barometric pressure at sea level changes daily based on the weather!
  //read value from EEPROM if Temp is in °F or °C and lightning units are in km or mi
  MetricON = EEPROM.read(MetricON_EEPROMaddr);  //read metric or imperial state from memory. If not set, this will be true, since all EEPROM addresses are 0xFF by default
  Serial.println();


  /* --- set buzzer pin & read value from EEPROM --- */
  BuzzerEnabled = EEPROM.read(Buzzer_EEPROMaddr);
  //Serial.print("BuzzerEnabled = "); Serial.println(BuzzerEnabled);
  //BuzzerEnabled = 0;
  pinMode(BuzzerPin, OUTPUT);
  digitalWrite(BuzzerPin, LOW);


  /* --- SD card feedback --- */
  Serial.println("***SD card feedback***");
  Serial.print("Initializing...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SS, OUTPUT);
  tft.setCursor(15, 195);
  tft.setTextColor(WHITE); 
  tft.setTextSize(2);
  tft.print("SD card: ");
  // see if the card is present and can be initialized:
  if (SD.begin(chipSelect)) 
  {
      Serial.println(" card initialized!");
      SDpresent = 1;
      tft.setTextColor(GREEN); 
      tft.print("DETECTED");
      //Check if our logfile is already on the card
      if(SD.exists(logFileName))
      {
        Serial.print("Logfile '");
        Serial.print(logFileName);
        Serial.println("' exists!");
        logFileExists = 1;
      }
  }
  else
  {
     Serial.println(" card failed, or not present!");
     SDpresent = 0;
     tft.setTextColor(RED); 
     tft.print("NOT FOUND");
  }
  Serial.println();


  /* -- init the RTC module (RUN ONCE AFTER INSTALLING A RTC MODULE, THEN COMMENT OUT THIS BLOCK) */
  // Init a new chip by turning off write protection and clearing the clock halt flag. 
  // These methods needn't always be called. See the DS1302 datasheet for details.
  /*
  rtc.writeProtect(false);
  rtc.halt(false);
  // Make a new time object to set the date and time. Example: Saturday 14th of December 2019, 22:52:00
  Time newT(2019, 12, 14, 22, 52, 00, Time::kSaturday);
  // Set the time and date on the chip
  rtc.time(newT);
  */
  

  /* --- check if RTC is present --- */
  Time t = rtc.time();
  if (t.mon == 165)    //when there is no clock, this value will be returend as 165
  {
    RTCpresent = false; 
    Serial.println("No Real Time Clock available! Using seconds since boot for logging.");
  }
  else if(t.yr == 2000) //when there is an error with the RTC, year will be set on 2000
  {
    RTCpresent = false; 
    Serial.println("Real Time Clock error! Using seconds since boot for logging.");
  }
  else
  {
    RTCpresent = true;
    Serial.println("Real Time Clock available! Current time:"); 
    Serial.println(returnTime(t));
  }
  Serial.println();
  
  
  /* --- end of boot, wait 2 secs & set interrupt state, then show info screen --- */
  delay(2000);
  AS3935IrqTriggered = 0;   //set the interrupt state of the AS3935 sensor to 0 (fix data input on boot)
  Serial.println("***End of setup, starting loop***");
  Serial.println();
  slideShowPlaying = 0;   //we always start without slide show
  showScreen(1);    //show info screen
}
