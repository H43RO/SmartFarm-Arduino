#include<dht11.h>
#include<SoftwareSerial.h>
#include<Wire.h>
#include<SPI.h>

#define sv1 8

byte i = 1;
int mode = 0;

SoftwareSerial mySerial(4, 5); //UART

const int pinDHT11 = 2;
const int pinRelay = 3;

int ret;    //반환값 저장 변수

dht11 DHT11;
float hum;
float tem;

volatile byte command = 0;
volatile byte data1;
volatile byte data2;

void receiveEvent(int howMany){
    char c;
    while(Wire.available()){
     c = Wire.read();
    }
    if(c == 'w'){
        digitalWrite(pinRelay, HIGH);
        Serial.println("Moter ON !");
    }else{
        digitalWrite(pinRelay, LOW);
        Serial.println("Moter OFF !");
    }
}

void setup() {
    pinMode(pinRelay, OUTPUT);
    Wire.begin(sv1); //I2C
    Wire.onReceive(receiveEvent); //데이터 전송 받을 때 receiveEvent함수 호출 I2C
    Serial.begin(9600);
    mySerial.begin(9600); //UART
    SPI.begin();
    pinMode(MISO, OUTPUT);
    SPCR |= _BV(SPE);
    SPCR &= ~_BV(MSTR);
    SPCR |= _BV(SPIE);
    SPI.setClockDivider(SPI_CLOCK_DIV16); //SPI
}

ISR(SPI_STC_vect){
    mode++;
    byte c = SPDR;
 
    switch(command){
        case 0:
        command = c;
        SPDR = 0;
        break;

        case 't':
            if(mode % 2 ==0){\      
            SPDR = data1;
            }else{
                SPDR = data2;
            }
            break;

        case 'h':
            SPDR = data2;
            break;
    }  
}

void loop() {
 mySerial.write(tem);
 ret = DHT11.read(pinDHT11); //dht11타입에 정의된 reda()함수 실행 후 값 읽기1111
   
    switch(ret){
        case DHTLIB_OK: Serial.println("OK"); break;
        case DHTLIB_ERROR_CHECKSUM:  
        Serial.println("Check sum error"); break;
        case DHTLIB_ERROR_TIMEOUT:
        Serial.println("Time out error"); break;
        default: Serial.println("Unknown error"); break;
    }    //dht11가 전송한 데이터를 디지털 입력 핀에서 값을 잘 수신했는지 점검
 
    Serial.print("Humidity(%): ");
    Serial.println((float)DHT11.humidity,2);
    hum=(float)DHT11.humidity;
    Serial.print("Temperature(oC): ");
    tem=(float)DHT11.temperature;
    Serial.println((float)DHT11.temperature,2);

    if(tem > 27 || hum > 80){
        digitalWrite(pinRelay, HIGH);
        Serial.println("Moter ON");
    }
    else{
        digitalWrite(pinRelay, LOW);
        Serial.println("Moter OFF");
    }      

    data1 = (byte)tem;
    data2 = (byte)hum;

    if (digitalRead(SS) == HIGH){
         command = 0;
    }
    
    delay(1000);
}