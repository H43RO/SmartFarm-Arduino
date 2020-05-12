#include <Wire.h>
#include <SPI.h>

#define sv1 8
#define sv2 9

volatile byte state = 0;
volatile byte command = 0;

const int Soil_Sensor_1 = A1;
const int Soil_Sensor_2 = A0;
const int A_1A = 6;
const int A_1B = 7;
const int B_1A = 8;
const int B_1B = 9;
const int A_2A = 4;
const int A_2B = 5;
const int B_2A = 2;
const int B_2B = 3;

void setup() {
  pinMode(MISO, OUTPUT);
  Wire.begin();                 // I2C 통신 초기화
  Serial.begin(9600);

  pinMode(A_1A, OUTPUT);
  pinMode(A_1B, OUTPUT);
  pinMode(B_1A, OUTPUT);
  pinMode(B_1B, OUTPUT);
  pinMode(A_2A, OUTPUT);
  pinMode(A_2B, OUTPUT);
  pinMode(B_2A, OUTPUT);
  pinMode(B_2B, OUTPUT);

  SPCR |= _BV(SPE);
  SPCR &= ~_BV(MSTR);
  SPCR |= _BV(SPIE);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
}

ISR(SPI_STC_vect) {
  byte c = SPDR;
  switch (command) {
    case 0:
      command = c;
      SPDR = 0;
      break;
    case 'w':
      SPDR = state;
      break;
  }
}

void pump(int flag) {
  // 1번
  boolean inPinA_1A = HIGH;
  boolean inPinA_1B = LOW;
  boolean inPinB_1A = HIGH;
  boolean inPinB_1B = LOW;

  // 2번
  boolean inPinA_2A = HIGH;
  boolean inPinA_2B = LOW;
  boolean inPinB_2A = HIGH;
  boolean inPinB_2B = LOW;  

  if(flag == 1) {             // 모터 드라이버 작동 시작
    inPinA_1A = HIGH;         // 서로 값이 달라야 정방향, 역방향 회전
    inPinA_1B = LOW;          
    inPinB_1A = HIGH;
    inPinB_1B = LOW;
    inPinA_2A = HIGH;
    inPinA_2B = LOW;
    inPinB_2A = HIGH;
    inPinB_2B = LOW;
  }

  digitalWrite(A_1A, inPinA_1A);
  digitalWrite(A_1B, inPinA_1B);
  digitalWrite(B_1A, inPinB_1A);
  digitalWrite(B_1B, inPinB_1B);
  digitalWrite(A_2A, inPinA_2A);
  digitalWrite(A_2B, inPinA_2B);
  digitalWrite(B_2A, inPinB_2A);
  digitalWrite(B_2B, inPinB_2B);
}

void stop() {               // 모터 드라이버 동작 멈춤
  digitalWrite(A_1A, LOW);
  digitalWrite(A_1B, LOW);
  digitalWrite(B_1A, LOW);
  digitalWrite(B_1B, LOW);
  digitalWrite(A_2A, LOW);
  digitalWrite(A_2B, LOW);
  digitalWrite(B_2A, LOW);
  digitalWrite(B_2B, LOW);
}

void loop() {
  int soil_1 = analogRead(Soil_Sensor_1);
  int soil_2 = analogRead(Soil_Sensor_2);

  Serial.println(soil_1);
  Serial.println(soil_2);

  if(soil_1 > 500 | soil_2 > 500) {    // 수분량이 많을 경우 값이 낮아짐.
    state = 1;
    pump(1);    // 모터 작동 시킴
    // sv1에게 데이터 전송 시작
    Wire.beginTransmission(sv1);  // I2C 통신에 참여하고 있는 sv1에게 데이터 전송을 시작함을 의미
    Wire.write('w');              // & write()로 큐에 넣어 놓은 데이터를 슬레이브에게 전송
    Wire.endTransmission();       // beginTransmission()으로 시작된 슬레이브에 대한 데이터 전송 종료

    // sv2에게 데이터 전송 시작
    Wire.beginTransmission(sv2);  // I2C 통신에 참여하고 있는 sv1에게 데이터 전송을 시작함을 의미
    Wire.write('w');              // & write()로 큐에 넣어 놓은 데이터를 슬레이브에게 전송
    Wire.endTransmission();       // beginTransmission()으로 시작된 슬레이브에 대한 데이터 전송 종료
   } else {
      state = 0;
      stop();     // 모터 멈춤
  }

  if(digitalRead(SS) == HIGH){
    command = 0;
  }

  delay(1000);
}