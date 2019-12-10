// ---------------------------------------------------- //
// LoRaNow.cpp
// ---------------------------------------------------- //
// 28/08/2019 - esp32 interrupt fatal erro add ICACHE_RAM_ATTR
// 27/04/2019 - Add setPinsSPI to help esp32 boards
// 24/04/2019 - Fix LoRaNow board mosfet
// 23/04/2019 - Add onSleep callback
// 22/04/2019 - Add LORA_STATE_RECEIVE on loop - fix interrupt reset on esp32/esp8266
// 20/04/2019 - Fix esp32 id
// 03/04/2019 - First version release
// 02/04/2019 - Add boards pinout
// 01/04/2019 - Clear exemple
// 28/03/2019 - Documentation
// 24/02/2019 - Add UniqueID
// 23/02/2019 - Create exemple
// 01/02/2019 - Clear file
// 22/01/2019 - Create github
// 03/01/2019 - Add state machine
// 05/10/2018 - Fix protocol esp bug
// ---------------------------------------------------- //
// Data: 18/09/2018
// Author: Luiz H. Cassettari
// ---------------------------------------------------- //
// Copyright © Luiz H. Cassettari. All rights reserved.
// Licensed under the MIT license.
// ---------------------------------------------------- //

#include <Arduino.h>
#include "LoRaNow.h"
#include "utility/LoRa.h"
#include "utility/LoRaUtilities.h"

#if defined(ARDUINO_ARCH_AVR)
#include <avr/boot.h>
#ifndef SIGRD
#define SIGRD 5
#endif
#endif

#ifdef ESP8266 || ESP32
    #define ISR_PREFIX ICACHE_RAM_ATTR
#else
    #define ISR_PREFIX
#endif

LoRaNowClass::LoRaNowClass()
{
  setTimeout(0);
}

byte LoRaNowClass::begin()
{
  setPins(_ss, _dio0);

#if defined(LORANOW_MOSFET_P)
  pinMode(LORANOW_MOSFET_P, OUTPUT);
  digitalWrite(LORANOW_MOSFET_P, LOW);
  delay(5);
#endif

  if (LoRa.begin(frequency))
  {
    if (state == LORA_STATE_NONE)
    {
      LoRa.receive();
      uint32_t seed = (uint32_t)LoRa.random() << 24 | (uint32_t)LoRa.random() << 16 | (uint32_t)LoRa.random() << 8 | (uint32_t)LoRa.random();
      LoRa.sleep();
      randomSeed(seed);
      now_id = makeId();
    }
	
    LORANOW_DEBUG_PRINTLN("[ln] Begin");
    LoRa.onReceive(LoRaNowClass::onReceive);
    LoRa.onTxDone(LoRaNowClass::onTxDone);
    sleep();
    return 1;
  }
  else 
  {
    LORANOW_DEBUG_PRINTLN("[ln] Begin Fail");
  }
  return 0;
}

byte LoRaNowClass::loop()
{
  if (wait != 0)
  {
    if (millis() - time >= wait)
    {
      time += wait;
      state_change(state);
    }
  }

  if (isSleep())
  {
    if (LoRaNow.sleepCallback)
    {
      LoRaNow.sleepCallback();
    }
    else
    {
      LoRaNow.receive();
    }
  }

  return 1;
}

void LoRaNowClass::state_change(byte _state, unsigned long _wait)
{
  wait = _wait;
  state = _state;
  if (wait == 0)
  {
    state_do(state);
  }
}

void LoRaNowClass::state_do(byte _state)
{
  switch (_state)
  {
  case LORA_STATE_INIT:
    if (!begin())
      state = LORA_STATE_END;
    break;
  case LORA_STATE_END:
    end();
    break;
  case LORA_STATE_TX_INIT:
    beginPacket();
    state_change(LORA_STATE_TX);
    break;
  case LORA_STATE_TX:
    endPacket();
    state_change(LORA_STATE_TX_WAIT);
    break;
  case LORA_STATE_TX_WAIT:
    // wait TxDone - get time
    break;
  case LORA_STATE_TX_DONE:
    state_change(LORA_STATE_RX1);
    break;
  case LORA_STATE_RX1:
    rxMode();
    if (rxwindow > 0)
      state_change(LORA_STATE_RX1_DONE, rxwindow);
    break;
  case LORA_STATE_RX1_DONE:
    state_change(LORA_STATE_SLEEP);
    break;
  case LORA_STATE_RX1_WAIT:
  case LORA_STATE_SLEEP:
    sleep();
    break;
  case LORA_STATE_RECEIVE:
    LoRaNow.beginDecode();
    while (LoRa.available())
    {
      LoRaNow.write(LoRa.read());
    }
    if (LoRaNow.endDecode())
    {
      if (LoRaNow.messageCallback)
      {
        LoRaNow.messageCallback((uint8_t *)LoRaNow.buffer(), LoRaNow.available());
      }
    }
    else
    {
      LoRa.receive();
    }
    LoRaNow.clear();
    if (state != LORA_STATE_TX_WAIT)
        state_change(LORA_STATE_SLEEP);
    break;
  }
}

void LoRaNowClass::sleep()
{
  state = LORA_STATE_SLEEP;
  LORANOW_DEBUG_PRINTLN("[ln] sleep");
  LoRa.sleep();
}

void LoRaNowClass::end()
{
  state = LORA_STATE_END;
  LORANOW_DEBUG_PRINTLN("[ln] end");
  LoRa.end();
  LoRa.onReceive(NULL);
  LoRa.onTxDone(NULL);
#if defined(LORANOW_MOSFET_P)
  pinMode(LORANOW_MOSFET_P, INPUT);
  pinMode(_ss, INPUT);
#endif
}

bool LoRaNowClass::isSleep()
{
  return LORA_STATE_SLEEP == state || LORA_STATE_END == state;
}

bool LoRaNowClass::isReady()
{
  return LORA_STATE_TX_WAIT != state && (millis() - time >= LORANOW_WAIT_READY);
}

void LoRaNowClass::delay(long interval)
{
  unsigned long previousMillis = millis();
  while (millis() - previousMillis < interval)
  {
    loop();
  }
}

void LoRaNowClass::showStatus(Stream &out)
{
  out.print("Node Id: ");
  out.println(now_id, HEX);

  out.print("Frequecy: ");
  out.println(frequency);

  if (_count)
  {
    out.print("Count: ");
    out.println(_count);
  }
  if (available())
  {
    out.print("Payload: ");
    LORA_HEX_PRINTLN(out, (uint8_t *)LoRaNow.buffer(), LoRaNow.available());
  }
}

void LoRaNowClass::setPinsSPI(int sck, int miso, int mosi, int ss, int dio0)
{
#if defined(ARDUINO_ARCH_AVR)
#elif defined(ARDUINO_ARCH_ESP8266)
#elif defined(ARDUINO_ARCH_ESP32)
  SPI.begin(sck, miso, mosi, ss);
#endif
  setPins(ss, dio0);
}
void LoRaNowClass::setPins(int ss, int dio0)
{
  _ss = ss;
  _dio0 = dio0;
  LoRa.setPins(ss, -1, dio0);
}

void LoRaNowClass::setFrequency(long _frequency)
{
  frequency = _frequency;
}

void LoRaNowClass::setFrequencyCN()
{
  frequency = 486500000;
}

void LoRaNowClass::setFrequencyEU()
{
  frequency = 868300000;
}

void LoRaNowClass::setFrequencyUS()
{
  frequency = 904100000;
}

void LoRaNowClass::setFrequencyAU()
{
  frequency = 917000000;
}

void LoRaNowClass::setSpreadingFactor(int _sf)
{
  sf = _sf;
}

void LoRaNowClass::gateway(bool gateway)
{
  _gateway = gateway;
  rxwindow = 0;
}

void LoRaNowClass::setId(uint32_t _id)
{
  if (_id == 0)
  {
    _id = makeId();
  }
  now_id = _id;
}

uint32_t LoRaNowClass::makeId()
{
#if defined(ARDUINO_ARCH_AVR)
  uint32_t _id =
      (uint32_t)boot_signature_byte_get(0x13) << 24 |
      (uint32_t)boot_signature_byte_get(0x15) << 16 |
      (uint32_t)boot_signature_byte_get(0x16) << 8 |
      (uint32_t)boot_signature_byte_get(0x17);
  return _id;
#elif defined(ARDUINO_ARCH_ESP8266)
  return ESP.getChipId();
#elif defined(ARDUINO_ARCH_ESP32)
  uint32_t _id = (uint32_t)((uint64_t)ESP.getEfuseMac() >> 16);
  return ((((_id)&0xff000000) >> 24) | (((_id)&0x00ff0000) >> 8) | (((_id)&0x0000ff00) << 8) | (((_id)&0x000000ff) << 24)); // swap bits
#endif
  return 0;
}

uint32_t LoRaNowClass::id()
{
  return now_id;
}

uint8_t LoRaNowClass::count()
{
  return _count;
}

size_t LoRaNowClass::write(uint8_t c)
{
  payload_buf[payload_len++] = c;
  return 1;
};

size_t LoRaNowClass::write(const uint8_t *buffer, size_t size)
{
  size_t n = 0;
  for (size_t i = 0; i < size; i++)
  {
    n += write(*(buffer + i));
  }
  return n;
}

int LoRaNowClass::available()
{
  return payload_len - payload_position;
}

int LoRaNowClass::read()
{
  return payload_position < payload_len ? payload_buf[payload_position++] : -1;
}

int LoRaNowClass::peek()
{
  return payload_position < payload_len ? payload_buf[payload_position] : -1;
}

void LoRaNowClass::flush()
{
  LORANOW_DEBUG_PRINT("[ln] flush ");
  LORANOW_DEBUG_PRINTLN(state);
  if (state == LORA_STATE_NONE || state == LORA_STATE_END)
  {
    state_change(LORA_STATE_INIT);
  }
  if (state == LORA_STATE_SLEEP || state == LORA_STATE_RECEIVE)
  {
    state_change(LORA_STATE_TX_INIT);
  }
}

void LoRaNowClass::send()
{
  flush();
}

int16_t LoRaNowClass::read16()
{
  if (available() < 2)
    return 0;
  return (read() << 8 | read());
}

int32_t LoRaNowClass::read24()
{
  if (available() < 3)
    return 0;
  return (read() << 24 | read() << 16 | read() << 8) >> 8;
}

int32_t LoRaNowClass::read32()
{
  if (available() < 4)
    return 0;
  return (read() << 24 | read() << 16 | read() << 8 | read());
}

uint8_t *LoRaNowClass::buffer()
{
  return ((uint8_t *)&payload_buf) + payload_position;
}

void LoRaNowClass::clear()
{
  payload_len = 0;
  payload_position = 0;
  memset(payload_buf, 0, LORANOW_BUF_SIZE);
}

unsigned int LoRaNowClass::readInt()
{
  if (available() < 2)
    return 0;
  unsigned int i = 0;
  i += read() << 8;
  i += read();
  return i;
}

unsigned long LoRaNowClass::readLong()
{
  if (available() < 4)
    return 0;
  unsigned long i = 0;
  i += read() << 24;
  i += read() << 16;
  i += read() << 8;
  i += read();
  return i;
}

int LoRaNowClass::beginDecode()
{
  clear();
  return 1;
}

int LoRaNowClass::endDecode()
{
  LORANOW_DEBUG_PRINTLN("[ln] Decode Start");
  if (available())
  {
    if (payload_buf[0] != LORANOW_PROTOCOL)
    {
      LORANOW_DEBUG_PRINTLN("[ln] Protocol Error");
      clear();
      return 0;
    }
    byte checksum = calcCheckSum(payload_buf, payload_len);
    LORANOW_DEBUG_PRINT("[ln] ");
    LORANOW_DEBUG_PRINT("CheckSum: ");
    LORANOW_DEBUG_PRINT(checksum);
    LORANOW_DEBUG_PRINTLN("");
    if (checksum == 0)
    {
      LoRaNowLayer *ln = (LoRaNowLayer *)&payload_buf;
      LORANOW_DEBUG_PRINT("[ln] Id: ");
      LORANOW_DEBUG_PRINTLN(_LORA_ID_TO_INT32(ln->id));
      LORANOW_DEBUG_PRINT("[ln] Count: ");
      LORANOW_DEBUG_PRINTLN(ln->count);
      if (LoRaNow._gateway == true)
      {
        now_id = _LORA_ID_TO_INT32(ln->id);
        _count = ln->count;
      }
      else if (_LORA_ID_TO_INT32(ln->id) != now_id || _count != ln->count)
      {
        LORANOW_DEBUG_PRINTLN("[ln] Id / Count Error");
        return 0;
      }

      payload_position += sizeof(LoRaNowLayer);

      LORANOW_DEBUG_PRINT("[ln] Decode OK - ");
      LORANOW_DEBUG_PRINT(available());
      LORANOW_DEBUG_PRINTLN(" Bytes");

      return 1;
    }
  }
  LORANOW_DEBUG_PRINTLN("[ln] Decode Error");
  return 0;
}

int LoRaNowClass::beginPacket()
{
  LoRaNow.txMode();
  uint8_t buf[payload_len + 1];
  memcpy((uint8_t *)(buf), payload_buf, payload_len);
  memcpy((uint8_t *)(payload_buf + sizeof(LoRaNowLayer)), buf, payload_len);

  LoRaNowLayer *ln = (LoRaNowLayer *)&payload_buf;
  payload_len += sizeof(LoRaNowLayer);

  if (LoRaNow._gateway == false)
  {
    _count++;
  }
  ln->protocol = LORANOW_PROTOCOL;
  _LORA_INT32_TO_ID(ln->id, now_id);
  ln->count = _count;
  ln->checksum = 0;
  return 0;
}

int LoRaNowClass::endPacket()
{
  LoRaNowLayer *ln = (LoRaNowLayer *)&payload_buf;
  if (available())
  {
    ln->length = payload_len - sizeof(LoRaNowLayer);
    ln->checksum = -calcCheckSum(payload_buf, payload_len);
    LoRa.beginPacket();
    LoRa.write((uint8_t *)LoRaNow.buffer(), LoRaNow.available());
    LoRa.endPacket(true);

    LORANOW_DEBUG_PRINT("[ln] Send: ");
#ifdef LORANOW_DEBUG_STREAM
    LORA_HEX_PRINTLN(LORANOW_DEBUG_STREAM, (uint8_t *)LoRaNow.buffer(), LoRaNow.available());
#endif

    LoRaNow.clear();
    return 1;
  }
  return 0;
}

byte LoRaNowClass::calcCheckSum(byte *packege, byte length)
{
  byte sum = 0;
  for (int i = 0; i < length; i++)
  {
    sum = packege[i] + sum;
  }
  return sum;
}

void LoRaNowClass::txMode()
{
  LORANOW_DEBUG_PRINTLN("[ln] txMode");
  LoRa.idle();
  LoRa.setFrequency(frequency);
  LoRa.setSpreadingFactor(sf);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
  LoRa.disableCrc();

  if (LoRaNow._gateway == true)
    LoRa.enableInvertIQ();
  else
    LoRa.disableInvertIQ();
}

void LoRaNowClass::rxMode()
{
  LORANOW_DEBUG_PRINTLN("[ln] rxMode");
  if (LoRaNow._gateway != true)
    LoRa.enableInvertIQ();
  else
    LoRa.disableInvertIQ();

  LoRa.receive();
}

void LoRaNowClass::receive()
{
  state_change(LORA_STATE_RX1);
}

// ---------------------------------------------------- //
// callback
// ---------------------------------------------------- //

void LoRaNowClass::onMessage(void (*cb)(uint8_t *payload, size_t size))
{
  messageCallback = cb;
}

void LoRaNowClass::onSleep(void (*cb)())
{
  sleepCallback = cb;
}

// ---------------------------------------------------- //
// LoRa
// ---------------------------------------------------- //

ISR_PREFIX void LoRaNowClass::onReceive(int packetSize)
{
  LORANOW_DEBUG_PRINTLN("[ln] Receive");
  LoRaNow.time = millis();
  LoRaNow.state_change(LORA_STATE_RECEIVE, LORANOW_WAIT_RECEIVE);
}

ISR_PREFIX void LoRaNowClass::onTxDone()
{
  LORANOW_DEBUG_PRINTLN("[ln] txDone");
  LoRaNow.time = millis();
  LoRaNow.state_change(LORA_STATE_TX_DONE, LORANOW_WAIT_TXDONE);
}

LoRaNowClass LoRaNow;
