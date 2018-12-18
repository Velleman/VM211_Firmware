/***************************************/
/* ------------ FUNCTIONS ------------ */
/***************************************/

//print final air quality (set color & LED)
void Print_Level(int data1)
{
  if (data1 < 600)
  {
    tft.setTextColor(GREEN,BLACK);
    tft.println("Excellent Air");
    controlLED('G');
    controlLogo(GREEN);
  }
  else if ((data1 >= 600) && (data1 < 800))
  {
    tft.setTextColor(GREEN,BLACK);
    tft.println("  Good Air   ");
    controlLED('G');
    controlLogo(GREEN);
  }
  else if ((data1 >= 800) && (data1 < 1000))
  {
    tft.setTextColor(BLUE,BLACK);
    tft.println("  Fair Air   ");
    controlLED('B');
    controlLogo(BLUE);
  }
  else if ((data1 >= 1000) && (data1 < 1500))
  {
    tft.setTextColor(YELLOW,BLACK);
    tft.println("Mediocre Air ");
    controlLED('Y');
    controlLogo(YELLOW);
  }
  else if (data1 >= 1500)
  {
    tft.setTextColor(RED,BLACK);
    tft.println("   Bad Air   ");
    controlLED('R');
    controlLogo(RED);
  }
}


//printDriverError decodes the CCS811Core::status type and prints the
//type of error to the serial terminal.
//
//Save the return value of any function of type CCS811Core::status, then pass
//to this function to see what the output was.
void printDriverError( CCS811Core::status errorCode )
{
  tft.setCursor(15, 105);
  tft.setTextColor(WHITE); 
  tft.setTextSize(2);

  tft.print("CCS811 status: ");

  switch ( errorCode )
  {
    case CCS811Core::SENSOR_SUCCESS:
      Serial.print("SUCCESS");
      tft.setTextColor(GREEN); 
      tft.print("DETECTED");
      break;
    case CCS811Core::SENSOR_ID_ERROR:
      Serial.print("ID_ERROR");
      tft.setTextColor(RED); 
      tft.print("ID_ERROR");
      break;
    case CCS811Core::SENSOR_I2C_ERROR:
      Serial.print("I2C_ERROR");
      tft.setTextColor(RED); 
      tft.print("I2C_ERROR");
      break;
    case CCS811Core::SENSOR_INTERNAL_ERROR:
      Serial.print("INTERNAL_ERROR");
      tft.setTextColor(RED); 
      tft.print("INTERNAL_ERROR");
      break;
    case CCS811Core::SENSOR_GENERIC_ERROR:
      Serial.print("GENERIC_ERROR");
      tft.setTextColor(RED); 
      tft.print("GENERIC_ERROR");
      break;
    default:
      Serial.print("Unspecified error.");
      tft.setTextColor(RED); 
      tft.print("Unspecified error.");
  }
}


//printSensorError gets, clears, then prints the errors
//saved within the error register.
void printSensorError()
{
  Serial.println("Error with CCS811 sensor!");
  uint8_t error = myCCS811.getErrorRegister();

  tft.setCursor(15, 105);
  tft.setTextColor(RED);  tft.setTextSize(2);
  tft.println("Error with CCS811 sensor!");

  if ( error == 0xFF ) //comm error
  {
    Serial.println("Failed to get ERROR_ID register.");
    tft.println("Failed to get ERROR_ID register!:");
  }
  else
  {
    Serial.print("Error: ");
    if (error & 1 << 5) Serial.print("HeaterSupply");     tft.println(" Error: HeaterSupply");
    if (error & 1 << 4) Serial.print("HeaterFault");      tft.println(" Error: HeaterFault");
    if (error & 1 << 3) Serial.print("MaxResistance");    tft.println(" Error: MaxResistance");
    if (error & 1 << 2) Serial.print("MeasModeInvalid");  tft.println(" Error: MeasModeInvalid");
    if (error & 1 << 1) Serial.print("ReadRegInvalid");   tft.println(" Error: ReadRegInvalid");
    if (error & 1 << 0) Serial.print("MsgInvalid");       tft.println(" Error: MsgInvalid");
    Serial.println();
  }
}


//function to show red, green and blue pattern on screen
//returns time it took to run the function
unsigned long testFillScreen()
{
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return micros() - start;
}


//update the vars in the CCS811 sensor with temp & humi data from BME280 sensor
void updateCCS811vars(float TEMP, float HUMIDITY)
{
  //Serial.println("Updating CCS811 sensor with enviroment values...");
  myCCS811.setEnvironmentalData( HUMIDITY, TEMP);
  //Serial.print("New humidity= "); Serial.print(HUMIDITY, 2); Serial.println("%");
  //Serial.print("New temp= "); Serial.print(TEMP, 2); Serial.println("°C");
}

  
//interrupt from AS3935 sensor, set var so loop handles this first
void interruptFunction()
{
  AS3935IrqTriggered = 1;
}


//change the LED color according to tatus
void controlLED(char COLOR)
{
    if(LEDenabled)
    {
      switch (COLOR) 
      {
        case 'R':     //red
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, HIGH);
          digitalWrite(LEDb, LOW);
          break;
        case 'G':     //green
          digitalWrite(LEDg, HIGH);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, LOW);
          break;
        case 'Y':     //yellow
          digitalWrite(LEDg, HIGH);
          digitalWrite(LEDr, HIGH);
          digitalWrite(LEDb, LOW);
          break;
        case 'B':     //blue
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, HIGH);
          break;
        case 'W':     //white
          digitalWrite(LEDg, HIGH);
          digitalWrite(LEDr, HIGH);
          digitalWrite(LEDb, HIGH);
          break;
        case 'O':     //off
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, LOW);
          break;  
        default:
          digitalWrite(LEDg, LOW);
          digitalWrite(LEDr, LOW);
          digitalWrite(LEDb, LOW);
      }
    }
}


//change the Velleman logo's color
void controlLogo(uint16_t logocolor)
{
    showbgd(103, 20, Velleman_114x36, 114, 36, logocolor, BLACK);
}


//convert the time since boot (in milliseconds) to readable format
void getTimeSinceBoot()
{
  allSeconds=     millis()      /   1000;
  runDays=        allSeconds    /   86400;
  secsRemaining=  allSeconds    %   86400;
  runHours=       secsRemaining /   3600;
  secsRemaining=  secsRemaining %   3600;
  runMinutes=     secsRemaining /   60;
  runSeconds=     secsRemaining %   60;
}


//convert a time in millis to minutes
int returnMinutes(unsigned long Millis)
{
  unsigned long conSeconds = Millis/1000;
  int conMinutes = conSeconds / 60;
  return conMinutes;
}


//check if the touchscreen has been touched & update Xpos & Ypos global vars
boolean touchedMe()
{
  digitalWrite(touchPin, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(touchPin, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
  {
    // scale from 0->1023 to tft.width
    Xpos = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    Ypos = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    /*
    Serial.print("X = "); Serial.print(Xpos);
    Serial.print("\tY = "); Serial.print(Ypos);
    Serial.print("\tPressure = "); Serial.println(p.z);
    */
    return 1; //we have been touched (yeey!)
  }
  else
  {
    return 0; //we haven't been touched (shame...)
  }
}


//function to print info to tft screen
void showScreen(int screenNr)
{
  currentScreenNr = screenNr; //set currentScreenVal
  
  //local vars for plotting the indicator onto the scales/graph
  int Xindic;
  float calcVal;
  //local vars for plotting the scales/graphs
  int startXimg = 10; int startYimg = 175;
  int widthImg = 300;  int heightImg = 18;

  //if a screen has been changed, clear the part below the header & update menu buttons
  if(currentScreenNr != previousScreenNr)
  {
    //tft.fillScreen(BLACK); //clear screen
    //we only have to clear part of the screen  (startX, startY, width, height, colour)
    tft.fillRect(10,70,310,170,BLACK); 
    //print menu buttons
    printMenuBtn();
  }

  //according to each screen, print some info
  switch (screenNr)
  {
      //Boot screen
      case 0:
        //print logo Velleman
        controlLogo(GREYY);
        //print logo EarthListener
        showbgd(35, 65, EarthListener_200x150, 200, 150, WHITE, BLACK);
        //print SW version
        tft.setFont(&FreeSans9pt7b);
        tft.setCursor(235, 207);
        tft.setTextColor(GREYY);  
        tft.setTextSize(1);
        tft.print(SWversion);
        //set font to standard system font
        tft.setFont();
        //test LED
        controlLED('W');
        delay(1500);
        //cycle logo & LED color
        showbgd(35, 65, EarthListener_200x150, 200, 150, RED, BLACK);
        controlLED('R');
        delay(500);
        showbgd(35, 65, EarthListener_200x150, 200, 150, GREEN, BLACK);
        controlLED('G');
        delay(500);
        showbgd(35, 65, EarthListener_200x150, 200, 150, BLUE, BLACK);
        controlLED('B');
        delay(500);
        //clear logo & version for sensor info
        tft.fillRect(0,65,320,175,BLACK);
        controlLED('0');
        break;

      
      //Info screen
      case 1:
        //print icons
        showbgd(40, 72, eCO2_65x50, 65, 50, WHITE, BLACK);
        showbgd(127, 72, temperature_65x50, 65, 50, WHITE, BLACK);
        showbgd(214, 72, pressure_65x50, 65, 50, WHITE, BLACK);
        showbgd(40, 148, tvoc_65x50, 65, 50, WHITE, BLACK);
        showbgd(127, 148, humidity_65x50, 65, 50, WHITE, BLACK);
        showbgd(214, 148, lightning_65x50, 65, 50, WHITE, BLACK);
        //print values from sensors
        printValues();
        break;

      
      //Setup screen
      case 2:
        //print values
        tft.setFont();  //standard system font
        tft.setTextSize(2);
        tft.setTextColor(GREYY,BLACK); 
        tft.setCursor(20, 80);
        tft.print("Buzzer on/off");
        tft.setCursor(20, 130);
        tft.print("Slideshow timer: "); tft.print(slideshowTimer); if(slideshowTimer < 10){tft.print(" ");}
        tft.setCursor(20, 180);
        tft.print("Indoor/outdoor use");
        //print icons (according to state)
        if(BuzzerEnabled)
        {
          showbgd(257, 75, slider_on_44x27, 44, 27, GREYY, BLACK);
        }
        else
        {
          showbgd(257, 75, slider_off_44x27, 44, 27, GREYY, BLACK);
        }
        showbgd(255, 125, arrows_47x26, 47, 26, GREYY, BLACK);
        if(AS3935_OUTDOORS)
        {
          showbgd(257, 175, slider_off_44x27, 44, 27, GREYY, BLACK);
        }
        else
        {
          showbgd(257, 175, slider_on_44x27, 44, 27, GREYY, BLACK);
        }
        //control LED
        controlLED('0'); //off
        //control Logo
        controlLogo(GREYY);
        break;

      
      //eCO2 screen
      case 3:
        //print value, icon & update LED
        tft.setFont();  //standard system font
        tft.setTextSize(3);
        tft.setCursor(140, 120);
        if (CO2 < 800)
        {
          tft.setTextColor(GREEN,BLACK);
          showbgd(10, 75, eCO2_100x77, 100, 77, GREEN, BLACK);
          controlLED('G');
          controlLogo(GREEN);
        }
        else if ((CO2 >=800) && (CO2 < 1000))
        {
          tft.setTextColor(BLUE,BLACK);
          showbgd(10, 75, eCO2_100x77, 100, 77, BLUE, BLACK);
          controlLED('B');
          controlLogo(BLUE);
        }
        else if ((CO2 >= 1000) && (CO2 < 1500))
        {
          tft.setTextColor(YELLOW,BLACK);
          showbgd(10, 75, eCO2_100x77, 100, 77, YELLOW, BLACK);
          controlLED('Y');
          controlLogo(YELLOW);
        }
        else if (CO2 >= 1500)
        {
          tft.setTextColor(RED,BLACK);
          showbgd(10, 75, eCO2_100x77, 100, 77, RED, BLACK);
          controlLED('R');
          controlLogo(RED);
        }      
        if(CO2 < 10)          {  tft.print("   "); }    //add leading spaces
        else if(CO2 < 100)    {  tft.print("  ");  }
        else if(CO2 < 1000)   {  tft.print(" ");   }
        tft.print(CO2);
        tft.println(" ppm");
        /*
        //print scale with fillRect(startX, startY, width, height, color)
        tft.fillRect( 10, 175, 46, 18, GREEN);
        tft.fillRect( 57, 175, 22, 18, BLUE);
        tft.fillRect( 80, 175, 56, 18, YELLOW);
        tft.fillRect( 137,175,173, 18, RED);
        */
        //print scale from bitmap file
        tft.setAddrWindow(startXimg, startYimg, startXimg + widthImg - 1, startYimg + heightImg - 1);
        tft.pushColors((const uint8_t*)eCO2_graph_300x18, widthImg * heightImg, 1, false);
        tft.fillRect( 10,193,300,  6, BLACK);  //earese the bottom under the scale (from previous indicator)
        //draw indicator with drawLine(startX, startY, endX, endY, color)
        //calcVal = ( ( (CO2-400) / (3000-400) ) * 300 ) + 10;
        calcVal = CO2-400;
        calcVal = calcVal / (3000-400);
        calcVal = calcVal * 300;
        calcVal = calcVal + 10;    
        Xindic = (int) calcVal;
        if(Xindic < 10){Xindic = 10;}
        else if(Xindic > 300){Xindic = 300;}
        tft.drawLine(Xindic, 175, Xindic, 198, WHITE);
        //print values of scale
        tft.setTextSize(1); 
        tft.setCursor(10, 165); tft.setTextColor(GREEN,BLACK); tft.print("400");
        tft.setCursor(48, 165); tft.setTextColor(BLUE,BLACK); tft.print("801");
        tft.setCursor(81, 165); tft.setTextColor(YELLOW,BLACK); tft.print("1001");
        tft.setCursor(138, 165); tft.setTextColor(RED,BLACK); tft.print("1501"); 
        tft.setCursor(285, 165); tft.setTextColor(RED,BLACK); tft.print("3000");       
        break;
        

      //Temperature screen
      case 4:
        //print value, icon & update LED
        tft.setFont();  //standard system font
        tft.setTextSize(3);
        tft.setCursor(140, 120);
        if(TEMP_BME280 < 0)
        {
          tft.setTextColor(GREY,BLACK);
          showbgd(10, 75, temperature_100x77, 100, 77, GREY, BLACK);
          controlLED('W');
          controlLogo(GREY);
        }
        else if (TEMP_BME280 <= 18)
        {
          tft.setTextColor(BLUE,BLACK);
          showbgd(10, 75, temperature_100x77, 100, 77, BLUE, BLACK);
          controlLED('B');
          controlLogo(BLUE);
        }
        else if ((TEMP_BME280 > 18) && (TEMP_BME280 <= 24))
        {
          tft.setTextColor(GREEN,BLACK);
          showbgd(10, 75, temperature_100x77, 100, 77, GREEN, BLACK);
          controlLED('G');
          controlLogo(GREEN);
        }
        else if ((TEMP_BME280 > 24) && (TEMP_BME280 <= 30))
        {
          tft.setTextColor(YELLOW,BLACK);
          showbgd(10, 75, temperature_100x77, 100, 77, YELLOW, BLACK);
          controlLED('Y');
          controlLogo(YELLOW);
        }
        else if (TEMP_BME280 > 30)
        {
          tft.setTextColor(RED,BLACK);
          showbgd(10, 75, temperature_100x77, 100, 77, RED, BLACK);
          controlLED('R');
          controlLogo(RED);
        }
        if(TEMP_BME280 < 10){   tft.print(" ");  }     //add leading spaces
        tft.print(TEMP_BME280,1);
        tft.print(" ");
        tft.print((char)247); //to print the ° symbol
        tft.println("C");
        /*
        //print scale with fillRect(startX, startY, width, height, color)
        tft.fillRect( 10, 175, 33, 18, GREY);
        tft.fillRect( 44, 175,122, 18, BLUE);
        tft.fillRect(167, 175, 39, 18, GREEN);
        tft.fillRect(207, 175, 39, 18, YELLOW);
        tft.fillRect(247, 175, 63, 18, RED);
        */
        //print scale from bitmap file
        tft.setAddrWindow(startXimg, startYimg, startXimg + widthImg - 1, startYimg + heightImg - 1);
        tft.pushColors((const uint8_t*)temp_graph_300x18, widthImg * heightImg, 1, false);
        tft.fillRect( 10,193,300, 6, BLACK);  //earese the bottom under the scale (from previous indicator)
        //draw indicator with drawLine(startX, startY, endX, endY, color)
        //calcVal = ( ( (TEMP_BME280+5) / (40+5) ) * 300 ) + 10;
        calcVal = TEMP_BME280 + 5;
        calcVal = calcVal / (40+5);
        calcVal = calcVal * 300;
        calcVal = calcVal + 10;    
        Xindic = (int) calcVal;
        if(Xindic < 10){Xindic = 10;}
        else if(Xindic > 300){Xindic = 300;}
        tft.drawLine(Xindic, 175, Xindic, 198, WHITE);
        //print values of scale
        tft.setTextSize(1); 
        tft.setCursor(10, 165); tft.setTextColor(GREY,BLACK); tft.print("-5");
        tft.setCursor(45, 165); tft.setTextColor(BLUE,BLACK); tft.print("0");
        tft.setCursor(167, 165); tft.setTextColor(GREEN,BLACK); tft.print("19");
        tft.setCursor(207, 165); tft.setTextColor(YELLOW,BLACK); tft.print("25");
        tft.setCursor(248, 165); tft.setTextColor(RED,BLACK); tft.print("31");
        tft.setCursor(295, 165); tft.setTextColor(RED,BLACK); tft.print("40"); 
        break; 

      
      //Pressure screen
      case 5:
        //print value, icon & update LED
        tft.setFont();  //standard system font
        tft.setTextSize(3);
        tft.setCursor(120, 110);
        tft.setTextColor(GREY,BLACK);
        if(AMBIENTPRESSURE_BME280_c < 1000)
        {
            tft.print(" ");     //add leading spaces
        }
        tft.print(AMBIENTPRESSURE_BME280_c,1);
        tft.println(" mBar");
        showbgd(10, 85, pressure_100x77, 100, 77, WHITE, BLACK);
        /*
        //plot pressure val @ sea level for comparison -> needs to be set for proper altitude value
        //(see note at case 9)
        tft.setTextSize(1);
        tft.setCursor(145, 140);
        tft.setTextColor(BLUE,BLACK);
        float convertedSeaLevelPressure;
        convertedSeaLevelPressure = seaLevelPressure / 100;
        tft.print(convertedSeaLevelPressure,1); tft.print(" mBar @ sea level");
        */
        controlLED('W');
        controlLogo(WHITE);
        break;

        
      //TVOC screen  
      case 6:
        //print value, icon & update LED
        tft.setFont();  //standard system font
        tft.setTextSize(3);
        tft.setCursor(140, 120);
        if (TVOC <= 50)
        {
          tft.setTextColor(GREEN,BLACK);
          showbgd(10, 75, tvoc_100x77, 100, 77, GREEN, BLACK);
          controlLED('G');
          controlLogo(GREEN);
        }
        else if ((TVOC > 50) && (TVOC <= 150))
        {
          tft.setTextColor(YELLOW,BLACK);
          showbgd(10, 75, tvoc_100x77, 100, 77, YELLOW, BLACK);
          controlLED('Y');
          controlLogo(YELLOW);
        }
        else if (TVOC > 150)
        {
          tft.setTextColor(RED,BLACK);
          showbgd(10, 75, tvoc_100x77, 100, 77, RED, BLACK);
          controlLED('R');
          controlLogo(RED);
        }
        if(TVOC < 10){          tft.print("   ");  }   //add leading spaces
        else if(TVOC < 100){    tft.print("  ");   }
        else if(TVOC < 1000){   tft.print(" ");    }
        tft.print(TVOC);
        tft.println(" ppb");
        /*
        //print scale with fillRect(startX, startY, width, height, color)
        tft.fillRect( 10, 175, 50, 18, GREEN);
        tft.fillRect( 61, 175, 99, 18, YELLOW);
        tft.fillRect( 161,175,149, 18, RED);
        */
        //print scale from bitmap file
        tft.setAddrWindow(startXimg, startYimg, startXimg + widthImg - 1, startYimg + heightImg - 1);
        tft.pushColors((const uint8_t*)tvoc_graph_300x18, widthImg * heightImg, 1, false);
        tft.fillRect( 10,193,300, 6, BLACK);  //earese the bottom under the scale (from previous indicator)
        //draw indicator with drawLine(startX, startY, endX, endY, color)
        //calcVal = TVOC + 10;
        calcVal = TVOC + 10;
        Xindic = (int) calcVal;
        if(Xindic < 10){Xindic = 10;}
        else if(Xindic > 300){Xindic = 300;}
        tft.drawLine(Xindic, 175, Xindic, 198, WHITE);
        //print values of scale
        tft.setTextSize(1); 
        tft.setCursor(10, 165); tft.setTextColor(GREEN,BLACK); tft.print("0");
        tft.setCursor(60, 165); tft.setTextColor(YELLOW,BLACK); tft.print("51");
        tft.setCursor(160, 165); tft.setTextColor(RED,BLACK); tft.print("151");
        tft.setCursor(290, 165); tft.setTextColor(RED,BLACK); tft.print("300");       
        break;

    
      //Humidity screen
      case 7:
        //print value, icon & update LED
        tft.setFont();  //standard system font
        tft.setTextSize(3);
        tft.setCursor(140, 120);
        if (HUMIDITY_BME280 < 30)
        {
          tft.setTextColor(RED,BLACK);
          showbgd(10, 75, humidity_100x77, 100, 77, RED, BLACK);
          controlLED('R');
          controlLogo(RED);
        }
        else if ((HUMIDITY_BME280 >= 30) && (HUMIDITY_BME280 <= 50))
        {
          tft.setTextColor(GREEN,BLACK);
          showbgd(10, 75, humidity_100x77, 100, 77, GREEN, BLACK);
          controlLED('G');
          controlLogo(GREEN);
        }
        else if (HUMIDITY_BME280 > 50)
        {
          tft.setTextColor(BLUE,BLACK);
          showbgd(10, 75, humidity_100x77, 100, 77, BLUE, BLACK);
          controlLED('B');
          controlLogo(BLUE);
        }
        if(HUMIDITY_BME280 < 10) { tft.print(" ");   }  //add leading spaces
        tft.print(HUMIDITY_BME280,1);
        tft.println(" %");
        /*
        //print scale with fillRect(startX, startY, width, height, color)
        tft.fillRect( 10, 175, 91, 18, RED);
        tft.fillRect( 102,175, 59, 18, GREEN);
        tft.fillRect( 162,175,148, 18, BLUE);
        */
        //print scale from bitmap file
        tft.setAddrWindow(startXimg, startYimg, startXimg + widthImg - 1, startYimg + heightImg - 1);
        tft.pushColors((const uint8_t*)humi_graph_300x18, widthImg * heightImg, 1, false);
        tft.fillRect( 10,193,300, 6, BLACK);  //earese the bottom under the scale (from previous indicator)
        //draw indicator with drawLine(startX, startY, endX, endY, color)
        //calcVal = ( (HUMIDITY_BME280 / 100) * 300) + 10;
        calcVal = HUMIDITY_BME280 * 3;
        calcVal = calcVal + 10;
        Xindic = (int) calcVal;
        if(Xindic < 10){Xindic = 10;}
        else if(Xindic > 300){Xindic = 300;}
        tft.drawLine(Xindic, 175, Xindic, 198, WHITE);
        //print values of scale
        tft.setTextSize(1); 
        tft.setCursor(10, 165); tft.setTextColor(RED,BLACK); tft.print("0");
        tft.setCursor(103, 165); tft.setTextColor(GREEN,BLACK); tft.print("31");
        tft.setCursor(163, 165); tft.setTextColor(BLUE,BLACK); tft.print("51");
        tft.setCursor(290, 165); tft.setTextColor(BLUE,BLACK); tft.print("100");       
        break;


      //lightning screen
      case 8:
        //print lightning information from previous strike
        tft.setFont();  //standard system font
        tft.setTextSize(2);
        tft.setCursor(35, 78);
        tft.setTextColor(RED,BLACK); 
        tft.println(lastErrorLine1);
        tft.setCursor(35, 98);
        tft.println(lastErrorLine2);
        //print icon
        showbgd(210, 125, lightning_100x77, 100, 77, YELLOW, BLACK);
        //print last detection
        tft.setCursor(35, 145);
        tft.setTextColor(YELLOW,BLACK);
        tft.println("Last detection:");
        tft.setCursor(35, 165);
        printLastDetectionTimeAS3935();
        break;


      //lightning screen after interrupt from sensor
      case 81:
        //print lightning information
        lightningscreen();
        int tempScreenNr;
        tempScreenNr = previousScreenNr;  //store previous screen to show later
        previousScreenNr = 81;
        showScreen(tempScreenNr);   //after lightning popup, show previous screen
        break;

        
      //Altitude screen
      // note: this screen is not shown in this project
      // If you want to use this screen (and proper values), first use myBME280.setReferencePressure(seaLevelPressure); 
      // If you do not set the correct sea level pressure for your location FOR THE CURRENT DAY it will not be able to calculate the altitude accurately!
      // Barometric pressure at sea level changes daily based on the weather!
      case 9:
        //print value, icon & update LED
        tft.setFont();  //standard system font
        tft.setTextSize(3);
        tft.setCursor(150, 110);
        tft.setTextColor(GREY,BLACK);
        tft.print(ALTITUDE_BME280,1);
        tft.println(" m");
        showbgd(10, 85, altitude_100x77, 100, 77, WHITE, BLACK);
        controlLED('W');
        controlLogo(WHITE);
        break;

        
      default:
        //Serial.println("Nothing to see here folks!");
        break;
  }
  //store our current screen so we can track if we changed screens later
  previousScreenNr = currentScreenNr;
}


//print the screen with lightning info after interrupt
//also stores error messages so we can show them later
void lightningscreen()
{
          tft.setFont();  //standard system font
          tft.setTextSize(2);
          tft.setCursor(35, 78);
          tft.setTextColor(RED,BLACK);
      
          //interpret interrupt source
          if (int_src == 0)
          {
            Serial.println("IRQ source result not expected...");
            lastErrorLine1 = "Error:IRQ source result";
            lastErrorLine2 = "      not expected...  ";
            tft.println(lastErrorLine1);
            tft.setCursor(35, 98);
            tft.println(lastErrorLine2);

          }
          else if (int_src == 1)
          {
            uint8_t lightning_dist_km = lightX.AS3935_GetLightningDistKm();
            Serial.print("Lightning detected! Distance to strike: ");
            Serial.print(lightning_dist_km);
            Serial.println("KM");
            lastErrorLine1 = "Lightning detected!    ";
            lastErrorLine2 = "Distance to strike:";
            lastErrorLine2 += lightning_dist_km;
            lastErrorLine2 += "KM ";
            tft.println(lastErrorLine1);
            tft.setCursor(35, 98);
            tft.println(lastErrorLine2);
          }
          else if (int_src == 2)
          {       
            Serial.println("Disturber detected!");
            lastErrorLine1 = "Disturber detected!    ";
            lastErrorLine2 = "                       ";
            tft.println(lastErrorLine1);
            tft.setCursor(35, 98);
            tft.println(lastErrorLine2);
          }
          else if (int_src == 3)
          {
            Serial.println("Noise level too high! Move the sensor.");
            lastErrorLine1 = "Noise level too high!  ";
            lastErrorLine2 = "Please move the sensor.";
            tft.println(lastErrorLine1);
            tft.setCursor(35, 98);
            tft.println(lastErrorLine2);
          }
          //lightX.AS3935_PrintAllRegs(); // Set for debug
          
          //print icon
          showbgd(210, 125, lightning_100x77, 100, 77, YELLOW, BLACK);

          //print last detection
          tft.setCursor(35, 145);
          tft.setTextColor(YELLOW,BLACK);
          tft.println("Last detection:");
          tft.setCursor(35, 165);
          printLastDetectionTimeAS3935();
          AS3935IrqTriggeredTime = millis();  //set pervious detection to now
      
          //buzz the buzzer for a short period + control the LED (if they are turned on)
          if(BuzzerEnabled){digitalWrite(BuzzerPin, HIGH);}
          controlLED('R');
          controlLogo(RED);
          delay(100);
          if(BuzzerEnabled){digitalWrite(BuzzerPin, LOW);}
          controlLED('Y');
          controlLogo(YELLOW);
          delay(200);
          if(BuzzerEnabled){digitalWrite(BuzzerPin, HIGH);}
          controlLED('R');
          controlLogo(RED);
          delay(100);
          if(BuzzerEnabled){digitalWrite(BuzzerPin, LOW);}
          controlLED('Y');
          controlLogo(YELLOW);
          
          delay(3000); //keep this info on screen for 3 seconds
}


//prints sensor information to screen (only used in info screen)
void printValues()
{
            tft.setFont();  //standard system font
            tft.setTextSize(1);
            
            //Print values, deleting those previously written by using a black background colour
            //CO2
            tft.setCursor(50, 125);
            if (CO2 < 800)
            {
              tft.setTextColor(GREEN,BLACK);
            }
            else if ((CO2 >=800) && (CO2 < 1000))
            {
              tft.setTextColor(BLUE,BLACK);
            }
            else if ((CO2 >= 1000) && (CO2 < 1500))
            {
              tft.setTextColor(YELLOW,BLACK);
            }
            else if (CO2 >= 1500)
            {
              tft.setTextColor(RED,BLACK);
            }
      
            if(CO2 < 10)          {  tft.print("   "); }    //add leading spaces
            else if(CO2 < 100)    {  tft.print("  ");  }
            else if(CO2 < 1000)   {  tft.print(" ");   }
            tft.print(CO2);
            tft.println(" ppm");
      
            //TVOC
            tft.setCursor(50, 200);
            if (TVOC <= 50)
            {
              tft.setTextColor(GREEN,BLACK);
            }
            else if ((TVOC > 50) && (TVOC <= 150))
            {
              tft.setTextColor(YELLOW,BLACK);
            }
            else if (TVOC > 150)
            {
              tft.setTextColor(RED,BLACK);
            }
            
            if(TVOC < 10){          tft.print("   ");  }   //add leading spaces
            else if(TVOC < 100){    tft.print("  ");   }
            else if(TVOC < 1000){   tft.print(" ");    }
            tft.print(TVOC);
            tft.println(" ppb");
      
            //Temperature
            tft.setCursor(140, 125);
            if(TEMP_BME280 < 0)
            {
              tft.setTextColor(GREY,BLACK);
            }
            else if (TEMP_BME280 <= 18)
            {
              tft.setTextColor(BLUE,BLACK);
            }
            else if ((TEMP_BME280 > 18) && (TEMP_BME280 <= 24))
            {
              tft.setTextColor(GREEN,BLACK);
            }
            else if ((TEMP_BME280 > 24) && (TEMP_BME280 <= 30))
            {
              tft.setTextColor(YELLOW,BLACK);
            }
            else if (TEMP_BME280 > 30)
            {
              tft.setTextColor(RED,BLACK);
            }
            if(TEMP_BME280 < 10){   tft.print(" ");  }     //add leading spaces
            tft.print(TEMP_BME280,1);
            tft.print(" ");
            tft.print((char)247); //to print the ° symbol
            tft.println("C  ");
      
            //Humidity
            tft.setCursor(140, 200);
            if (HUMIDITY_BME280 < 30)
            {
              tft.setTextColor(RED,BLACK);
            }
            else if ((HUMIDITY_BME280 >= 30) && (HUMIDITY_BME280 <= 50))
            {
              tft.setTextColor(GREEN,BLACK);
            }
            else if (HUMIDITY_BME280 > 50)
            {
              tft.setTextColor(BLUE,BLACK);
            }
            if(HUMIDITY_BME280 < 10) { tft.print(" ");   }  //add leading spaces
            tft.print(HUMIDITY_BME280,1);
            tft.println(" %");
      
            //Pressure
            tft.setCursor(215, 125);
            tft.setTextColor(GREY,BLACK);
            if(AMBIENTPRESSURE_BME280_c < 1000)
            {
              tft.print(" ");     //add leading spaces
            }
            tft.print(AMBIENTPRESSURE_BME280_c,1);
            tft.println(" mBar");

            //print last detection time lightning sensor
            tft.setCursor(210, 200);
            printLastDetectionTimeAS3935();
            
            //print total quality
            tft.setCursor(125, 225);
            Print_Level(CO2);
}


//print menue buttons, according to current screen
void printMenuBtn()
{
    if(currentScreenNr == 0)
    {
      //print nothing, boot menu
    }
    else if(currentScreenNr == 1) //info screen
    {
      //print slideshow control button
      if(slideShopPlaying)
      {
        showbgd(10, 210, play_23x23, 23, 23, GREYY, BLACK);
      }
      else
      {
        showbgd(10, 210, pauze_23x23, 23, 23, GREYY, BLACK);
      }
      //print setup button
      showbgd(287, 210, setup_23x23, 23, 23, GREYY, BLACK);
    }
    else if(currentScreenNr == 2) //setup screen
    {
      //show menu instead of setup icon
      showbgd(287, 210, menu_23x23, 23, 23, GREYY, BLACK);
    }
    else if(currentScreenNr == 81) //lightning screen after interrupt
    {
      //only show info icon, because no user input is allowed
      showbgd(10, 210, info_23x23, 23, 23, RED, BLACK);
    }
    else  //other screens
    {
      //print slideshow control button
      if(slideShopPlaying)
      {
        showbgd(10, 210, play_23x23, 23, 23, GREYY, BLACK);
      }
      else
      {
        showbgd(10, 210, pauze_23x23, 23, 23, GREYY, BLACK);
      }
      //print menu screen button
      showbgd(287, 210, menu_23x23, 23, 23, GREYY, BLACK);
      //print location circles
      for(int i=0; i<6; i++)
      {
        int xPosCircle = 110 + (i * 20);
        tft.fillCircle(xPosCircle, 221, 7, GREYY);   //fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
      }
      //fill currentscreen circle
      int xPosCircleSelected = 110 + ((currentScreenNr - 3) * 20);
      tft.fillCircle(xPosCircleSelected, 221, 5, BLACK);   //fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
    }
}


//looks at minutes since AS3935 trigger and outputs in none/min/hours/>1day to screen
void printLastDetectionTimeAS3935()
{
  if(AS3935IrqTriggeredTime == 0) //still set to init value -> no detection recorded!
  {
    tft.setTextColor(GREEN,BLACK);
    tft.print("    none    ");
    if(currentScreenNr == 8){controlLED('G');controlLogo(GREEN);};
  }
  else if(minutesSinceAS3935Trigger < 1)
  {
    tft.setTextColor(RED,BLACK);
    tft.print("< 1 min ago ");
    if(currentScreenNr == 8){controlLED('R');controlLogo(RED);};
  }
  else if(minutesSinceAS3935Trigger > 60)
  {
    int hoursSinceAS3935Trigger = minutesSinceAS3935Trigger / 60;
    tft.setTextColor(GREEN,BLACK);
    if(currentScreenNr == 8){controlLED('G');controlLogo(GREEN);};
    if(hoursSinceAS3935Trigger > 24)
    {
      tft.print("> 1 day ago ");
    }
    else
    {
      if(hoursSinceAS3935Trigger < 10)
      {
        tft.print(" "); //fix leading space
      }
      tft.print(hoursSinceAS3935Trigger);
      tft.print(" hours ago");
    }
  }
  else
  {
    if(minutesSinceAS3935Trigger <= 15)
    {
      tft.setTextColor(RED,BLACK);
      if(currentScreenNr == 8){controlLED('R');controlLogo(RED);};
    }
    else if((minutesSinceAS3935Trigger > 15) && (minutesSinceAS3935Trigger <= 30))
    {
      tft.setTextColor(YELLOW,BLACK); 
      if(currentScreenNr == 8){controlLED('Y');controlLogo(YELLOW);};
    }
    else
    {
      tft.setTextColor(GREEN,BLACK); 
      if(currentScreenNr == 8){controlLED('G');controlLogo(GREEN);};
    }
    if(minutesSinceAS3935Trigger < 10)
    {
      tft.print(" "); //fix leading space
    }
    tft.print(minutesSinceAS3935Trigger);
    tft.print(" min ago  ");
  }
}


//analyses the touch input (according to current screen) and links to other screens/functions
void inputControl()
{ 
    //first, check on which screen we are, then rule out what to do and which screen to go to
    switch (currentScreenNr) 
    {
      //boot screen
      case 0:
        // no touch functions
        break;
        
      //info screen
      case 1:
          //check slideshow button
          if( (Xpos > 0) && (Xpos < 45) && (Ypos > 0) && (Ypos < 30) )
          {
            //Serial.println("Touch on slideshow button!");
            toggleSlideShow();
            previousScreenNr = currentScreenNr;
            currentScreenNr = 3;    //start slideshow on screen 3
          }
          //check setup screen button
          else if( (Xpos > 0) && (Xpos < 45) && (Ypos > 210) && (Ypos < 245) )
          {
            //Serial.println("Touch on setup screen button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 2;
          }
          //check eCO2 button
          else if( (Xpos > 140) && (Xpos < 220) && (Ypos > 30) && (Ypos < 80) )
          {
            //Serial.println("Touch on eCO2 button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 3;
          }
          //check temp button
          else if( (Xpos > 140) && (Xpos < 220) && (Ypos > 100) && (Ypos < 140) )
          {
            //Serial.println("Touch on temp button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 4;
          }
          //check pressure button
          else if( (Xpos > 140) && (Xpos < 220) && (Ypos > 165) && (Ypos < 210) )
          {
            //Serial.println("Touch on pressure button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 5;
          }
          //check tvoc button
          else if( (Xpos > 50) && (Xpos < 110) && (Ypos > 30) && (Ypos < 80) )
          {
            //Serial.println("Touch on tvoc button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 6;
          }
          //check humidity button
          else if( (Xpos > 50) && (Xpos < 110) && (Ypos > 100) && (Ypos < 140) )
          {
            //Serial.println("Touch on humidity button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 7;
          }
          //check lightning button
          else if( (Xpos > 50) && (Xpos < 110) && (Ypos > 165) && (Ypos < 210) )
          {
            //Serial.println("Touch on lightning button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 8;
          }
          else
          {
            //Serial.println("I do not know what you are touching...");
          }
        break;
        
      //setup screen
      case 2:
        //check info screen button
        if( (Xpos > 0) && (Xpos < 40) && (Ypos > 215) && (Ypos < 245) )
        {
            //Serial.println("Touch on info screen button!");
            previousScreenNr = currentScreenNr;
            currentScreenNr = 1;
        }
        //check buzzer button
        else if( (Xpos > 180) && (Xpos < 215) && (Ypos > 190) && (Ypos < 235) )
        {
            //Serial.println("Touch on buzzer button!");
            toggleBuzzer();
        }
        //check indoor/outdoor button
        else if( (Xpos > 50) && (Xpos < 85) && (Ypos > 190) && (Ypos < 235) )
        {
            //Serial.println("Touch on indoor/outdoor button!");
            toggleIndoor();
        }
        //check arrow left
        else if( (Xpos > 110) && (Xpos < 150) && (Ypos > 190) && (Ypos < 208) )
        {
            //Serial.println("Touch on arrow left!");
            slideshowTimer--;
            if(slideshowTimer <1){slideshowTimer = 1;}
            
        }
        //check arrow right
        else if( (Xpos > 110) && (Xpos < 150) && (Ypos > 214) && (Ypos < 235) )
        {
           //Serial.println("Touch on arrow right!");
            slideshowTimer++;
            if(slideshowTimer > 60){slideshowTimer = 60;}
        }
        break;
        
      //eCO² screen
      case 3:
        checkBaseTouch();
        break;
        
      //temperature screen
      case 4:
        checkBaseTouch();
        break;
        
      //pressure screen
      case 5:
        checkBaseTouch();
        break;
        
      //TVOC screen
      case 6:
        checkBaseTouch();
        break;

      //humidity screen
      case 7:
        checkBaseTouch();
        break;
        
      //lightning screen
      case 8:
        checkBaseTouch();
        break;
        
      //lightning screen after interrupt
      case 81:
        // no touch functionality (delay of 3 seconds, so touch would never be handled correct)
        break;

      //altitude screen
      case 9:
        checkBaseTouch();
        break;
        
      default:
        // no default statements
        break;
    }
}


//check for touches in detailed menus 
//these values were found by simply touching parts of the screen and defining borders
//there might be some logic, but I tried untill it worked :-)
void checkBaseTouch()
{
  //check slideshow button
  if( (Xpos > 0) && (Xpos < 45) && (Ypos > 0) && (Ypos < 30) )
  {
    //Serial.println("Touch on slideshow button!");
    toggleSlideShow();
    previousScreenNr = currentScreenNr;
    currentScreenNr++;    //go to the next slide immediately
    if(currentScreenNr > 8){currentScreenNr = 3;}
  }
  //check info screen button
  else if( (Xpos > 0) && (Xpos < 45) && (Ypos > 210) && (Ypos < 245) )
  {
    //Serial.println("Touch on info screen button!");
    previousScreenNr = currentScreenNr;
    currentScreenNr = 1;
    slideShopPlaying = 0;
  }
  //check input left side of screen
  else if( (Xpos > 50) && (Xpos < 300) && (Ypos > 0) && (Ypos < 100) )
  {
    //Serial.println("Touch on left side of screen!");
    previousScreenNr = currentScreenNr;
    currentScreenNr--;
    if(currentScreenNr < 3){currentScreenNr = 8;}
    slideShopPlaying = 0;
  }
  //check input right side of screen
  else if( (Xpos > 50) && (Xpos < 300) && (Ypos > 140) && (Ypos < 245) )
  {
    //Serial.println("Touch on right side of screen!");
    previousScreenNr = currentScreenNr;
    currentScreenNr++;
    if(currentScreenNr > 8){currentScreenNr = 3;}
    slideShopPlaying = 0;
  }
  //check input circles middle of screen
  else if( (Xpos > 0) && (Xpos < 40) && (Ypos > 80) && (Ypos < 165) )
  {
    //Serial.println("Touch on middle bottom of screen!");
    previousScreenNr = currentScreenNr;
    currentScreenNr++;
    if(currentScreenNr > 8){currentScreenNr = 3;}
    slideShopPlaying = 0;
  }
  else
  {
    //Serial.println("I have no idea what you are trying to touch...");
  }
}


//check state of Slideshow & toggle
void toggleSlideShow()
{
  if(slideShopPlaying) { slideShopPlaying = 0; } else { slideShopPlaying = 1; }
  printMenuBtn(); //reprint button
}


//check state of Buzzer & toggle + write to long term memory (so if you loose power, we still know what you want)
void toggleBuzzer()
{
  if(BuzzerEnabled) { BuzzerEnabled = 0; } else { BuzzerEnabled = 1; }
  //write updated value to EEPROM long term memory
  EEPROM.write(Buzzer_EEPROMaddr, BuzzerEnabled);
}


//check status of indoor mode & toggle + write to long term memory (so if you loose power, we still know what you want)
void toggleIndoor()
{
  if(AS3935_OUTDOORS){ AS3935_OUTDOORS = 0; } else { AS3935_OUTDOORS = 1; }
  //write updated value to EEPROM long term memory
  EEPROM.write(AS3935_EEPROMaddr, AS3935_OUTDOORS);
  //push reboot message
  tft.setFont();  //standard system font
  tft.setTextSize(2);
  tft.setTextColor(RED,BLACK); 
  tft.setCursor(20, 180);
  tft.print("Please reboot!    ");
  delay(800);
}


// Draws a bitmap on the screen, works way faster then drawBitmap()!
// showbgd(start position X, start position y, bitmap, width, height, colour, background colour)
// GFX drawBitmap() expects each row to be on 8-bit boundary.  i.e. pad = 7
// Mono BMP rows are on 32-bit boundary. i.e. pad = 31,  rows are in RVS order.
// FreeFont bitmaps have no padding.  i.e. pad = 0.   e.g. 5x4 bitmap is in 3 bytes
void showbgd(int x, int y, const uint8_t *bmp, int w, int h, uint16_t color, uint16_t bg)
{
    uint8_t pad = 7;
    int yy, xx;
    uint8_t first = 1, RVS = pad & 0x80;
    uint16_t pixel;
    pad &= 31;
    uint16_t wid = (w + pad) & ~pad;                    //bits per row
    tft.setAddrWindow(x, y, x + w - 1, y + h - 1); //
    for (yy = 0; yy < h; yy++) {
        uint32_t ofs = (RVS ? (h - yy - 1) : yy) * wid; //bit offset
        const uint8_t *p = bmp + (ofs >> 3);            //flash address
        uint8_t mask = 0x80 >> (ofs & 7);               //bit mask
        uint8_t c = pgm_read_byte(p++);
        for (xx = 0; xx < w; xx++) {
            if (mask == 0) {
                c = pgm_read_byte(p++);
                mask = 0x80;
            }
            pixel = (c & mask) ? color : bg;
            tft.pushColors(&pixel, 1, first);
            first = 0;
            mask >>= 1;
        }
    }
    tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1);
}
