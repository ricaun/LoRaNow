# LoRaNow API

## Include Library

```c
#include <LoRaNow.h>
```

### Begin

Initialize the library.

```c
LoRaNow.begin();
```
Returns `1` on success, `0` on failure.

### Set pins

Override the default `SS`, and `DIO0` pins used by the library. **Must** be called before `LoRaNow.begin()`.

```c
LoRaNow.setPins(ss, dio0);
```
 * `ss` - new slave select pin to use, defaults to `10` or `gpio16`
 * `dio0` - new DIO0 pin to use, defaults to `2` or `gpio15`.  **Must** be interrupt capable via [attachInterrupt(...)](https://www.arduino.cc/en/Reference/AttachInterrupt).

This call is optional and only needs to be used if you need to change the default pins used.

### Set pins SPI

Override the SPI pins and the default `SS`, and `DIO0` pins used by the library. **Must** be called before `LoRaNow.begin()`.

```c
LoRaNow.setPinsSPI(sck, miso, mosi, ss, dio0);
```
 * `sck` - new sck pin to use for SPI communication
 * `miso` - new miso pin to use for SPI communication
 * `mosi` - new mosi pin to use for SPI communication
 * `ss` - new slave select pin to use, defaults to `10` or `gpio16`
 * `dio0` - new DIO0 pin to use, defaults to `2` or `gpio15`.  **Must** be interrupt capable via [attachInterrupt(...)](https://www.arduino.cc/en/Reference/AttachInterrupt).

This call is optional and only works on ESP32 platform.

### End

Stop the library

```c
LoRaNow.end()
```

## Sending data

### Clear

Clear the payload buffer.

```c
LoRaNow.clear();
```

### Writing

Write data to the  payload buffer. Each packet can contain up to 128 bytes.

```c
LoRaNow.write(byte);

LoRaNow.write(buffer, length);
```
* `byte` - single byte to write to packet

or

* `buffer` - data to write to packet
* `length` - size of data to write

Returns the number of bytes written.

**Note:** Other Arduino `Print` API's can also be used to write data into the packet

### Send

Send the payload buffer to the LoRa Module.

```c
LoRaNow.send();
```

#### Register callback

### onMessege

Register a callback function for when a valid payload is received.

```c
LoRaNow.onMessage(onMessage);

void onMessage(uint8_t *buffer, size_t size) {
 // ...
}
```

 * `onMessage` - function to call when a valid payload is received.

### onSleep

Register a callback function for when a the protocol is on sleep mode.

```c
LoRaNow.onSleep(onSleep);

void onSleep() {
 // ...
}
```

 * `onSleep` - function to call when a protocol is on sleep mode.

## State machine

### Loop

This function need to be on the loop to work properly 
 * This function uses `millis()`
 * All callback is called by `LoRaNow.loop()`

```c
LoRaNow.loop();
```

## Radio parameters

### Frequency

Change the frequency of the radio.

```c
LoRaNow.setFrequency(frequency);
```
 * `frequency` - frequency in Hz (`433E6`, `866E6`, `915E6`)

## Frequency by country

* `LoRaNow.setFrequencyCN()` - Select the frequency 486.5 MHz - Used in China
* `LoRaNow.setFrequencyEU()` - Select the frequency 868.3 MHz - Used in Europe
* `LoRaNow.setFrequencyUS()` - Select the frequency 904.1 MHz - Used in USA, Canada and South America
* `LoRaNow.setFrequencyAU()` - Select the frequency 917.0 MHz - Used in Australia, Brazil and Chile

### Spreading Factor

Change the spreading factor of the radio.

```c
LoRaNow.setSpreadingFactor(spreadingFactor);
```
 * `spreadingFactor` - spreading factor, defaults to `7`

Supported values are between `6` and `12`.

## Protocol parameters

### Id

Identification of the node, build in number using the serial number [ArduinoUniqueID](https://github.com/ricaun/ArduinoUniqueID) of the chip (AVR) or the mac adress (ESP)

```c
unsigned long id = LoRaNow.id();
```

```c
LoRaNow.setId(id);
```

* `id` - identification number (4 bytes)

### Count

The count number, always increment by one when a message is send.

```c
byte count = LoRaNow.count();
```
 
### Gateway

This function defines the board to work like a gateway, this means always listen for messages from the nodes.

```c
LoRaNow.gateway();
```