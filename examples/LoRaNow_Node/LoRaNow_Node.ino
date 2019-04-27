/*
  LoRaNow Simple Node

  This code sends message and listen expecting some valid message from the gateway

  created 01 04 2019
  by Luiz H. Cassettari
*/

#include <LoRaNow.h>

void setup() {
  Serial.begin(115200);
  Serial.println("LoRaNow Simple Node");

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
  LoRaNow.onSleep(onSleep);
  LoRaNow.showStatus(Serial);
}

void loop() {
  LoRaNow.loop();
}

void onMessage(uint8_t *buffer, size_t size)
{
  Serial.print("Receive Message: ");
  Serial.write(buffer, size);
  Serial.println();
  Serial.println();
}

void onSleep()
{
  Serial.println("Sleep");
  delay(5000); // "kind of a sleep"
  Serial.println("Send Message");
  LoRaNow.print("LoRaNow Node Message ");
  LoRaNow.print(millis());
  LoRaNow.send();
}
