#include "thermistor.h"
#include <Arduino.h>
float calc_resistance(){
      //these calculations are derived from ADC conversion and voltage divider equations
      float temp = analogRead(A5);
      
       temp = 1023 / temp - 1;
       temp = 3000 / temp;
      
      
      return temp;
      
      }


float calc_temp(){
      float resistance =calc_resistance();
      // from datasheet of thermistor
    float steinhart;
    steinhart = resistance / THERMISTORNOMINAL;     // (R/Ro)
    steinhart = log(steinhart);                  // ln(R/Ro)
    steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                 // Invert
    steinhart -= 273.15;                         // convert to C
    
    
      return steinhart;
      
      }

    float print_device(){
//      Serial.println("Temperature (*C) is:");
      float temperature = calc_temp();
//      Serial.println("...in *F");
      float mul = 1.8;
      temperature = temperature * mul;
      temperature = temperature+32;
      delay(2000);
return temperature;

}
