#include <LoRa.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <math.h>

#define TFT_CS 15
#define TFT_DC 21
#define TFT_RST 4
SPIClass vspi(VSPI);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

#define LORA_CS 5
#define LORA_RST 27
#define LORA_IRQ 2
SPIClass hspi(HSPI);

void setup() {
  Serial.begin(115200);
  // vspi.begin(18, 19, 23); // SCLK, MISO, MOSI for VSPI
  tft.begin();

  // Init LoRa
  hspi.begin(14, 12, 13);  // SCLK, MISO, MOSI for HSPI
  LoRa.setSPI(hspi);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  Serial.println("LoRa Receiver");

  while (!LoRa.begin(433E6))  //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");

  float senderCoords[2] = { 28.6139, 77.2090 };    // From LoRa
  float receiverCoords[2] = { 28.7041, 77.1025 };  // From onboard GPS or fixed

  drawUI(senderCoords, receiverCoords);
}

void loop() {
  int packetSize = LoRa.parsePacket();  // try to parse packet
  if (packetSize) {

    Serial.print("Received packet :- ");

    while (LoRa.available())  // read packet
    {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData);
      int latStart = LoRaData.indexOf("lat:") + 4;
      int latEnd = LoRaData.indexOf("#", latStart);
      String lat = LoRaData.substring(latStart, latEnd);

      int lonStart = LoRaData.indexOf("lon:") + 4;
      int lonEnd = LoRaData.indexOf("#", lonStart);
      String lon = LoRaData.substring(lonStart, lonEnd);

      updateSenderLocation(lat, lon);
    }
    Serial.print("with RSSI ");  // print RSSI of packet
    Serial.println(LoRa.packetRssi());
  }
}

float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  const float R = 6371.0;  // Earth radius in km

  float dLat = radians(lat2 - lat1);
  float dLon = radians(lon2 - lon1);

  float a = sin(dLat / 2) * sin(dLat / 2) + cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon / 2) * sin(dLon / 2);

  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  float distance = R * c;

  return distance;  // in kilometers
}


void drawUI(float sender[2], float receiver[2]) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1);

  tft.setTextSize(3);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(100, 10);
  tft.println("SAARTHI");

  tft.setCursor(20, 50);
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(1);
  tft.println("Sender:");

  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(30, 70);
  tft.setTextSize(2);
  tft.printf("Lat : %.6f", sender[0]);
  tft.setCursor(30, 90);
  tft.printf("Lon : %.6f", sender[1]);

  tft.setCursor(20, 120);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_CYAN);
  tft.println("Receiver:");

  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(30, 140);
  tft.setTextSize(2);
  tft.printf("Lat : %.6f", receiver[0]);
  tft.setCursor(30, 160);
  tft.printf("Lon : %.6f", receiver[1]);

  // Distance
  float dist = calculateDistance(sender[0], sender[1], receiver[0], receiver[1]);

  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(40, 200);
  tft.setTextSize(3);
  tft.printf("Dist: %.2f km", dist);
}

void updateSenderLocation(String lat, String lon) {
  // Clear old latitude
  tft.fillRect(30, 70, 200, 20, ILI9341_BLACK);  // x, y, w, h, color
  tft.setCursor(30, 70);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("Lat : ");
  tft.print(lat);

  // Clear old longitude
  tft.fillRect(30, 90, 200, 20, ILI9341_BLACK);
  tft.setCursor(30, 90);
  tft.print("Lon : ");
  tft.print(lon);
}
