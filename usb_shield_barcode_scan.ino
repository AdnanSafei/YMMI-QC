#include <hidboot.h>
#include <hiduniversal.h>

#include <Wire.h>
String data;



const int Out = 7;


String DataBarcode;
bool x = false;

class KbdRptParser : public KeyboardReportParser
{
    void PrintKey(uint8_t mod, uint8_t key);

  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);

    void OnKeyDown  (uint8_t mod, uint8_t key);
    void OnKeyUp  (uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
};

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
  MODIFIERKEYS mod;
  *((uint8_t*)&mod) = m;
}

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  PrintKey(mod, key);
  uint8_t c = OemToAscii(mod, key);

  if (c)
    OnKeyPressed(c);
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {

  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  //Serial.print("UP ");
  //PrintKey(mod, key);
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
  digitalWrite(Out,HIGH);
  //Serial.println("Trigger Button Pressed");
  //Serial.print((char)key);
  if (key == 0x0D){
    x = true;
  }else{
    DataBarcode += (char)key;
    data += (char)key;
  }
}


USB Usb;
HIDUniversal Hid(&Usb);

KbdRptParser Prs;



/*// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}*/

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent(){
    Serial.print("Sending to Data to Master");
    //Wire.write("Halo sayang");
    //digitalWrite(Out,LOW);
    
   //char c;
   //c = "";
   //DataBarcode.toCharArray(c, sizeof(DataBarcode));
   //Serial.println(c);
   data += "\n";
   Wire.write(data.c_str()); // respond with message of x bytes
                                // as expected by master    
   digitalWrite(Out,LOW); 
   data = "";             
}




void setup()
{
  Wire.begin(1);                // join i2c bus with address #1
  Wire.onRequest(requestEvent); // register event

  
  pinMode(Out,OUTPUT);
  digitalWrite(Out,LOW);
  
  Serial.begin( 115200 );

  Serial.println("Start");

  if (Usb.Init() == -1){
    Serial.println("OSC did not start.");
  }else{
    Serial.println("USB Host Up");
  }

  delay( 200 );

  Hid.SetReportParser(0, &Prs);
}

void loop()
{
  Usb.Task();
  if (x){
    Serial.println(DataBarcode);
    x = false;
    DataBarcode = "";
    digitalWrite(Out,LOW); 
  }
}
