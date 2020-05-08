/*
  LoRaNow Simple Gateway

  This code sends message to node id number one.

  created 01 05 2020
  by Luiz H. Cassettari
*/

#include <LoRaNow.h>

void setup() {
  Serial.begin(115200);
  Serial.println("LoRaNow Simple Gateway");

  // LoRaNow.setFrequencyCN(); // Select the frequency 486.5 MHz - Used in China
  // LoRaNow.setFrequencyEU(); // Select the frequency 868.3 MHz - Used in Europe
  // LoRaNow.setFrequencyUS(); // Select the frequency 904.1 MHz - Used in USA, Canada and South America
  // LoRaNow.setFrequencyAU(); // Select the frequency 917.0 MHz - Used in Australia, Brazil and Chile
  
  // LoRaNow.setFrequency(frequency);
  // LoRaNow.setSpreadingFactor(sf);
  // LoRaNow.setPins(ss, dio0);

  // LoRaNow.setPinsSPI(sck, miso, mosi, ss, dio0); // Only works with ESP32

  if (!LoRaNow.begin()) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }

  LoRaNow.onMessage(onMessage);
  LoRaNow.gateway();
}

void loop() {
  LoRaNow.loop();
  if (runEvery(2000))
  {
    Serial.println("Send Message");
    // Send data to the node
    LoRaNow.clear();
    LoRaNow.print("LoRaNow Gateway Message ");
    LoRaNow.print(millis());
    LoRaNow.send(1); // Send to Node Id (Only work with gateway) 
  }
}

void onMessage(uint8_t *buffer, size_t size)
{
  unsigned long id = LoRaNow.id();
  byte count = LoRaNow.count();
  
  Serial.print("Node Id: ");
  Serial.println(id, HEX);
  Serial.print("Count: ");
  Serial.println(count);
  Serial.print("Message: ");
  Serial.write(buffer, size);
  Serial.println();
  Serial.println();
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}