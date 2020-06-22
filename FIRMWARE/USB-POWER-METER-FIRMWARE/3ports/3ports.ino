#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#define USB1_SWITCH PB5
#define USB2_SWITCH PB3
#define USB3_SWITCH PB4
#define USB1_U PA5
#define USB2_U PA1
#define USB3_U PA3
#define USB1_I PA4
#define USB2_I PA0
#define USB3_I PA2

//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(PB6, PB7, U8X8_PIN_NONE); //SCL SDA RESET
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE); //RESET
int cnt = 0, freq = 20;
float SUM1 = 0, SUM2 = 0, SUM3 = 0;
String wifi_status = "FOO", web_status = "FOO";
SoftwareSerial mySerial(PA11, PA12); // RX, TX

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_pcsenior_r);
  u8x8.clear();

  pinMode(PA6, INPUT);
  randomSeed(analogRead(PB10));

  //LEFT 1
  pinMode(USB1_SWITCH, OUTPUT);
  pinMode(USB1_U, INPUT);
  pinMode(USB1_I, INPUT);
  //LEFT 2
  pinMode(USB2_SWITCH, OUTPUT);
  pinMode(USB2_U, INPUT);
  pinMode(USB2_I, INPUT);
  //LEFT 3
  pinMode(USB3_SWITCH, OUTPUT);
  pinMode(USB3_U, INPUT);
  pinMode(USB3_I, INPUT);
}


void loop() {
  digitalWrite(USB1_SWITCH, 0);
  digitalWrite(USB2_SWITCH, 0);
  digitalWrite(USB3_SWITCH, 0);
  String serialData = "";
  while (Serial.available() > 0){
    serialData += char(Serial.read());
  }
  if (serialData != "" && serialData.length() < 10){
    if (serialData[0] == 'A'){
      mySerial.println(serialData);
      if (serialData[1] == '1'){
        wifi_status = "CON";
      }
      else if (serialData[1] == '0'){
        wifi_status = "DISCON";
      }
      web_status = String(serialData[2]) + String(serialData[3]) + String(serialData[4]);
    }
  }

  float U1 = analogRead(USB1_U) / 30.7; // x / 1023.0 * 3.3 * 10.1
  float I1 = analogRead(USB1_I) / 155.0; // x / 1023 * 3.3 / 100 / 0.005
  float P1 = U1 * I1;
  String USB1 = "1 " + String(U1) + "V " + String(P1) + "W  ";
  float U2 = analogRead(USB2_U) / 30.7;
  float I2 = analogRead(USB2_I) / 155.0;
  float P2 = U2 * I2;
  String USB2 = "2 " + String(U2) + "V " + String(P2) + "W  ";
  float U3 = analogRead(USB3_U) / 30.7;
  float I3 = analogRead(USB3_I) / 155.0;
  float P3 = U3 * I3;
  String USB3 = "3 " + String(U3) + "V " + String(P3) + "W  ";
  
  u8x8.setCursor(0, 0);
  u8x8.print(USB1);
  u8x8.setCursor(0, 1);
  u8x8.print(USB2);
  u8x8.setCursor(0, 2);
  u8x8.print(USB3);

  u8x8.setCursor(0, 4);
  u8x8.print(String("WIFI ") + wifi_status); 
  u8x8.setCursor(0, 5);
  u8x8.print(String("SERVER ") + web_status); 

  long randNum = random(-2147483648, 2147483647);
  if (cnt > freq){
    cnt = 0;
    String content = 
      String("{\"protocol\":\"01\",\"mux\":\"") + String(randNum) + ("\",\"data\":{") +
      String("\"1\":[") + String(int(100 * SUM1) / freq / 100.0) + String(",\"W\"],") +
      String("\"2\":[") + String(int(100 * SUM2) / freq / 100.0) + String(",\"W\"],") +
      String("\"3\":[") + String(int(100 * SUM3) / freq / 100.0) + String(",\"W\"]") +
      String("}}");
    Serial.println(content);
    SUM1 = 0;
    SUM2 = 0;
    SUM3 = 0;
  }
  else{
    cnt += 1;
    SUM1 += P1;
    SUM2 += P2;
    SUM3 += P3;
  }
  delay(500);
}
