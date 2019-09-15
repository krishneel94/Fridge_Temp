#include <SPI.h>
#include <WiFi101.h>
#include <string.h>

#include "networking.h"
#define CANO_MSG_LEN 100
#include <elapsedMillis.h>
#define SECRET_SSID "Insanity1-Guest"
#define SECRET_PASS "fame4fifteen"
char ssid[] = SECRET_SSID;        // network SSID
char pass[] = SECRET_PASS;    // network password


int status = WL_IDLE_STATUS;
int start_time = 0;


WiFiSSLClient client;
String json_dump_beginning = "{\"message\": {\"timestamp\": ";
String json_dump_middle1 = ", \"messageType\": \"statistic\", \"statisticType\": \"fridge.project\", \"data\": {\"Temperature\": \"";
String json_dump_middle2 = "\", \"fow_rate\": ";
String json_dump_end = "}, \"id\": 123456789, \"tzoffset\": -5}, \"deviceId\": \"Fridge-3\"}";

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void set_time() {
  delay(4000);
  int tries = 0;
  int maxTries = 10;
  
  do {
    start_time = WiFi.getTime();
    tries++;
  }
  while ((start_time == 0) && (tries < maxTries));

  if (tries > maxTries) {
    Serial.println("NTP unreachable");
  } else {
    Serial.print("start time received: ");
    Serial.print(start_time);
  }
}

int get_time() {
  return start_time + millis()/1000;
}

void setup_wifi_network() {
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8, 7, 4, 2);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
    //print the status code
    Serial.print("Status: ");
    Serial.println(status);

    //check which status code it is
    switch (status) {
      case WL_CONNECTED:
        Serial.println("Connected");
        break;
      case WL_NO_SHIELD:
        Serial.println("No Shield");
        break;
      case WL_IDLE_STATUS:
        Serial.println("idle status");
        break;
      case WL_NO_SSID_AVAIL:
        Serial.println("no ssid avail");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("scan completed");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("connect failed");
        break;
      case WL_CONNECTION_LOST:
        Serial.println("Connection lost");
        break;
      case WL_DISCONNECTED:
        Serial.println("Disconnected");
        break;
    }
    delay(1000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();
  set_time();
}
char* send_to_canopy(String m) {
  int bracket_stack = 0;
  bool json_started = false;
  char msg_buf[CANO_MSG_LEN];
  int char_index = 0;
  
  //send message to canopy
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if(client.connect("leaf-uat.gocanopy.io",443)){
  Serial.println("connected to server");
  String message = json_dump_beginning + (start_time + millis()/1000) + json_dump_middle1 + m + '"' +json_dump_end;
  int s = message.length();
    // Make a HTTPS request:
    client.println("POST /events/v2/ HTTP/1.1");
    client.println("Host: leaf-uat.gocanopy.io");
    client.println("canopy-auth-req: banyan:olat1@locolabs.com:odGJlBg@1E85");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(message.length());
    client.println();
    client.println(message);
    Serial.println(message);
  }
  //  //read response back from canopy
  while (1) {
    
    while (client.available()) {
      char c = client.read();
    
      if (c == '}') {
        bracket_stack--;
      }

      if (c == '{') {
        bracket_stack++;
        if (bracket_stack == 1) {
          json_started = true;
        }
      }

      if (c == -1) {
        Serial.println();
        Serial.println("reached end of string, disconnecting from server.");
        client.stop();
        msg_buf[char_index] = '\0';
        char_index++;
        break;
      }

      Serial.write(c);
      if (json_started == true && char_index != 100 - 1 ) {
        msg_buf[char_index] = c;
        char_index++;
      }

      //this will not work if there are any additional characters before the start of the JSON message
      if (c == -1 || (bracket_stack == 0 && json_started == true)) {
        Serial.println();
        Serial.println("reached end of JSON, disconnecting from server.");
        client.stop();
        msg_buf[char_index] = '\0';
        char_index++;
        break;
      }
    }

    // if the server's disconnected, stop the client:
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting from server.");
      client.stop();
      break;
    }
  }

  return msg_buf;
}

////TODO: need to change return type from char* and add it as a parameter
//char* send_to_canopy() { //char* message
//  String message = json_dump_beginning + (start_time + millis()/1000) + json_dump_middle1 + "ON" + json_dump_middle2 + "1.15" + json_dump_end;
//  int bracket_stack = 0;
//  bool json_started = false;
//  char msg_buf[100];
//  int char_index = 0;
//  int length_stack = 0;
//  char* c_len = "Content-Length: 0";
//  //send message to canopy
//  Serial.println("\nStarting connection to server...");
//  // if you get a connection, report back via serial:
//  if (client.connect("leaf-uat.gocanopy.io", 443)) {
//    Serial.println("connected to server");
//    // Make a HTTPS request:
//    client.println("POST /events/v2/ HTTP/1.1");
//    client.println("Host: leaf-uat.gocanopy.io");
//    client.println("canopy-auth-req: banyan:leaf@loco.chargeit:JSfX9DttcFZ552rr!");
//    client.println("Content-Type: application/json");
//    client.print("Content-Length: ");
//    Serial.print("Content-Length: ");
//    client.println(message.length());
//    client.println();
//    Serial.println(message.length());
//    Serial.println(message);
//    client.println(message);
//  }
//
//  //read response back from canopy
//  while (1) {
//    
//    while (client.available()) {
//      char c = client.read();
//    
//      if (c == '}') {
//        bracket_stack--;
//      }
//
//      if (c == '{') {
//        bracket_stack++;
//        if (bracket_stack == 1) {
//          json_started = true;
//        }
//      }
//
//      if (c == -1) {
//        Serial.println();
//        Serial.println("reached end of string, disconnecting from server.");
//        client.stop();
//        msg_buf[char_index] = '\0';
//        char_index++;
//        break;
//      }
//
//      Serial.write(c);
//      if (json_started == true && char_index != 100 - 1 ) {
//        msg_buf[char_index] = c;
//        char_index++;
//      }
//
//      //this will not work if there are any additional characters before the start of the JSON message
//      if (c == -1 || (bracket_stack == 0 && json_started == true)) {
//        Serial.println();
//        Serial.println("reached end of JSON, disconnecting from server.");
//        client.stop();
//        msg_buf[char_index] = '\0';
//        char_index++;
//        break;
//      }
//    }
//
//    // if the server's disconnected, stop the client:
//    if (!client.connected()) {
//      Serial.println();
//      Serial.println("disconnecting from server.");
//      client.stop();
//      break;
//    }
//  }
//
//  return msg_buf;
//}
