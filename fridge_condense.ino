#include <Arduino.h>
#include "thermistor.h"
#include <ArduinoJson.h>
#include "networking.h"
#include <WiFi101.h>
extern WiFiSSLClient client;
using namespace std;
const int SMTP_PORT = 465;
const char* smtp_server = "smtp.gmail.com";
String mylogin = "bellkrish@gmail.com";
String mybody = "hello Krish";
String retval;


String echo(String request){ // send request and get response
  if (Serial) Serial.println(request);
client.println(request);
String retval = response();
  if (Serial) Serial.println();
return retval;
}



String response() { // get server response with timeout
  unsigned long starttime = millis();
  unsigned long lastbyte = starttime;
  boolean alldone = false;
  boolean gotbyte = false;


  while (client.available() > 0 ) {
    char c = client.read();
    retval = retval + String(c);
    if (Serial) Serial.write(c);
    lastbyte = millis();
    gotbyte = true;
  }
  // a pause of 500mS second probably indicates message over
  if (gotbyte && (millis() - lastbyte > 500)) alldone = true;

  return retval;
}



void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  while (!Serial) {
    ;
  }


  setup_wifi_network();
  client.flush();
}

String mysendname = "bellkrish@gmail.com";
String myfrom = "bellkrish@gmail.com";
String mysubject = "bellkrish@gmail.com";



void loop() {
  
  // put your main code here, to run repeatedly:
  float val = print_device();
  //if(val > 40){
  Serial.println("Trying to connect to mail server");

  client.connect(smtp_server, SMTP_PORT);
  Serial.println(response());
  client.flush();



  //Serial.println("This is your" + response());

  Serial.println("\nSending mail request");

  Serial.println("begin ehlo");
  echo("EHLO 192.168.2.151");
Serial.println("end ehlo");


  Serial.println("begin auth");
  echo("AUTH LOGIN");
  //while(response() != "334"){
   //echo(" ");
  //}

  Serial.println("end"); 

  echo("64bit encoded pass"); //removed from github for security
  echo("64bit encoded pass"); //removed from github for security
  echo("MAIL FROM:<bellkrish@gmail.com>");

  echo("RCPT TO:<bellkrish@gmail.com>");
  

  echo("DATA");

  echo("To: Krish");
  echo("From: Krish");
  echo("Subject: Oh hello");
  echo("Hello World");
  echo(".");
  echo("QUIT");
  

  //mybody = mybody  + "\n\n Posted by an Arduino WiFi101 shield.\n"; // add footer
  //noecho("From: \"" + mysendname + "\" <" + myfrom + ">");
  //noecho("Reply-To:<" + myfrom + ">");



  //noecho("Subject: " + mysubject + "\n");

  // noecho(mybody);
  // nb . to finish
  //echo("\r\n.\r\nQUIT");
  client.stop();
}
//}
//String m = String(val);
//  Serial.println(m);
//send_to_canopy(m);
//delay(5000);
