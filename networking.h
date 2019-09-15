#include<WiFi101.h>
extern WiFiSSLClient client;
void setup_wifi_network();

void set_time();

int get_time();

char* send_to_canopy(String m);
