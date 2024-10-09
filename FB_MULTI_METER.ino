
/**
  MIT License

  Copyright (c) 2017 Boodskap Inc

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#include <ArduinoJson.h>
#include <BoodskapCommunicator.h>
#include <ModbusMaster.h>
#include <SoftwareSerial.h>

#define MAX485_TX_ENABLE  D11  // EIA-485 transmit control pin
#define EIA485_RX         D3  // EIA-485 serial receive pin
#define EIA485_TX         D9  // EIA-485 serial transmit pin

#define GEN_ID "1"
#define GEN_ID1 "2"
#define M_ST "0"
//#define M_ID2 "0"
//#define M_ID3 "0"
//#define GEN_ID1 "2"
//#define GEN_ID1 "2"

ModbusMaster node;
ModbusMaster node1;
ModbusMaster node2;

ModbusMaster node3;
ModbusMaster node4;
ModbusMaster node5;

ModbusMaster node6;
ModbusMaster node7;
ModbusMaster node8;

SoftwareSerial RS485Serial(EIA485_RX, EIA485_TX); // RX, TX

void preTransmission() {                      //enable an RS485 transceiver'sDriver Enable pin, and optionally disable its Receiver Enable pin.
  digitalWrite(MAX485_TX_ENABLE, true);
}
void postTransmission() {                     //enable an RS485 transceiver'sReceiver Enable pin, and disable its Driver Enable pin
  digitalWrite(MAX485_TX_ENABLE, false);
}

#define CONFIG_SIZE 1024
#define REPORT_INTERVAL 15000
#define MESSAGE_ID 350413 //Message defined in the platform
/**
 * ***** PLEASE CHANGE THE BELOW SETTINGS MATCHING YOUR ENVIRONMENT *****
*/
#define DEF_WIFI_SSID "MAP"  //Your WiFi SSID
#define DEF_WIFI_PSK "9750216721" //Your WiFi password
#define DEF_DOMAIN_KEY "EEFRJGPUUH" //your DOMAIN Key
#define DEF_API_KEY "DCD095nfvcQd" //Your API Key
#define DEF_DEVICE_MODEL "FBNALAPMS" //Your device model
#define DEF_FIRMWARE_VER "1.0.0" //Your firmware version

BoodskapTransceiver Boodskap(UDP); //MQTT, UDP, HTTP
uint32_t lastReport = 0;

void sendReading();
void sendReading1();
void sendReading2();
void handleData(byte* data);
bool handleMessage(uint16_t messageId, JsonObject& header, JsonObject& data);

void gprs(float k, float k1, float k2, float k3, float k4, float k5, float k6, float k7, float k8, float k9, float k10, float k11, float k12, float k13, float k14, float k15, float k16, float k17, float k18, float k19, float k20, float k21, float k22, float k23);
void gprs2();
String gprs_response;
char gprs_resp[100];
int meter_id;
//int m_st;
void setup() {
  pinMode(MAX485_TX_ENABLE, OUTPUT);
  digitalWrite(MAX485_TX_ENABLE, false);

  RS485Serial.begin(9600);
  Serial.begin(9600);
  Serial.println("My Little SCADA System");
  Boodskap.setHandleData(&handleData);
  Boodskap.setHandleMessage(&handleMessage);

  node.begin(2, RS485Serial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  node1.begin(2, RS485Serial);
  node1.preTransmission(preTransmission);
  node1.postTransmission(postTransmission);
  node2.begin(2, RS485Serial);
  node2.preTransmission(preTransmission);
  node2.postTransmission(postTransmission);

  node3.begin(3, RS485Serial);
  node3.preTransmission(preTransmission);
  node3.postTransmission(postTransmission);
  node4.begin(3, RS485Serial);
  node4.preTransmission(preTransmission);
  node4.postTransmission(postTransmission);
  node5.begin(3, RS485Serial);
  node5.preTransmission(preTransmission);
  node5.postTransmission(postTransmission);


  node6.begin(4, RS485Serial);
  node6.preTransmission(preTransmission);
  node6.postTransmission(postTransmission);
  node7.begin(4, RS485Serial);
  node7.preTransmission(preTransmission);
  node7.postTransmission(postTransmission);
  node8.begin(4, RS485Serial);
  node8.preTransmission(preTransmission);
  node8.postTransmission(postTransmission);

  

  StaticJsonBuffer<CONFIG_SIZE> buffer;
  JsonObject &config = buffer.createObject();

  config["ssid"] = DEF_WIFI_SSID;
  config["psk"] = DEF_WIFI_PSK;
  config["domain_key"] = DEF_DOMAIN_KEY;
  config["api_key"] = DEF_API_KEY;
  config["dev_model"] = DEF_DEVICE_MODEL;
  config["fw_ver"] = DEF_FIRMWARE_VER;
  config["dev_id"] = DEF_DEVICE_MODEL + String(ESP.getChipId()); //Your unique device ID

  /**

     If you have setup your own Boodskap IoT Platform, then change the below settings matching your installation 31 48
     Leave it for default Boodskap IoT Cloud Instance
  */
  config["api_path"] = "http://api.boodskap.io"; //HTTP API Base Path Endpoint
  //config["api_fp"] = "B9:01:85:CE:E3:48:5F:5E:E1:19:74:CC:47:A1:4A:63:26:B4:CB:32"; //In case of HTTPS enter your server fingerprint (https://www.grc.com/fingerprints.htm)
  config["udp_host"] = "udp.boodskap.io"; //UDP Server IP
  config["udp_port"] = 5555; //UDP Server Port
  config["mqtt_host"] = "mqtt.boodskap.io"; //MQTT Server IP
  config["mqtt_port"] = 1883; //MQTT Server Port
  config["heartbeat"] = 45; //seconds
  Boodskap.setup(config);
}

void loop() {
  Boodskap.loop();
  sendReading();
  delay(10000);
  sendReading1();
  delay(10000);
  sendReading2();
}

void sendReading() {

  uint16_t result;
  uint16_t result1;
  uint16_t result2;
  uint16_t result3;
  uint16_t result4;
  uint16_t result5;
  uint16_t result6;
  uint16_t result7;
  uint16_t result8;

  Serial.read();
  while (Serial.available()) {
    gprs_response = Serial.readString();
    gprs_response.toCharArray(gprs_resp, 100);
  }
  if (strstr(gprs_resp, "RESET")) {
    Serial.write("AT+CSMP=17,167,0,0\r\n");
    delay(2000);
    Serial.write("AT+CMGF=1\r\n");
    delay(2000);
    Serial.write("AT+CMGS=\"9750216721\"\r\n"); // Replace x with mobile number
    delay(2000);
    Serial.print("MODULE RESET AT NALA");// The SMS text you want to send
    delay(2000);
    Serial.write(0X1A);
    delay(1000);
    Serial.write(0X0D);
    delay(2000);
    ESP.restart();
  }
  StaticJsonBuffer<1024> buffer;
  JsonObject &data = buffer.createObject();

  Serial.println("RESULT READING");
     result = node.readHoldingRegisters(4097, 33);//40000 40100 0X9CA4
     delay(1000);
     Serial.println("Node Readed");
     result1 = node1.readDiscreteInputs(0, 4);
     Serial.println("Node1 Readed");
     delay(1000);
     result2 = node2.readHoldingRegisters(16384, 25);
     Serial.println("Node2 Readed");
     delay(1000);

     if ((result == node.ku8MBSuccess)||(result1 == node1.ku8MBSuccess)||(result2 == node2.ku8MBSuccess))
     {
      Serial.println("Getting_Data");
      float x =(node.getResponseBuffer(4096));
      Serial.println(x);
      float y = (x/100);
      Serial.println(y);
      float x1 = node.getResponseBuffer(4097);
      float y1 = (x1/100);
      Serial.println(y1);
      float x2 = node.getResponseBuffer(4098);
      float y2 = (x2/100);
      Serial.println(y2);
      float x3 = (y1+y2+y);
      float y3 = (x3/3);
      Serial.println(y3);
      float x4 = node.getResponseBuffer(4100);
      float y4 = (x4/100);
      Serial.println(y4);
      float x5 = node.getResponseBuffer(4102);
      float y5 = (x5/100);
      Serial.println(y5);
      float x6 = node.getResponseBuffer(4103);
      float y6 = (x6/100);
      Serial.println(y6);
      float x7 = (y4+y5+y6);
      float y7 = (x7/3);
      Serial.println(y7);
      float x8 = node.getResponseBuffer(4105);
      float y8 = ((x8*240)/1000);
      Serial.println(y8);
      float x9 = node.getResponseBuffer(4106);
      float y9 = ((x9*240)/1000);
      Serial.println(y9);
      float x10 = node.getResponseBuffer(4107);
      float y10 = ((x10*240)/1000);
      Serial.println(y10);
      float x11 = (y8+y9+y10);
      float y11 = (x11/3);
      Serial.println(y11);
      float x12 = node.getResponseBuffer(4109);
      float y12 = ((x12*240)/1000);
      Serial.println(y12);
      float x13 = node.getResponseBuffer(4110);
      float y13 = ((x13*240)/1000);
      Serial.println(y13);
      float x14 = node.getResponseBuffer(4111);
      float y14 = ((x14*240)/1000);
      Serial.println(y14);
      float x15 = (y12+y13+y14);
      float y15 = (x15);
      Serial.println(y15);
      float x16 = node.getResponseBuffer(4124);
      float y16 = (x16/100);
      Serial.println(y16);
      float x17 = node.getResponseBuffer(4125);
      float y17 = (x17/100);
      Serial.println(y17);
      delay(1000);
      float y18 = node1.getResponseBuffer(0);
      float x19 =node2.getResponseBuffer(16385);
      float y19 = ((x19*240)/10);
      Serial.print("KWH: ");
      Serial.println(y19);
      float x20 = node.getResponseBuffer(4118);
      float y20 = ((x20*240)/1000);
      Serial.println(y20);
      float x21 = node.getResponseBuffer(4119);
      float y21 = ((x21*240)/1000);
      Serial.println(y21);
      float x22 = node.getResponseBuffer(4120);
      float y22 = ((x22*240)/1000);
      Serial.println(y22);
      float x23 = (y20+y21+y22);
      float y23 = (x15);
      Serial.println(y23);
     

  


  data["rv"] = y;
  data["bv"] = y1;
  data["yv"] = y2;
  data["tv"] = y3;
  data["rbv"] = y4;
  data["byv"] = y5;
  data["yrv"] = y6;
  data["tpv"] = y7;
  data["rc"] = y8;
  data["bc"] = y9;
  data["yc"] = y10;
  data["tc"] = y11;
  data["rp"] = y12;
  data["bp"] = y13;
  data["yp"] = y14;
  data["tp"] = y15;
  data["pf"] = y16;
  data["fre"] = y17;
 // data["gen"] = y18;
  data["kwh"] = y19;
  data["rkva"] = y20;
  data["bkva"] = y21;
  data["ykva"] = y22;
  data["tkva"] = y23;
  meter_id=101;
 // data["meter_id"] = 350403;
  if (y18 == 1)
  {
    data["gen"] = 1;
    data["gen_id"] = GEN_ID;;
  }
  else if (y18 == 2)
  {
    data["gen"] = 1;
    data["gen_id"] = GEN_ID1;;
  }
  else
  {
    data["gen"] = 0;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Boodskap.sendMessage(MESSAGE_ID, data);
    delay(50000);
  }
  else {
    Serial.write("AT+CSMP=17,167,0,0\r\n");
    delay(2000);
    Serial.write("AT+CMGF=1\r\n");
    delay(2000);
    Serial.write("AT+CMGS=\"9750216721\"\r\n"); // Replace x with mobile number
    delay(2000);
    Serial.print("WIFI CUT DOWN AT NALA");// The SMS text you want to send
    delay(2000);
    Serial.write(0X1A);
    delay(1000);
    Serial.write(0X0D);
    delay(2000);
    gprs(y, y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, y11, y12, y13, y14, y15, y16, y17, y18, y19, y20, y21, y22, y23);
  }
  }
     else{
      Serial.write("AT+CSMP=17,167,0,0\r\n");
      delay(2000);
      Serial.write("AT+CNMI=2,2,0,0,0\r\n");
      delay(2000);
      Serial.write("AT+CMGF=1\r\n");
      delay(2000);
      Serial.write("AT+CMGS=\"9750216721\"\r\n"); // Replace x with mobile number
      delay(2000);
      Serial.print("DATA NOT AVAILABLE AT NALA EB");// The SMS text you want to send
      delay(2000);
      Serial.write(0X1A);
      delay(1000);
      Serial.write(0X0D);
      delay(2000);
      data["m_st"]= M_ST;;
      meter_id = 101;
      gprs2();
      }
}


void sendReading1() {

  uint16_t result3;
  uint16_t result4;
  uint16_t result5;

  Serial.read();
while (Serial.available()) {
    gprs_response = Serial.readString();
    gprs_response.toCharArray(gprs_resp, 100);
  }
  if (strstr(gprs_resp, "RESET")) {
    Serial.write("AT+CSMP=17,167,0,0\r\n");
    delay(2000);
    Serial.write("AT+CMGF=1\r\n");
    delay(2000);
    Serial.write("AT+CMGS=\"9750216721\"\r\n"); // Replace x with mobile number
    delay(2000);
    Serial.print("MODULE RESET AT NALA");// The SMS text you want to send
    delay(2000);
    Serial.write(0X1A);
    delay(1000);
    Serial.write(0X0D);
    delay(2000);
    ESP.restart();
  }
  StaticJsonBuffer<1024> buffer;
  JsonObject &data = buffer.createObject();

  Serial.println("RESULT READING for METER 2");
     
     result3 = node3.readHoldingRegisters(4097, 33);//40000 40100 0X9CA4
     delay(1000);
     Serial.println("Node3 Readed");
     result4 = node4.readDiscreteInputs(0, 4);
     Serial.println("Node4 Readed");
     delay(1000);
     result5 = node5.readHoldingRegisters(16384, 25);
     Serial.println("Node5 Readed");
     delay(1000);
  
     if ((result3 == node3.ku8MBSuccess)||(result4 == node4.ku8MBSuccess)||(result5 == node5.ku8MBSuccess))
     {
      Serial.println("Meter2 Getting_Data");
      float x =(node3.getResponseBuffer(4096));
      Serial.println(x);
      float y = (x/100);
      Serial.println(y);
      float x1 = node3.getResponseBuffer(4097);
      float y1 = (x1/100);
      Serial.println(y1);
      float x2 = node3.getResponseBuffer(4098);
      float y2 = (x2/100);
      Serial.println(y2);
      float x3 = (y1+y2+y);
      float y3 = (x3/3);
      Serial.println(y3);
      float x4 = node3.getResponseBuffer(4100);
      float y4 = (x4/100);
      Serial.println(y4);
      float x5 = node3.getResponseBuffer(4102);
      float y5 = (x5/100);
      Serial.println(y5);
      float x6 = node3.getResponseBuffer(4103);
      float y6 = (x6/100);
      Serial.println(y6);
      float x7 = (y4+y5+y6);
      float y7 = (x7/3);
      Serial.println(y7);
      float x8 = node3.getResponseBuffer(4105);
      float y8 = ((x8*50)/1000);
      Serial.println(y8);
      float x9 = node3.getResponseBuffer(4106);
      float y9 = ((x9*50)/1000);
      Serial.println(y9);
      float x10 = node3.getResponseBuffer(4107);
      float y10 = ((x10*50)/1000);
      Serial.println(y10);
      float x11 = (y8+y9+y10);
      float y11 = (x11/3);
      Serial.println(y11);
      float x12 = node3.getResponseBuffer(4109);
      float y12 = ((x12*50)/1000);
      Serial.println(y12);
      float x13 = node3.getResponseBuffer(4110);
      float y13 = ((x13*50)/1000);
      Serial.println(y13);
      float x14 = node3.getResponseBuffer(4111);
      float y14 = ((x14*50)/1000);
      Serial.println(y14);
      float x15 = (y12+y13+y14);
      float y15 = (x15);
      Serial.println(y15);
      float x16 = node3.getResponseBuffer(4124);
      float y16 = (x16/100);
      Serial.println(y16);
      float x17 = node3.getResponseBuffer(4125);
      float y17 = (x17/100);
      Serial.println(y17);
      delay(1000);
      float y18 = node4.getResponseBuffer(0);
      float x19 =node5.getResponseBuffer(16385);
      float y19 = ((x19*50)/10);
      Serial.print("KWH: ");
      Serial.println(y19);
      float x20 = node3.getResponseBuffer(4118);
      float y20 = ((x20*50)/1000);
      Serial.println(y20);
      float x21 = node3.getResponseBuffer(4119);
      float y21 = ((x21*50)/1000);
      Serial.println(y21);
      float x22 = node3.getResponseBuffer(4120);
      float y22 = ((x22*50)/1000);
      Serial.println(y22);
      float x23 = (y20+y21+y22);
      float y23 = (x15);
      Serial.println(y23);

  data["rv"] = y;
  data["bv"] = y1;
  data["yv"] = y2;
  data["tv"] = y3;
  data["rbv"] = y4;
  data["byv"] = y5;
  data["yrv"] = y6;
  data["tpv"] = y7;
  data["rc"] = y8;
  data["bc"] = y9;
  data["yc"] = y10;
  data["tc"] = y11;
  data["rp"] = y12;
  data["bp"] = y13;
  data["yp"] = y14;
  data["tp"] = y15;
  data["pf"] = y16;
  data["fre"] = y17;
 // data["gen"] = y18;
  data["kwh"] = y19;
  data["rkva"] = y20;
  data["bkva"] = y21;
  data["ykva"] = y22;
  data["tkva"] = y23;
    meter_id=102;
  data["meter_id"] = meter_id;
  if (y18 == 1)
  {
    data["gen"] = 1;
    data["gen_id"] = GEN_ID;;
  }
  else if (y18 == 2)
  {
    data["gen"] = 1;
    data["gen_id"] = GEN_ID1;;
  }
  else
  {
    data["gen"] = 0;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Boodskap.sendMessage(MESSAGE_ID, data);
    delay(50000);
  }
  else {
    Serial.write("AT+CSMP=17,167,0,0\r\n");
    delay(2000);
    Serial.write("AT+CMGF=1\r\n");
    delay(2000);
//    Serial.write("AT+CMGS=\"9750216721\"\r\n"); // Replace x with mobile number
//    delay(2000);
//    Serial.print("WIFI CUT DOWN AT NALA");// The SMS text you want to send
//    delay(2000);
//    Serial.write(0X1A);
//    delay(1000);
//    Serial.write(0X0D);
//    delay(2000);
    gprs(y, y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, y11, y12, y13, y14, y15, y16, y17, y18, y19, y20, y21, y22, y23);
  }
  }
     else{
      Serial.write("AT+CSMP=17,167,0,0\r\n");
      delay(2000);
      Serial.write("AT+CNMI=2,2,0,0,0\r\n");
      delay(2000);
      Serial.write("AT+CMGF=1\r\n");
      delay(2000);
//      Serial.write("AT+CMGS=\"9750216721\"\r\n"); // Replace x with mobile number
//      delay(2000);
//      Serial.print("DATA NOT AVAILABLE AT NALA DG160");// The SMS text you want to send
//      delay(2000);
//      Serial.write(0X1A);
//      delay(1000);
//      Serial.write(0X0D);
//      delay(2000);
      data["m_st"]= M_ST;;
       meter_id = 102;
      gprs2();}
}

void sendReading2() {

  uint16_t result6;
  uint16_t result7;
  uint16_t result8;

  Serial.read();
while (Serial.available()) {
    gprs_response = Serial.readString();
    gprs_response.toCharArray(gprs_resp, 100);
  }
  if (strstr(gprs_resp, "RESET")) {
    Serial.write("AT+CSMP=17,167,0,0\r\n");
    delay(2000);
    Serial.write("AT+CMGF=1\r\n");
    delay(2000);
    Serial.write("AT+CMGS=\"9750216721\"\r\n"); // Replace x with mobile number
    delay(2000);
    Serial.print("MODULE RESET AT NALA");// The SMS text you want to send
    delay(2000);
    Serial.write(0X1A);
    delay(1000);
    Serial.write(0X0D);
    delay(2000);
    ESP.restart();
  }
  StaticJsonBuffer<1024> buffer;
  JsonObject &data = buffer.createObject();

  Serial.println("RESULT READING for METER 3");
     
     result6 = node6.readHoldingRegisters(4097, 33);//40000 40100 0X9CA4
     delay(1000);
     Serial.println("Node3 Readed");
     result7 = node7.readDiscreteInputs(0, 4);
     Serial.println("Node4 Readed");
     delay(1000);
     result8 = node8.readHoldingRegisters(16384, 25);
     Serial.println("Node5 Readed");
     delay(1000);
  
     if ((result6 == node6.ku8MBSuccess)||(result7 == node7.ku8MBSuccess)||(result8 == node8.ku8MBSuccess))
     {
      Serial.println("Meter2 Getting_Data");
      float x =(node6.getResponseBuffer(4096));
      Serial.println(x);
      float y = (x/100);
      Serial.println(y);
      float x1 = node6.getResponseBuffer(4097);
      float y1 = (x1/100);
      Serial.println(y1);
      float x2 = node6.getResponseBuffer(4098);
      float y2 = (x2/100);
      Serial.println(y2);
      float x3 = (y1+y2+y);
      float y3 = (x3/3);
      Serial.println(y3);
      float x4 = node6.getResponseBuffer(4100);
      float y4 = (x4/100);
      Serial.println(y4);
      float x5 = node6.getResponseBuffer(4102);
      float y5 = (x5/100);
      Serial.println(y5);
      float x6 = node6.getResponseBuffer(4103);
      float y6 = (x6/100);
      Serial.println(y6);
      float x7 = (y4+y5+y6);
      float y7 = (x7/3);
      Serial.println(y7);
      float x8 = node6.getResponseBuffer(4105);
      float y8 = ((x8*160)/1000);
      Serial.println(y8);
      float x9 = node6.getResponseBuffer(4106);
      float y9 = ((x9*160)/1000);
      Serial.println(y9);
      float x10 = node6.getResponseBuffer(4107);
      float y10 = ((x10*160)/1000);
      Serial.println(y10);
      float x11 = (y8+y9+y10);
      float y11 = (x11/3);
      Serial.println(y11);
      float x12 = node6.getResponseBuffer(4109);
      float y12 = ((x12*160)/1000);
      Serial.println(y12);
      float x13 = node6.getResponseBuffer(4110);
      float y13 = ((x13*160)/1000);
      Serial.println(y13);
      float x14 = node6.getResponseBuffer(4111);
      float y14 = ((x14*160)/1000);
      Serial.println(y14);
      float x15 = (y12+y13+y14);
      float y15 = (x15);
      Serial.println(y15);
      float x16 = node6.getResponseBuffer(4124);
      float y16 = (x16/100);
      Serial.println(y16);
      float x17 = node6.getResponseBuffer(4125);
      float y17 = (x17/100);
      Serial.println(y17);
      delay(1000);
      float y18 = node7.getResponseBuffer(0);
      float x19 =node8.getResponseBuffer(16385);
      float y19 = ((x19*160)/10);
      Serial.print("KWH: ");
      Serial.println(y19);
      float x20 = node6.getResponseBuffer(4118);
      float y20 = ((x20*160)/1000);
      Serial.println(y20);
      float x21 = node6.getResponseBuffer(4119);
      float y21 = ((x21*160)/1000);
      Serial.println(y21);
      float x22 = node6.getResponseBuffer(4120);
      float y22 = ((x22*160)/1000);
      Serial.println(y22);
      float x23 = (y20+y21+y22);
      float y23 = (x15);
      Serial.println(y23);


  data["rv"] = y;
  data["bv"] = y1;
  data["yv"] = y2;
  data["tv"] = y3;
  data["rbv"] = y4;
  data["byv"] = y5;
  data["yrv"] = y6;
  data["tpv"] = y7;
  data["rc"] = y8;
  data["bc"] = y9;
  data["yc"] = y10;
  data["tc"] = y11;
  data["rp"] = y12;
  data["bp"] = y13;
  data["yp"] = y14;
  data["tp"] = y15;
  data["pf"] = y16;
  data["fre"] = y17;
 // data["gen"] = y18;
  data["kwh"] = y19;
  data["rkva"] = y20;
  data["bkva"] = y21;
  data["ykva"] = y22;
  data["tkva"] = y23;
    meter_id=103;
//    m_st = 0;
  data["meter_id"] = meter_id;
 // data["m_st"] = m_st;
  
  if (y18 == 1)
  {
    data["gen"] = 1;
    data["gen_id"] = GEN_ID;;
  }
  else if (y18 == 2)
  {
    data["gen"] = 1;
    data["gen_id"] = GEN_ID1;;
  }
  else
  {
    data["gen"] = 0;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Boodskap.sendMessage(MESSAGE_ID, data);
    delay(50000);
  }
  else {
    Serial.write("AT+CSMP=17,167,0,0\r\n");
    delay(2000);
    Serial.write("AT+CMGF=1\r\n");
    delay(2000);
    Serial.write("AT+CMGS=\"9750216721\"\r\n"); // Replace x with mobile number
    delay(2000);
    Serial.print("WIFI CUT DOWN AT NALA");// The SMS text you want to send
    delay(2000);
    Serial.write(0X1A);
    delay(1000);
    Serial.write(0X0D);
    delay(2000);
    gprs(y, y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, y11, y12, y13, y14, y15, y16, y17, y18, y19, y20, y21, y22, y23);
  }
  }
     else{
      Serial.write("AT+CSMP=17,167,0,0\r\n");
      delay(2000);
      Serial.write("AT+CNMI=2,2,0,0,0\r\n");
      delay(2000);
      Serial.write("AT+CMGF=1\r\n");
      delay(2000);
      Serial.write("AT+CMGS=\"9750216721\"\r\n"); // Replace x with mobile number
      delay(2000);
      Serial.print("DATA NOT AVAILABLE AT NALA DG82");// The SMS text you want to send
      delay(2000);
      Serial.write(0X1A);
      delay(1000);
      Serial.write(0X0D);
      delay(2000);
      data["m_st"]= M_ST;;
       meter_id = 103;
      gprs2();
     }
}

void gprs(float k, float k1, float k2, float k3, float k4, float k5, float k6, float k7, float k8, float k9, float k10, float k11, float k12, float k13, float k14, float k15, float k16, float k17, float k18, float k19, float k20, float k21, float k22, float k23) {
  StaticJsonBuffer<1024> buffer;
  JsonObject &root = buffer.createObject();
  JsonObject &header = root.createNestedObject("header");
  JsonObject &data = root.createNestedObject("data");
  header["key"] = "EEFRJGPUUH";
  header["api"] = "DCD095nfvcQd";
  header["did"] = DEF_DEVICE_MODEL + String(ESP.getChipId());
  header["dmdl"] = "FBNALAPMS";
  header["fwver"] = "1.0.0";
  header["mid"] = 350413;
  data["rv"] = k;;
  data["bv"] = k1;;
  data["yv"] = k2;;
  data["tv"] = k3;;
  data["rbv"] = k4;;
  data["byv"] = k5;;
  data["yrv"] = k6;;
  data["tpv"] = k7;;
  data["rc"] = k8;;
  data["bc"] = k9;;
  data["yc"] = k10;;
  data["tc"] = k11;;
  data["rp"] = k12;;
  data["bp"] = k13;;
  data["yp"] = k14;;
  data["tp"] = k15;;
  data["pf"] = k16;;
  data["fre"] = k17;;
  data["gen"] = k18;;
  data["kwh"] = k19;;
  data["rkva"] = k20;;
  data["bkva"] = k21;;
  data["ykva"] = k22;;
  data["tkva"] = k23;;
  data["meter_id"] = meter_id;;
  if (k18 == 1)
  {
    data["gen"] = 1;
    data["gen_id"] = GEN_ID;;
  }
  else if (k18 == 2)
  {
    data["gen"] = 1;
    data["gen_id"] = GEN_ID1;;
  }
  else
  {
    data["gen"] = 0;
  }
  delay(10000);
  size_t len = root.measureLength();
  delay(5000);
  String send2 = String(len + 3);
  Serial.write("AT\r\n");
  delay(5000);
  Serial.write("AT+CIPMODE=0\r\n");
  delay(2000);
  Serial.write("AT+CIPMUX=0\r\n");
  delay(2000);
  Serial.write("AT+CGATT=1\r\n");
  delay(2000);
  Serial.write("AT+CREG?\r\n");
  delay(2000);
  Serial.write("AT+CSTT=\"airtelgprs.com\",\"\",\"\"\r\n");
  delay(1000);
  Serial.write("AT+CIICR\r\n");
  delay(5000);
  Serial.write("AT+CIFSR\r\n");
  delay(1000);
  Serial.write("AT+CIPSTART=\"UDP\",\"udp.boodskap.io\",\"5555\"\r\n");
  delay(5000);
  Serial.write("AT+CIPSEND=");
  delay(1000);
  Serial.print(send2);
  delay(1000);
  Serial.print("\r\n");
  delay(1000);
  root.printTo(Serial);
  delay(5000);
  Serial.write(0X1A);
  delay(1000);
  Serial.write(0X0D);
  delay(1000);
  Serial.write("AT+CIPSHUT\r\n");
  delay(15000);
}


void gprs2() {
  StaticJsonBuffer<1024> buffer;
  JsonObject &root = buffer.createObject();
  JsonObject &header = root.createNestedObject("header");
  JsonObject &data = root.createNestedObject("data");
  header["key"] = "EEFRJGPUUH";
  header["api"] = "DCD095nfvcQd";
  header["did"] = DEF_DEVICE_MODEL + String(ESP.getChipId());
  header["dmdl"] = "FBNALAPMS";
  header["fwver"] = "1.0.0";
  header["mid"] = 350413;
  data["meter_id"] = meter_id;;
  data["m_st"]= M_ST;;
  delay(10000);
  size_t len = root.measureLength();
  delay(5000);
  String send2 = String(len + 3);
  Serial.write("AT\r\n");
  delay(5000);
  Serial.write("AT+CIPMODE=0\r\n");
  delay(2000);
  Serial.write("AT+CIPMUX=0\r\n");
  delay(2000);
  Serial.write("AT+CGATT=1\r\n");
  delay(2000);
  Serial.write("AT+CREG?\r\n");
  delay(2000);
  Serial.write("AT+CSTT=\"airtelgprs.com\",\"\",\"\"\r\n");
  delay(1000);
  Serial.write("AT+CIICR\r\n");
  delay(5000);
  Serial.write("AT+CIFSR\r\n");
  delay(1000);
  Serial.write("AT+CIPSTART=\"UDP\",\"udp.boodskap.io\",\"5555\"\r\n");
  delay(5000);
  Serial.write("AT+CIPSEND=");
  delay(1000);
  Serial.print(send2);
  delay(1000);
  Serial.print("\r\n");
  delay(1000);
  root.printTo(Serial);
  delay(5000);
  Serial.write(0X1A);
  delay(1000);
  Serial.write(0X0D);
  delay(1000);
  Serial.write("AT+CIPSHUT\r\n");
  delay(15000);
}

void handleData(byte* data) {
  //handle raw data from the platform
}

bool handleMessage(uint16_t messageId, JsonObject& header, JsonObject& data) {
  //handle JSON commands from the platform
  return false; //return true if you have successfully handled the message
}
