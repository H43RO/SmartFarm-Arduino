#include <SPI.h>
#include <SoftwareSerial.h>
#include<Wire.h>

#define sv2 9

SoftwareSerial mySerial(4, 5);

float t = 0;
char buf[100];
volatile byte pos = 0;
volatile boolean done = false;

const int LEDR = 6;
const int LEDB = 7;
const int LEDR1 = 8;
const int LEDB1 = 9;
const int LEDR2 = 10;
const int LEDB2 = 11;
const int LEDR3 = 12;
const int LEDB3 = 13;
const int CDS = 0;

volatile byte command = 0;
volatile byte data = 0;

int state = LOW;
boolean interrupt = false; 

void setup() {
  pinMode(MISO,OUTPUT);
  Wire.begin(sv2);
  Wire.onReceive(receiveEvent);
  pinMode (LEDR, OUTPUT);
  pinMode (LEDB, OUTPUT);
  pinMode (LEDR1, OUTPUT);
  pinMode (LEDB1, OUTPUT);
  pinMode (LEDR2, OUTPUT);
  pinMode (LEDB2, OUTPUT);
  pinMode (LEDR3, OUTPUT);
  pinMode (LEDB3, OUTPUT);
  SPCR |= _BV(SPE);
  SPCR |= _BV(SPIE);
  SPCR &= ~_BV(MSTR);
  mySerial.begin(9600);
  Serial.begin(9600);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
}

void atTempRising(){
  state = LOW;
  Serial.println("Temp Rising");
}

void atWaterPumping(){
  state = HIGH;
  Serial.println("Water Pumping");
}

void loop() {
    if(done){
      buf[pos] = 0;
      Serial.print(buf);
      pos = 0; done = false;
    }
    int cdsValue = analogRead(CDS);
    if ((cdsValue > 900)) {
      data = 1;
      if(mySerial.available()){
          t = mySerial.read();
          if(t >= 20){
            atTempRising();
            digitalWrite(LEDR, state);
            digitalWrite(LEDB, state);
            digitalWrite(LEDR1, state);
            digitalWrite(LEDB1, state);
            digitalWrite(LEDR2, state);
            digitalWrite(LEDB2, state);
            digitalWrite(LEDR3, state);
            digitalWrite(LEDB3, state);
          }
      }
      Serial.print("cds =  ");
      Serial.println(cdsValue);
      Serial.print("temp = ");
      Serial.println(t);
      state = HIGH;
      Serial.println("LED ON (cds < 900)");
    } else {
      data = 0;
      state = LOW;
      Serial.println("LED OFF (cds > 1000 )");
    }
    if (digitalRead(SS) == HIGH){
      command = 0;
    }
    digitalWrite(LEDR, state);
    digitalWrite(LEDB, state);
    digitalWrite(LEDR1, state);
    digitalWrite(LEDB1, state);
    digitalWrite(LEDR2, state);
    digitalWrite(LEDB2, state);
    digitalWrite(LEDR3, state);
    digitalWrite(LEDB3, state);
    delay(1000);
}

ISR(SPI_STC_vect){
  byte c = SPDR;
  switch(command){
    case 0:
    command = c;
    SPDR = 0;
    break;
    case 'l':
    SPDR = data;
    break;
  }
}

void receiveEvent(int howMany){
  while(Wire.available()){
    char c = Wire.read();
    Serial.println(c);
    if(c == 'w'){
      atWaterPumping();
      digitalWrite(LEDR, state);
      digitalWrite(LEDB, state);
      digitalWrite(LEDR1, state);
      digitalWrite(LEDB1, state);
      digitalWrite(LEDR2, state);
      digitalWrite(LEDB2, state);
      digitalWrite(LEDR3, state);
      digitalWrite(LEDB3, state);
    }
  }
}