#include <EEPROM.h>
#define EEPROM_SIZE 5
unsigned int rlAddress = 0;
unsigned int rsAddress = 1;
unsigned int fAddress = 2;
unsigned int mAddress = 3;
unsigned int sleepTimerAddress = 4;

unsigned int rl;
unsigned int rs;
unsigned int f;
unsigned int m;
unsigned int sleepTimer;
unsigned int sleepTimerCheck;
RTC_DATA_ATTR unsigned int rlSleep;
RTC_DATA_ATTR unsigned int rsSleep;
RTC_DATA_ATTR unsigned int fSleep;
RTC_DATA_ATTR unsigned int mSleep;
RTC_DATA_ATTR unsigned int sleepTimerSleep;
RTC_DATA_ATTR unsigned int sleepCounter = 1;
RTC_DATA_ATTR unsigned int needleTypeSleep;



#include <SPI.h>
#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS 15                  //so i can use 15 for output otherwise cant use chip select pin and crashes idk why
static const int spiClk = 1000000;  // 1 MHz
SPIClass* hspi = NULL;
int myCS = 23;

long buttonTimer = 0;
long longPressTime = 3000;
bool buttonActive;
bool longPressActive;

long buttonTimer1 = 0;
long longPressTime1 = 1500;
bool buttonActive1;
bool longPressActive1;
long longPressTimer1 = 0;

long buttonTimer2 = 0;
long longPressTime2 = 1500;
bool buttonActive2;
bool longPressActive2;
long longPressTimer2 = 0;

int battVoltage = 34;
int motorVoltage = 35;
int button = 2;  //power button
int motorDrive = 16;
int converterEnable = 17;
int button1 = 4;
int button2 = 5;  //upButton in terms of bitwise

int minutes = 0;
bool menu = false;
int needleType = 1;
int whichMenu = 1;
bool subMenu = false;
unsigned int buttonWaitTime = 250;
bool userActive;
bool needleChange;


unsigned int loopCount = 0;

int reads;      //variable for calculations
float voltage;  //variable for calculations
float motorVoltageSum;
float battSum;
int avgLoopCnt;

unsigned int digitalPot = 0;
RTC_DATA_ATTR unsigned int digitalPotSleep;
unsigned int incSmall = 1;
unsigned int incBig = 5;

unsigned int digitalPotBits[] = { 5, 13, 22, 29, 37, 44, 51, 58, 64, 71, 77, 82, 88, 93, 98, 103, 108,
                                  113, 118, 122, 126, 130, 134, 138, 142, 146, 149, 153, 156, 159, 163,
                                  166, 169, 172, 175, 177, 180, 183, 185, 188, 190, 193, 195, 198, 200,
                                  202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 224, 225, 227,
                                  229, 230, 232, 233, 235, 236, 238, 239, 241, 242, 244, 245, 246, 248,
                                  249, 250, 251 };

const unsigned char four[1024] PROGMEM = {
  0xff, 0xfe, 0x80, 0x03, 0xb6, 0xd9, 0xb6, 0xd9, 0xb6, 0xd9, 0xb6, 0xd9, 0x80, 0x03, 0xff, 0xfe
};

const unsigned char three[1024] PROGMEM = {
  0xff, 0xfe, 0x80, 0x03, 0xb6, 0xc1, 0xb6, 0xc1, 0xb6, 0xc1, 0xb6, 0xc1, 0x80, 0x03, 0xff, 0xfe
};

const unsigned char two[1024] PROGMEM = {
  0xff, 0xfe, 0x80, 0x03, 0xb6, 0x01, 0xb6, 0x01, 0xb6, 0x01, 0xb6, 0x01, 0x80, 0x03, 0xff, 0xfe
};

const unsigned char one[1024] PROGMEM = {
  0xff, 0xfe, 0x80, 0x03, 0xb0, 0x01, 0xb0, 0x01, 0xb0, 0x01, 0xb0, 0x01, 0x80, 0x03, 0xff, 0xfe
};

const unsigned char zero[1024] PROGMEM = {
  0xff, 0xfe, 0x80, 0x03, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x03, 0xff, 0xfe
};

const unsigned char rll[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x19, 0x80, 0x19, 0x80, 0x00, 0x00, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00,
  0x19, 0x80, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0x00, 0x89, 0x00, 0x89, 0x00,
  0xf1, 0x00, 0xa1, 0x00, 0x91, 0x00, 0x89, 0xf0
};

const unsigned char rmm[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x19, 0x80, 0x19, 0x80, 0x00, 0x00, 0x66, 0x60, 0x66, 0x60, 0x00, 0x00,
  0x19, 0x80, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0x10, 0x89, 0xb0, 0x89, 0x50,
  0xf1, 0x50, 0xa1, 0x50, 0x91, 0x10, 0x89, 0x10
};

const unsigned char fff[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdb, 0x6c, 0xdb, 0x6c, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x08, 0x00, 0x08, 0x00,
  0x0f, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00
};

const unsigned char mmm[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x0c, 0xc3, 0x0c, 0x18, 0x60, 0x18, 0x60,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x0d, 0x80, 0x0a, 0x80,
  0x0a, 0x80, 0x0a, 0x80, 0x08, 0x80, 0x08, 0x80
};

const unsigned char rlMenu[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x1e, 0x78, 0x00, 0x00, 0x1e, 0x78, 0x00,
  0x00, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x01, 0xe0, 0x07, 0x80,
  0x01, 0xe0, 0x07, 0x80, 0x00, 0xc0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x30, 0x00,
  0x00, 0x1e, 0x78, 0x00, 0x00, 0x1e, 0x78, 0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char rsMenu[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x1e, 0x78, 0x00, 0x00, 0x1e, 0x78, 0x00,
  0x00, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x83, 0x00, 0x01, 0xe3, 0xc7, 0x80,
  0x01, 0xe3, 0xc7, 0x80, 0x00, 0xc1, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x30, 0x00,
  0x00, 0x1e, 0x78, 0x00, 0x00, 0x1e, 0x78, 0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char fMenu[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x61, 0x86, 0x18, 0x3c, 0xf3, 0xcf, 0x3c,
  0x3c, 0xf3, 0xcf, 0x3c, 0x18, 0x61, 0x86, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char mMenu[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x60, 0x06, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0x60, 0x06, 0x00,
  0x18, 0x01, 0x80, 0x18, 0x3c, 0x03, 0xc0, 0x3c, 0x3c, 0x03, 0xc0, 0x3c, 0x18, 0x01, 0x80, 0x18,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char cclock[] PROGMEM = {
  0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x30, 0x06, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x80, 0x00, 0x03,
  0x00, 0x00, 0x60, 0x00, 0x04, 0x00, 0x00, 0x10, 0x00, 0x08, 0x00, 0x80, 0x08, 0x00, 0x10, 0x00,
  0x80, 0x04, 0x00, 0x20, 0x00, 0x80, 0x02, 0x00, 0x20, 0x00, 0x80, 0x02, 0x00, 0x40, 0x00, 0x80,
  0x01, 0x00, 0x40, 0x00, 0x80, 0x01, 0x00, 0x40, 0x00, 0x80, 0x01, 0x00, 0x80, 0x00, 0x80, 0x00,
  0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0xff, 0x00, 0x80,
  0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x80, 0x40,
  0x00, 0x00, 0x01, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00,
  0x00, 0x02, 0x00, 0x20, 0x00, 0x00, 0x02, 0x00, 0x10, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00,
  0x08, 0x00, 0x04, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x00, 0x60, 0x00, 0x00, 0xc0, 0x01, 0x80,
  0x00, 0x00, 0x30, 0x06, 0x00, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



//screen
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(115200);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, 1);
  pinMode(button, INPUT_PULLDOWN);
  delay(3000);
  if (digitalRead(button) == 1) {

    pinMode(motorVoltage, INPUT);
    pinMode(battVoltage, INPUT);

    //buttons and outputs
    pinMode(button1, INPUT_PULLDOWN);
    pinMode(button2, INPUT_PULLDOWN);
    pinMode(motorDrive, OUTPUT);
    pinMode(converterEnable, OUTPUT);

    buttonActive = false;
    longPressActive = false;
    buttonActive1 = false;
    longPressActive1 = false;
    buttonActive2 = false;
    longPressActive2 = false;
    loopCount = 0;
    motorVoltageSum = 0;
    battSum = 0;
    avgLoopCnt = 500;  // value will be decided
    menu = false;
    minutes = 0;
    whichMenu = 1;
    subMenu = false;
    userActive = false;
    needleChange = false;
    sleepTimerCheck = millis();

    EEPROM.begin(EEPROM_SIZE);
    delay(10);

    //sleep and flash to operation
    digitalPot = digitalPotSleep;
    if (sleepCounter == 1) {
      rl = EEPROM.read(rlAddress);
      rs = EEPROM.read(rsAddress);
      f = EEPROM.read(fAddress);
      m = EEPROM.read(mAddress);
      sleepTimer = EEPROM.read(sleepTimerAddress);
    } else {
      rl = rlSleep;
      rs = rsSleep;
      f = fSleep;
      m = mSleep;
      sleepTimer = sleepTimerSleep;
      needleType = needleTypeSleep;
    }
    sleepCounter++;



    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    delay(100);

    //initialise hspi with default pins
    //SCLK = 14, MISO = 12, MOSI = 13, SS = 15
    hspi = new SPIClass(HSPI);
    hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);  //SCLK, MISO, MOSI, SS
    pinMode(hspi->pinSS(), OUTPUT);
    digitalWrite(hspi->pinSS(), 1);
    pinMode(myCS, OUTPUT);
    digitalWrite(myCS, 1);

    digitalWrite(motorDrive, 0);
    digitalWrite(converterEnable, 1);


    display.clearDisplay();
    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(16, 1);
    display.println("ZERO");
    display.invertDisplay(true);
    //display.dim(false);
    display.display();
    delay(250);
    display.invertDisplay(false);
    delay(250);
    display.invertDisplay(true);
    delay(250);
    display.invertDisplay(false);
    delay(250);
    display.invertDisplay(true);
    delay(250);
    display.invertDisplay(false);
    delay(250);
    display.invertDisplay(true);
    delay(250);
    display.invertDisplay(false);
    delay(250);
    display.invertDisplay(true);
    delay(250);
    display.invertDisplay(false);
    delay(1000);
  } else {
    delay(1000);
    esp_deep_sleep_start();
  }
}



void loop() {
  if (digitalRead(motorDrive) == 1 || userActive == true) {
    userActive = false;
    sleepTimerCheck = millis();
  } else {
    if ((millis() - sleepTimerCheck) / 60000 >= sleepTimer) {
      sleepy();
    }
  }

  if (menu == false) {
    loopCount++;

    reads = analogRead(motorVoltage);
    voltage = reads * 3.1 / 4095;
    voltage = voltage * 4.83;
    motorVoltageSum = motorVoltageSum + voltage;

    reads = analogRead(battVoltage);
    voltage = reads * 3.1 / 4095;
    voltage = voltage * 1.37;
    battSum = battSum + voltage;

    if (loopCount > avgLoopCnt) {
      // update voltage reads on screen
      minutes = millis() / (60000);
      int hours = minutes / 60;
      minutes = minutes - (hours * 60);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.setTextColor(WHITE);
      display.print(hours);
      display.print(":");
      if (minutes < 10) {
        display.print("0");
      }
      display.print(minutes);

      motorVoltageSum = motorVoltageSum / loopCount;
      display.setTextSize(3);
      display.setCursor(17, 11);
      if (motorVoltageSum < 10) {
        display.print("0");
      }
      display.print(motorVoltageSum);
      display.setCursor(110, 11);
      display.print("V");

      battSum = battSum / avgLoopCnt;
      if (battSum > 3.9) {
        display.drawBitmap(112, 0, four, 16, 32, WHITE);
      } else if (3.9 >= battSum && battSum > 3.6) {
        display.drawBitmap(112, 0, three, 16, 32, WHITE);
      }
      if (3.6 >= battSum && battSum > 3.3) {
        display.drawBitmap(112, 0, two, 16, 32, WHITE);
      }
      if (3.3 >= battSum && battSum > 2.9) {
        display.drawBitmap(112, 0, one, 16, 32, WHITE);
      }
      if (battSum <= 2.9) {
        display.drawBitmap(112, 0, zero, 16, 32, WHITE);
      }
      if (needleType == 1) {
        display.drawBitmap(0, 11, rll, 12, 20, WHITE);
      } else if (needleType == 2) {
        display.drawBitmap(0, 11, rmm, 12, 20, WHITE);
      } else if (needleType == 3) {
        display.drawBitmap(0, 11, fff, 14, 20, WHITE);
      } else if (needleType == 4) {
        display.drawBitmap(0, 11, mmm, 14, 20, WHITE);
      }

      display.display();
      battSum = 0;
      motorVoltageSum = 0;
      loopCount = 0;
    }

    if (needleChange == true) {
      needleType++;
      if (needleType > 4) {
        needleType = 1;
      }
      delay(500); // laterrrrrrrrrrrrrrr ??
      needleChange = false;
      buttonActive = false;
      buttonActive1 = false;
    }



    //power button
    if (digitalRead(button) == 1 && needleChange == false) {
      userActive = true;

      if (digitalRead(button1) == 1) {
        needleChange = true;
        return;
      }

      if (buttonActive == false) {
        buttonActive = true;
        buttonTimer = millis();
      }

      if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
        longPressActive = true;
        sleepy();
      }

    } else {

      if (buttonActive == true) {

        if (longPressActive == true) {
          longPressActive = false;
        }

        else {
          digitalWrite(motorDrive, !(digitalRead(motorDrive)));
        }
        buttonActive = false;
      }
    }


    //down button in terms of bitwise
    if (digitalRead(button1) == 1 && needleChange == false) {
      userActive = true;

      if (digitalRead(button) == 1) {
        needleChange = true;
        return;
      }

      if (longPressActive1 == true && millis() - longPressTimer1 > 250) {
        if (digitalPot < incBig) {
          digitalPot = 0;
        } else {
          digitalPot = digitalPot - incBig;
        }
        sspiTransfer();
        longPressTimer1 = millis();
      }

      if (buttonActive1 == false) {
        buttonActive1 = true;
        buttonTimer1 = millis();
      }

      if ((millis() - buttonTimer1 > longPressTime1) && (longPressActive1 == false)) {
        longPressActive1 = true;
        longPressTimer1 = millis();
      }

    } else {

      if (buttonActive1 == true) {

        if (longPressActive1 == true) {
          longPressActive1 = false;
        }

        else {
          if (digitalPot < incSmall) {
            digitalPot = 0;
          } else {
            digitalPot = digitalPot - incSmall;
          }
          sspiTransfer();
        }
        buttonActive1 = false;
      }
    }



    //up button in terms of bitwise
    if (digitalRead(button2) == 1) {
      userActive = true;

      if (digitalRead(button) == 1) {
        menu = true;
        digitalWrite(motorDrive, 0);
        delay(buttonWaitTime);
      }

      if (longPressActive2 == true && millis() - longPressTimer2 > 250) {
        if (digitalPot + incBig > 75) {
          digitalPot = 75;
        } else {
          digitalPot = digitalPot + incBig;
        }
        sspiTransfer();
        longPressTimer2 = millis();
      }

      if (buttonActive2 == false) {
        buttonActive2 = true;
        buttonTimer2 = millis();
      }

      if ((millis() - buttonTimer2 > longPressTime2) && (longPressActive2 == false)) {
        longPressActive2 = true;
        longPressTimer2 = millis();
      }

    } else {

      if (buttonActive2 == true) {

        if (longPressActive2 == true) {
          longPressActive2 = false;
        }

        else {
          if (digitalPot + incSmall > 75) {
            digitalPot = 75;
          } else {
            digitalPot = digitalPot + incSmall;
          }
          sspiTransfer();
        }
        buttonActive2 = false;
      }
    }

  }

  else {  //menu
    if (digitalRead(button) == 1 || digitalRead(button1) == 1 || digitalRead(button2) == 1) {
      userActive = true;
    }
    if (whichMenu == 1) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(15, 0);
      display.print("RL");
      display.drawBitmap(10, 16, rlMenu, 32, 16, WHITE);
      display.setCursor(63, 6);
      display.print(rl);
      display.print(" mm");
      if (subMenu == true) {
        subMenuCentik();
      }
      display.display();
      if (subMenu == true && digitalRead(button2) == 1) {
        rl = rl + 1;
        if (rl > 5) {
          rl = 5;
        }
        delay(buttonWaitTime);
      } else if (subMenu == true && digitalRead(button1) == 1) {
        if (rl < 1) {
          rl = 0;
        } else {
          rl = rl - 1;
        }
        delay(buttonWaitTime);
      }

    } else if (whichMenu == 2) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(15, 0);
      display.print("RS");
      display.drawBitmap(10, 16, rsMenu, 32, 16, WHITE);
      display.setCursor(63, 6);
      display.print(rs);
      display.print(" mm");
      if (subMenu == true) {
        subMenuCentik();
      }
      display.display();
      if (subMenu == true && digitalRead(button2) == 1) {
        rs = rs + 1;
        if (rs > 5) {
          rs = 5;
        }
        delay(buttonWaitTime);
      } else if (subMenu == true && digitalRead(button1) == 1) {
        if (rs < 1) {
          rs = 0;
        } else {
          rs = rs - 1;
        }
        delay(buttonWaitTime);
      }

    } else if (whichMenu == 3) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(23, 3);
      display.print("F");
      display.drawBitmap(10, 17, fMenu, 32, 16, WHITE);
      display.setCursor(63, 6);
      display.print(f);
      display.print(" mm");
      if (subMenu == true) {
        subMenuCentik();
      }
      display.display();
      if (subMenu == true && digitalRead(button2) == 1) {
        f = f + 1;
        if (f > 5) {
          f = 5;
        }
        delay(buttonWaitTime);
      } else if (subMenu == true && digitalRead(button1) == 1) {
        if (f < 1) {
          f = 0;
        } else {
          f = f - 1;
        }
        delay(buttonWaitTime);
      }

    } else if (whichMenu == 4) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(21, 3);
      display.print("M");
      display.drawBitmap(10, 19, mMenu, 32, 16, WHITE);
      display.setCursor(63, 6);
      display.print(m);
      display.print(" mm");
      if (subMenu == true) {
        subMenuCentik();
      }
      display.display();
      if (subMenu == true && digitalRead(button2) == 1) {
        m = m + 1;
        if (m > 5) {
          m = 5;
        }
        delay(buttonWaitTime);
      } else if (subMenu == true && digitalRead(button1) == 1) {
        if (m < 1) {
          m = 0;
        } else {
          m = m - 1;
        }
        delay(buttonWaitTime);
      }

    } else if (whichMenu == 5) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.drawBitmap(0, 0, cclock, 33, 32, WHITE);
      display.setCursor(50, 8);
      display.print(sleepTimer);
      display.print(" min");
      if (subMenu == true) {
        subMenuCentik();
      }
      display.display();
      if (subMenu == true && digitalRead(button2) == 1) {
        sleepTimer = sleepTimer + 1;
        if (sleepTimer > 59) {
          sleepTimer = 59;
        }
        delay(buttonWaitTime);
      } else if (subMenu == true && digitalRead(button1) == 1) {
        if (sleepTimer < 2) {
          sleepTimer = 1;
        } else {
          sleepTimer = sleepTimer - 1;
        }
        delay(buttonWaitTime);
      }
    } else if (whichMenu == 6) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("Press power to save  settings to memory.");
      display.display();
    }

    if (digitalRead(button2) == 1 && subMenu == false) {
      whichMenu++;
      if (whichMenu > 6) {
        whichMenu = 1;
      }
      delay(buttonWaitTime);
    } else if (digitalRead(button) == 1 && subMenu == false) {
      if (whichMenu == 6) {
        EEPROM.write(rlAddress, rl);
        EEPROM.write(rsAddress, rs);
        EEPROM.write(fAddress, f);
        EEPROM.write(mAddress, m);
        EEPROM.write(sleepTimerAddress, sleepTimer);
        EEPROM.commit();
      } else {
        subMenu = true;
      }
      delay(buttonWaitTime);
    } else if (digitalRead(button) == 1 && subMenu == true) {
      subMenu = false;
      delay(buttonWaitTime);
    } else if (digitalRead(button1) == 1 && subMenu == false) {
      menu = false;
      delay(buttonWaitTime);
      buttonActive = false;
      buttonActive1 = false;
      buttonActive2 = false;
    }
  }
}

void sleepy() {
  digitalWrite(motorDrive, 0);
  digitalWrite(converterEnable, 0);
  digitalPotSleep = digitalPot;
  rlSleep = rl;
  rsSleep = rs;
  fSleep = f;
  mSleep = m;
  sleepTimerSleep = sleepTimer;
  needleTypeSleep = needleType;


  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(16, 1);
  display.println("ZERO");
  display.invertDisplay(true);
  display.display();
  display.invertDisplay(false);
  delay(250);
  display.invertDisplay(true);
  delay(250);
  display.invertDisplay(false);
  delay(250);
  display.invertDisplay(true);
  delay(250);
  display.invertDisplay(false);
  delay(250);
  display.invertDisplay(true);
  delay(250);
  display.invertDisplay(false);
  delay(250);
  display.invertDisplay(true);
  delay(1000);
  display.dim(true);
  delay(2000);
  display.invertDisplay(false);
  display.clearDisplay();
  display.display();

  esp_deep_sleep_start();
}

void sspiTransfer() {
  digitalWrite(myCS, 0);
  hspi->transfer(B00010001);
  hspi->transfer(digitalPotBits[digitalPot]);
  digitalWrite(myCS, 1);
}

void subMenuCentik() {
  display.setTextSize(1);
  display.setCursor(110, 0);
  display.print("**");
}
