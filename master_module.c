#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int SS_TEMP = 10;
const int SS_LIGHT = 9;
const int SS_SOIL = 8;

LiquidCrystal_I2C lcd(0x3F, 20, 4);

const int pinRed = 3;
const int pinBuzz = 1;
const int pinTrig = 0;
const int pinEcho = 2;
const int button1 = 4;
const int button2 = 5;
const int button3 = 6;
const int button4 = 7;

int rangeMax = 200;
int rangeMin = 0;

long T, L;
const int pinLM35 = 2;
float  currentTemp = 25.0, currentHumid = 85.0;
byte pumpState = 0, lightState = 0;

byte degree[8]={ B00110,
                 B01001,
                 B01001,
                 B00110,
                 B00000,
                 B00000,
                 B00000,
                 B00000,
               };

void setup() {
   Serial.end();
   lcd.begin();
   lcd.backlight();
   lcd.createChar(0, degree);
   lcd.setCursor(0, 2); lcd.print("Temp  : ");
   lcd.setCursor(13, 2); lcd.write((byte)0);
   lcd.setCursor(14, 2); lcd.print("C");
   lcd.setCursor(0, 3); lcd.print("Humid : ");
   lcd.setCursor(13, 3); lcd.print("%");
   pinMode(SS_TEMP, OUTPUT);
   pinMode(SS_SOIL, OUTPUT);
   pinMode(SS_LIGHT, OUTPUT);
   SPI.begin();
   digitalWrite(SS_TEMP, HIGH);
   digitalWrite(SS_LIGHT, HIGH);
   digitalWrite(SS_SOIL, HIGH);
   pinMode(pinRed,OUTPUT);
   pinMode(pinBuzz, OUTPUT);
   pinMode(pinTrig, OUTPUT);
   pinMode(pinEcho, INPUT);
   SPI.setClockDivider(SPI_CLOCK_DIV16);
}

void sensingInvade(){
   digitalWrite(pinTrig, LOW); delayMicroseconds(2);
   digitalWrite(pinTrig, HIGH); delayMicroseconds(10);
   digitalWrite(pinTrig, LOW);
   T = pulseIn(pinEcho, HIGH);
   L = T/58.52;
   if(L >= rangeMax || L <= rangeMin){ //측정 가능 지역 아닐 시
     Serial.println("-1");
   } else if(L < 10){ //20cm 이내로 물체 감지 시
     digitalWrite(pinBuzz,HIGH);
     digitalWrite(pinRed, HIGH);
   } else{ //평상 시
     digitalWrite(pinBuzz,LOW);
     digitalWrite(pinRed, LOW);
   }
}

byte transferAndWait(const byte what){
  byte a = SPI.transfer(what);
  delayMicroseconds(20);
  return a;
}

void loop() {
   sensingInvade();

   digitalWrite(SS_TEMP, LOW);
   transferAndWait('t');
   transferAndWait(0);

   currentTemp = transferAndWait(0);
   transferAndWait('h');
   transferAndWait(0);

   currentHumid = transferAndWait(0);
   digitalWrite(SS_TEMP, HIGH);
   Serial.println(currentTemp);
   Serial.println(currentHumid);
   digitalWrite(SS_SOIL, LOW);
   transferAndWait('w');
   transferAndWait(0);

   pumpState = transferAndWait(0);
   digitalWrite(SS_SOIL, HIGH);
   Serial.println(pumpState);

   if(pumpState == 1){
      lcd.setCursor(0, 0); lcd.print("Please Watering");
   }else{
      lcd.setCursor(0, 0); lcd.print("Water state Okay");
   }

   digitalWrite(SS_LIGHT, LOW);
   transferAndWait('l');
   transferAndWait(0);

   lightState = transferAndWait(0);
   digitalWrite(SS_LIGHT, HIGH);
   Serial.println(lightState);

   if(lightState == 0){
      lcd.setCursor(0, 1); lcd.print("Light is Okay");
   }else{
      lcd.setCursor(0, 1); lcd.print("Light is weak");
   }

   lcd.setCursor(0, 1);
   lcd.print("");
   
   //currentTemp = analogRead(pinLM35) * 5 * 100 / 1024.0;
   //currentHumid = 75.8;
   lcd.setCursor(8, 2); lcd.print(currentTemp, 1);
   lcd.setCursor(8, 3); lcd.print(currentHumid, 1);
   delay(1000);
}