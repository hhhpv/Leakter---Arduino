#include <SoftwareSerial.h>
// replace with your channel's thingspeak API key
String apiKey = "your_api_key";
float sensor = A0;
int gas_value;
const int buzzer = 13;
SoftwareSerial mySerial(9, 10);
// connect 9 to TX of Esp
// connect 10 to RX of Esp
SoftwareSerial ser(3,5); // TX,RX of Esp
// this runs once
void setup() {                
  // enable debug serial
  //sets the baud rate to 115200
  Serial.begin(115200);
  mySerial.begin(9600); 
  // enable software serial
  //sets the baud rate to 9600
  ser.begin(115200);
  ser.println("AT+RST");   // reset ESP8266
  pinMode(buzzer, OUTPUT);
  
}
// this runs again and again 
void loop() {

  
  gas_value = analogRead(sensor);
  

  if(gas_value>=300.0)
  {  
    tone(buzzer, 1000);
    String a="gas detected";
    
    
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  ser.println(cmd);
   
  if(ser.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(gas_value);
  getStr +="&field2=0";
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  ser.println(cmd);

  if(ser.find(">"))
  {
    ser.print(getStr);
     mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
     delay(1000);  // Delay of 1 second
     mySerial.println("AT+CMGS=\" your_phone_number \"\r"); // Replace x with mobile number
     delay(1000);
     mySerial.println("GSM ALERT: GAS LEAKAGE (channel updated..check app)");// The SMS text you want to send
     delay(100);
     mySerial.println((char)26);// ASCII code of CTRL+Z for saying the end of sms to  the module 
      delay(1000);
  }
  else
  {
    ser.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
     mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
     delay(1000);  // Delay of 1 second
     mySerial.println("AT+CMGS=\"your_phone_number\"\r"); // Replace x with mobile number
     delay(1000);
     mySerial.println("GSM ALERT: GAS LEAKAGE (No Wifi)");// The SMS text you want to send
     delay(100);
     mySerial.println((char)26);// ASCII code of CTRL+Z for saying the end of sms to  the module 
      delay(1000);;
  }
  }
  
   noTone(buzzer);
  // thingspeak needs 15 sec delay between updates
  delay(3000);  
}



