//==============================GLOBAL VARIABLE===============================================
const String SerialNumber = "889880818820"; //serial number mesin
const String code = "BC00"; //kode mesin
char c; //I2C receive byte
String cc; //I2C buffer byte
String ccc; //LCD print barcode

const String password = "password";
const String username = "nicabreon";
const String token = "bmljYWJyZW9uOnBhc3N3b3Jk";
//==========================================================================================

//=================B64==================================
#include "Base64.h"

//================================WIFI CILENT & HTTP CLIENT==========================
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
const char *ssid =  "meAdnanIoT";     // replace with your wifi ssid and wpa2 key
const char *pass =  "58587272";
String terminal = "http://103.140.90.58:8008/api/process/barcode/";      //"http://103.140.90.10:9900/data/";
String url;
String payload;
WiFiClient client;
HTTPClient http;  //Declare an object of class HTTPClient
int heap = 0;  //check heap memmory
//================================================================================



//=================================SIGNAL REQUEST==================================
const int SR = D5; //D5 input
int LED = 2;
int sign = 0;
//===================================================================================


//==============================================TWI LCD====================================
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

int i = 0;
int j = 0;
int k = 0;
int delayTime2 = 350; // Delay between shifts
//====================================================================================


//==============================================JSON====================================
#include <ArduinoJson.h>
StaticJsonBuffer<500> jsonBuffer; //1000 byte data json simpan pada stack secara statis.
//DynamicJsonBuffer  jsonBuffer(200);  //200 byte data simpan pada head secara dinamis.

//========================================================================================



void setup() {
//======================================TWI BUS & SERIAL BEGIN==============================
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output
  delay(1000);
  Serial.println();
  Serial.println("Master begin");
  Serial.print("Machine SN:");
  Serial.println(SerialNumber);
  Serial.print("Scanner Code:");
  Serial.println(code);
  Serial.println("=====================================================");
//====================================================================================


//====================================TWI LCD BEGIN===============================================
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.clear();
  
  lcd.setCursor(2,0);
  lcd.print(code);
  lcd.printf(" -READY-");

  lcd.setCursor(0,1);
  lcd.printf("Connecting WiFi ");
  delay(500);
  lcd.setCursor(0,1);
  lcd.printf("                ");
  lcd.setCursor(0,1);
  lcd.print(ssid);
  
//============================================================================================



//=======================================WIFI BEGIN==================================================
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
            lcd.setCursor(0,1);
            lcd.printf("WiFi disconnect!");       
     }
     
  Serial.println();
  Serial.println("WiFi connected      ");
  lcd.setCursor(0,1);
  lcd.printf("WiFi connected   ");
  delay(1000);
//=======================================================================================================


//=================================SIGNAL REQUEST TWI==================================
pinMode(SR, INPUT);
pinMode (LED, OUTPUT);
//=======================================================================================================


}


void scrollInFromRight (int line, char str1[]) {
  i = strlen(str1);
  for (j = 16; j >= 0; j--) {
    lcd.setCursor(0, line);
    for (k = 0; k <= 15; k++) {
      lcd.print(" "); // Clear line
    }
  lcd.setCursor(j, line);
  lcd.print(str1);
  delay(delayTime2);
}
}



void scrollInFromLeft (int line, char str1[]) {
  i = 40 - strlen(str1);
  line = line - 1;
  for (j = i; j <= i + 16; j++) {
    for (k = 0; k <= 15; k++) {
      lcd.print(" "); // Clear line
  }
  lcd.setCursor(j, line);
  lcd.print(str1);
  delay(delayTime2);
}

}



void post(){
//===========================================Json Formatting======================================================================
   while (!Serial) continue;

  JsonObject& root = jsonBuffer.createObject(); //object json tunggal
  root["station_code"] = "01";  //statiun pengecekan
  root["product_code"] = "ZY92710"; //kode barang
  root["workflow_code"] = "w-asm-ag3-001"; //flow produksi
  root["scanner_dev"] = (SerialNumber);//"xxxxxxxxxx"; //kode scanner 10 digit
  root["barcode"] = cc; //data barcode max 16 digit
  

  JsonArray& data = root.createNestedArray("status_condition"); //object json dengan array / class /libarary /multi
  data.add("OK");
//  data.add("NG1");
//  data.add("NG2");
//  data.add("NG3");
//  data.add("NG4");
  data.add("-");
  data.add("-");
  data.add("-");
  data.add("-");

  //root["notes"] = "apablia terdapat kategori NG pada QC";

  delay(1000);
//  Serial.println(); //ganti baris /new line
//  root.printTo(Serial);  //print json dengan format baris horizontal
  Serial.println("======================================================");
  Serial.println("Json Data:");
  //Serial.println(); //ganti baris /new line
  root.prettyPrintTo(Serial); //print json dengan format body
  Serial.println("====================================================");
  
//=================================================================================================================================

//=========================================Json Post===================================================================================
  
  lcd.setCursor(0,1);
  lcd.printf("Try to reach svr");

  Serial.println("Try to reach server");
  Serial.println("====================================================");
  
  while (WiFi.status() != WL_CONNECTED) 
       {
              Serial.println("Wifi Disconnect");
              lcd.setCursor(0,1);
              lcd.printf("WiFi disconnect!");
       } 



  while (WiFi.status() == WL_CONNECTED){ 

    lcd.setCursor(0,1);
    lcd.printf("Sending data....");
    
    //StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
    //JsonObject& JSONencoder = JSONbuffer.createObject();
  
    //JSONencoder["value"] = value_var;
    //JSONencoder[] = (root());
    char JSONmessageBuffer[600];
    //JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    root.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println("==================================================");
    Serial.println("Wifi Connected");
    Serial.println("Buffer Message:");  
    Serial.print(JSONmessageBuffer);
    Serial.println("==================================================");

    url = "";
    delay(100);
    url  = (terminal);
    
    //String auth = base64::encode(username + ":" + password);
    String gen = (username + ":" + password);  //make input from username and password
    char inputString[gen.length()+1]; //set the length of buffer same as length of input
    gen.toCharArray(inputString, sizeof(inputString)); //store input string to char array
    int inputStringLength = sizeof(inputString); //define length of data

    int encodedLength = Base64.encodedLength(inputStringLength); //set length data for B64
    char encodedString[encodedLength];
    Base64.encode(encodedString, inputString, inputStringLength);
    String auth = (encodedString);
    auth = auth.substring(0,(auth.length()-4));
    //Serial.print("Encoded string is:\t");
    //Serial.println(encodedString);
    
    

    Serial.println("Prepare to Sending data to:");
    Serial.println("==================================================");
    Serial.println("url+payload:");
    Serial.println(url);
    Serial.println("body:");
    Serial.println(JSONmessageBuffer);
    Serial.println("auth key(B64):");
    Serial.println(auth);
    Serial.println("Token key(B64):");
    Serial.println(token);
    Serial.println("==================================================");


    http.begin(url); //("103.140.90.58", 8008, "/api/process/barcode/");   //http.begin("IP address/ URL", port, "payload");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Basic bmljYWJyZW9uOnBhc3N3b3Jk"); //("Authorization", "Basic" + token); //("Authorization", "Basic" + auth);
    //auto httpCode = http.POST(JSONmessageBuffer);
    int httpCode = http.POST(JSONmessageBuffer);
    payload = "";
    
    if (httpCode > 0) {
      payload = http.getString();
  
      Serial.println("payload:");
      Serial.println("==================================================");
      Serial.print(payload);
      Serial.println();
      Serial.println("==================================================");
      
//      scrollInFromRight(0, "Line1 From Right"); test
//      scrollInFromLeft(1, "Line2 From Left."); //test
      char feedback[payload.length()];
      payload.toCharArray(feedback, sizeof(feedback));
      scrollInFromRight(0,feedback);

/*      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(payload);
      for (int i = 0; i < payload.length()+7 ; i++){
        lcd.scrollDisplayLeft();
        delay(300);
      }
 */
     
     } else {
      Serial.println("Error on HTTP request");
    }
    http.end();
    lcd.clear();
    break;
  }

  while (WiFi.status() != WL_CONNECTED){
      lcd.setCursor(0,1);
      lcd.printf("WiFi disconnect! "); 
      break;
  }
//======================================================================================================================================


//=====================================CHECK HEAP CAPCACITY==========================================================================
heap = NULL;
heap = ESP.getFreeHeap();

Serial.println("====================================================================================");
Serial.print("ESP.getFreeHeap(); ");
Serial.print(ESP.getFreeHeap());
Serial.println("byte");
Serial.println("====================================================================================");

//==================================================================================================================================
   
}

void loop() {
  lcd.setCursor(2,0);
  lcd.print(code);
  lcd.printf(" -READY-");
  if(WiFi.status() != WL_CONNECTED){
      lcd.setCursor(0,1);
      lcd.printf("WiFi disconnect! "); 
  }
  if(WiFi.status() == WL_CONNECTED){
      lcd.setCursor(0,1);
      lcd.printf("WiFi connected   "); 
  }
  
  sign = digitalRead(SR);
  delay(50);
  
  cc = "";
  ccc = "";

  if(sign == HIGH){
    digitalWrite(LED, HIGH);
    cc += (SerialNumber);
    cc += "-";
    
    Wire.requestFrom(1, 16);    // request 15 bytes from slave device #1
    while (Wire.available()) { // slave may send less than requested
      c = Wire.read(); // receive a byte as character
  //    Serial.println(c); 
  
      if(c != '\n'){
        if( c == '!'){
          c = '1';
        }
        else if( c == '@'){
          c = '2';
        }
        else if( c == '#'){
          c = '3';
        }
        else if( c == '$'){
          c = '4';
        }
        else if( c == '%'){
          c = '5';
        }
        else if( c == '^'){
          c = '6';
        }
        else if( c == '&'){
          c = '7';
        }
        else if( c == '*'){
          c = '8';
        }
        else if( c == '('){
          c = '9';
        }
        else if( c == ')'){
          c = '0';
        }
  
        cc += c;
        ccc += c;
      } else{
        break;
      }
    }
    cc += "-";
    cc += (code);
    Serial.println(cc);
  
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("-");
    lcd.print(code);
    lcd.printf(" READY-");
    lcd.setCursor(0,1);
    lcd.print(ccc);  
  
    post();
  }
  
  
  
  if(sign == LOW){
  digitalWrite(LED, LOW);
  //Serial.println("Waiting Data");
  //lcd.setCursor(0,1);
  //lcd.printf("Waiting..."); 
  }
  
  //Serial.print("check: ");
}
