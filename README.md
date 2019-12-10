# LoRaNow

LoRaNow is a open source communication protocol to make easier to understand the concept of Node / Gateway communication using LoRa technology.

This library was design using the [Arduino LoRa](https://github.com/sandeepmistry/arduino-LoRa) Library by [Sandeep Mistry](https://github.com/sandeepmistry).

## Features 

* Hardware Compatibility - Compatible with Arduino and ESP boards with a LoRa module attached.
* Id - Each node has an identification, by default the library creates a unique id for the node. 
* Checksum - Each payload has a checksum to verify if the data is corrupted.
* State Machine - The library has a state machine algorithm to manage the LoRa module, open an RX window after sending data and put the module to sleep. 

## Compatible Hardware

The main focus hardware is Arduino and ESP boards with [RFM95W](http://www.hoperf.com/rf_transceiver/lora/RFM95W.html) LoRa module.

### RFM95W

The default pinout connection is like the table below.

| RFM95W | Arduino | ESP8266 | ESP32 |
| :----: | :-----: | :-----: | :-----: |
| VCC | 3.3V | 3.3V | 3.3V |
| GND | GND | GND | GND |
| SCK | SCK | SCK | SCK |
| MISO | MISO | MISO | MISO |
| MOSI | MOSI | MOSI | MOSI |
| SS | 10 | gpio16 | gpio18 |
| DIO0 | 2 | gpio15 | gpio26 |


`SS`, and `DIO0` pins can be changed by using `LoRaNow.setPins(ss, dio0)` or `LoRaNow.setPinsSPI(sck, miso, mosi, ss, dio0)`. `DIO0` pin is needed, it **must** be interrupt capable via [`attachInterrupt(...)`](https://www.arduino.cc/en/Reference/AttachInterrupt).

# Ferquency

You can use [this table](https://www.thethingsnetwork.org/wiki/LoRaWAN/Frequencies/By-Country) to lookup the available frequencies by your country. The selectable frequency also depends on your hardware. 

You can select the frequency using `LoRaNow.setFrequency(frequency)` or you can select the frequency by country.

* `LoRaNow.setFrequencyCN()` - Select the frequency 486.5 MHz - Used in China
* `LoRaNow.setFrequencyEU()` - Select the frequency 868.3 MHz - Used in Europe
* `LoRaNow.setFrequencyUS()` - Select the frequency 904.1 MHz - Used in USA, Canada and South America
* `LoRaNow.setFrequencyAU()` - Select the frequency 917.0 MHz - Used in Australia, Brazil and Chile

# Protocol Payload

This is the payload format, this protocol don't have any kind of encryption.

| Protocol | Device ID | Count | Length | CheckSum | Payload |
| :-------: | :------: | :------:| :------:| :------:| :------:|
| 1 Byte | 4 Bytes | 1 Byte | 1 Byte | 1 Byte | N Bytes |


## Installation

* Install the library by [Using the Library Manager](https://www.arduino.cc/en/Guide/Libraries#toc3)
* **OR** by [Importing the .zip library](https://www.arduino.cc/en/Guide/Libraries#toc4) using either the [master](https://github.com/ricaun/LoRaNow/archive/1.0.5.zip) or one of the [releases](https://github.com/ricaun/LoRaNow/releases) ZIP files.

## Examples

The library comes with [examples](examples). After installing the library you need to restart the Arduino IDE before they can be found under **File > Examples > LoRaNow**.

## API

See [API.md](API.md).

## License

This library is [licensed](LICENSE) under the [MIT Licence](https://en.wikipedia.org/wiki/MIT_License).

## To do

- [x] Example for ESP8266 board
- [x] Example for ESP32 board
- [ ] Improve documentation
- [ ] State machine

----

See news and other projects on my [blog](http://loranow.com)

Do you like this library? Please [star this project on GitHub](https://github.com/ricaun/LoRaNow/stargazers)!
