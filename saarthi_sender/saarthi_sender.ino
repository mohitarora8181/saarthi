#include <LoRa.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

float latitude , longitude;
String  lat_str , lng_str;
TinyGPSPlus gps;
HardwareSerial SerialGPS(1);

#define VEH_NO 12345678
#define VEH_NAME "Mercedes"

void setup() {
  //LoRa Setup
  Serial.begin(115200); 
  while (!Serial);
  Serial.println("LoRa Sender");
  LoRa.setPins(5, 14, 4);    //setup LoRa transceiver module
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");

  //GPS Setup
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
}

void loop() {
  
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
        LoRa.beginPacket();
        String packet = "lat:"+lat_str+"#lon:"+lng_str+"#veh_no:"+VEH_NO+"#veh_name:"+VEH_NAME;
        LoRa.println(packet);
        LoRa.endPacket();
      }
     delay(1000);
     Serial.println();  
    }
  }
}
