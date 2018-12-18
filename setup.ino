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
  Serial.println();


  /* --- RGB LED pins --- */
  if(LEDenabled)  //set pins for RGB LED & set everything low => black, but only if LEDenabled = true
  {
    pinMode(LEDgnd, OUTPUT);   digitalWrite(LEDgnd, LOW);
    pinMode(LEDg, OUTPUT);     digitalWrite(LEDg, LOW);
    pinMode(LEDr, OUTPUT);     digitalWrite(LEDr, LOW);
    pinMode(LEDb, OUTPUT);     digitalWrite(LEDb, LOW);
  }
  

  /* --- Write LCD fields to show boot info --- */
  showScreen(0);


  /* --- CCS811 sensor feedback --- */
  Serial.println("***CCS811 sensor***");
  CCS811Core::status returnCode = myCCS811.begin();
  Serial.print("CCS811 begin exited with: ");
  printDriverError( returnCode );
  Serial.println();
  Serial.println();


  /* --- AS3935 sensor feedback --- */
  Serial.println("***AS3935 sensor feedback***");
  Serial.print("Scanning at address 0x");
  if(AS3935_ADD < 10){Serial.print("0");}
  Serial.print(AS3935_ADD,HEX);
  Serial.println(", please wait...");
  
  tft.setCursor(15, 135);
  tft.setTextColor(WHITE); 
  tft.setTextSize(2);
  tft.print("AS3935 status: ");

  Wire.begin();
  while (!Serial);           // Scan i2C Address
  delay(2000);
  
  Wire.beginTransmission(AS3935_ADD);
  byte error = Wire.endTransmission();
  if (error == 0)
  {
    Serial.print("Scanning complete, I2C device found at address 0x");
    if(AS3935_ADD < 10){Serial.print("0");}
    Serial.println(AS3935_ADD,HEX);

    tft.setTextColor(GREEN); 
    tft.print("DETECTED");

    Serial.println("Starting setup procedure ...");
    delay(500);
  
    I2c.begin(); // Set i2C Libr Enable pullups set to 400kHz speed
    I2c.pullup(true); //Set i2C pUll up
    I2c.setSpeed(1);  //Set speed to 1
    delay(10); // Delay
  
    lightX.AS3935_DefInit();   // set registers to default
    //read value for AS3935_OUTDOORS out of EEPROM memory
    AS3935_OUTDOORS = EEPROM.read(AS3935_EEPROMaddr);
    //Serial.print("AS3935_OUTDOORS = "); Serial.println(AS3935_OUTDOORS);
    //AS3935_OUTDOORS = 0;
    // now update sensor cal for your application and power up chip
    lightX.AS3935_ManualCal(AS3935_CAPACITANCE, AS3935_OUTDOORS, AS3935_DIST_EN); //
    //   AS3935_ManualCal Parameters:
    //   --> capacitance, in pF (marked on package)
    //   --> indoors/outdoors (INDOORS:0 / OUTDOORS:1)
    //   --> disturbers (AS3935_DIST_EN:1 / AS3935_DIST_DIS:2)
    // function also powers up the chip
  
    // enable interrupt (hook IRQ pin to Arduino Uno/Mega interrupt to AS3935_IRQPIN -> 19)
    pinMode(AS3935_IRQPIN, INPUT_PULLUP);   // See http://arduino.cc/en/Tutorial/DigitalPins
    attachInterrupt(digitalPinToInterrupt(AS3935_IRQPIN), interruptFunction, RISING);
  
    Serial.println("Printing out Reg vals:");
    lightX.AS3935_PrintAllRegs(); // Print all Regs
    Serial.println();  
  }
  else if (error==4) 
  {
    Serial.print("Unknow error at address 0x");
    if(AS3935_ADD < 10){Serial.print("0");}
    Serial.println(AS3935_ADD,HEX);
    Serial.println();  
    tft.setTextColor(RED); 
    tft.print("ERROR");
  }    
  else
  {
    Serial.print("Scanning complete, device not found at address 0x");
    if(AS3935_ADD < 10){Serial.print("0");}
    Serial.println(AS3935_ADD,HEX);
    Serial.println();  
    tft.setTextColor(RED); 
    tft.print("NOT FOUND");
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
    Serial.println("BME280 Sensor connection succesfull!");
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
  
  /* --- end of boot, wait 2 secs & set interrupt state, then show info screen --- */
  delay(2000);
  AS3935IrqTriggered = 0;   //set the interrupt state of the AS3935 sensor to 0 (fix data input on boot)
  Serial.println("***End of setup, starting loop***");
  Serial.println();
  slideShopPlaying = 0;   //we always start without slideshow
  showScreen(1);    //show info screen
}
