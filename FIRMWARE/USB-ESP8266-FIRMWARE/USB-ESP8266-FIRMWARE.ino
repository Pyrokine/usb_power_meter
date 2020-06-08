#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <BigNumber.h>

// 设置区
// WIFI用户名与密码，仅支持2.4G
const char* ssid     = "";
const char* password = "";
// 指纹将于2021.06.02过期,如果使用指纹信息，需要在过期前自行更新
const uint8_t fingerprint[20] = {0x40, 0xaf, 0x00, 0x6b, 0xec, 0x90, 0x22, 0x41, 0x8e, 0xa3, 0xad, 0xfa, 0x1a, 0xe8, 0x25, 0x41, 0x1d, 0x1a, 0x54, 0xb3};
String remote_host = "https://pyro.cafe/RBQ/IoT/Portal";
String local_host = "https://192.168.3.177:5000/RBQ/IoT/Portal";
// 选择使用远程或本地服务器，本地服务器地址可以自行修改，需要自行搭建，远程服务器需要自行测试是否能连接
String host_choice = remote_host;
//公钥n，在服务器上生成，或自行生成，使用RSA算法
BigNumber n = "";
//公钥e，65537，不需要修改
String e = "10000000000000001";
//设备ID，在服务器上生成
String device_id = "";
int len_e = e.length();
// END - 设置区

ESP8266WiFiMulti WiFiMulti;
String is_wifi = "0", is_web = "0";

void setup() {
  Serial.begin(115200);
  BigNumber::begin();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}

void loop() {
  if (WiFiMulti.run() == WL_CONNECTED){
    is_wifi = "1";
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
//    client->setFingerprint(fingerprint);
    client->setInsecure();  
    
    HTTPClient https;
    String serialData = "01";
    while (Serial.available() == 0){};
    delay(200);
    while (Serial.available() > 0){
      serialData += char(Serial.read());
    }

    String content = String("<") + device_id + String(">");
    bool END = false;
    int len_serialData = serialData.length();
    int cnt_serialData = 0;
    while (END == false){
      BigNumber m = "999";
      int cnt_m = 1;
      while (cnt_m < 50 && cnt_serialData < len_serialData) {
        m = m * BigNumber(1000) + BigNumber(char(serialData[cnt_serialData]));
        cnt_serialData += 1;
        cnt_m += 1;
      }
      if (cnt_m < 50){
        END = true;
      }
//      Serial.println(x);
      
      BigNumber c = "1";
      for (int i = 0; i < len_e; i++){
        if (e[i] == '1'){
          c = (c * m) % n;
        }
        m = (m * m) % n;
      }
      content += String("[") + c.toString() + String("]");
    }

//    Serial.println(content);
    if (https.begin(*client, host_choice)){
      int httpCode = https.POST(content);
      if (httpCode == 200){
        is_web = "1";
      }
      else{
        is_web = "0";
      }
//      Serial.println(httpCode);
    }
  }
  else{
    is_wifi = "0";
  }
  Serial.println(String("A") + is_wifi + is_web);
}
