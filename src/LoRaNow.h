// ---------------------------------------------------- //
// LoRaNow.h
// ---------------------------------------------------- //
// Data: 18/09/2018
// Author: Luiz H. Cassettari
// ---------------------------------------------------- //
// Copyright © Luiz H. Cassettari. All rights reserved.
// Licensed under the MIT license.
// ---------------------------------------------------- //

#ifndef LORANOW_H
#define LORANOW_H

#include <Arduino.h>
#include "utility/LoRa.h"
#include "utility/LoRaUtilities.h"

// ---------------------------------------------------- //
// debug
// ---------------------------------------------------- //

//#define LORANOW_DEBUG_STREAM Serial

#ifdef LORANOW_DEBUG_STREAM
#define LORANOW_DEBUG_PRINTLN(...)                 \
	{                                              \
		LORANOW_DEBUG_STREAM.println(__VA_ARGS__); \
	}
#define LORANOW_DEBUG_PRINT(...)                 \
	{                                            \
		LORANOW_DEBUG_STREAM.print(__VA_ARGS__); \
	}
#else
#define LORANOW_DEBUG_PRINT(...)
#define LORANOW_DEBUG_PRINTLN(...)
#endif

// ---------------------------------------------------- //
// defines
// ---------------------------------------------------- //

#define LORANOW_PROTOCOL 0x27
#define LORANOW_BUF_SIZE 128
#define LORANOW_WAIT_READY 2000
#define LORANOW_WAIT_RECEIVE 100
#define LORANOW_WAIT_TXDONE 10

#if defined(ARDUINO_ARCH_ESP32)
#define LORANOW_DEFAULT_SS_PIN 18
#define LORANOW_DEFAULT_DIO0_PIN 26
#elif defined(ARDUINO_ARCH_ESP8266)
#define LORANOW_DEFAULT_SS_PIN 16
#define LORANOW_DEFAULT_DIO0_PIN 15
#else
#define LORANOW_DEFAULT_SS_PIN 10
#define LORANOW_DEFAULT_DIO0_PIN 2
#endif

#ifndef LORANOW_DEFAULT_FREQUENCY
#define LORANOW_DEFAULT_FREQUENCY 917E6
#endif

#define LORANOW_DEFAULT_SF 7

#if defined(ARDUINO_AVR_LORANOW_V1)
#define LORANOW_MOSFET_P 9
#endif

#if defined(ARDUINO_AVR_LORANOW_V0)
#define LORANOW_MOSFET_P 8
#endif

struct LoRaNowLayer
{
	uint8_t protocol;
	uint8_t id[4];
	uint8_t count;
	uint8_t length;
	uint8_t checksum;
};

// ---------------------------------------------------- //
// LoRa_State
// ---------------------------------------------------- //

enum LoRa_State
{
	LORA_STATE_NONE,

	LORA_STATE_END,
	LORA_STATE_INIT,
	LORA_STATE_SLEEP,

	LORA_STATE_TX_INIT,
	LORA_STATE_TX,
	LORA_STATE_TX_WAIT,
	LORA_STATE_TX_DONE,

	LORA_STATE_RX1_WAIT,
	LORA_STATE_RX1,
	LORA_STATE_RX1_DONE,
	LORA_STATE_RECEIVE,
};

// ---------------------------------------------------- //
// class
// ---------------------------------------------------- //

class LoRaNowClass : public Stream
{
  private:
	uint8_t _ss = LORANOW_DEFAULT_SS_PIN;
	uint8_t _dio0 = LORANOW_DEFAULT_DIO0_PIN;
	unsigned long frequency = LORANOW_DEFAULT_FREQUENCY;
	uint8_t sf = LORANOW_DEFAULT_SF;
	unsigned int rxwindow = LORANOW_WAIT_READY;

	// ----------------------------------------------- //
	// 	State machine

	unsigned int wait = 0;
	unsigned long time = 0;
	// ----------------------------------------------- //

	bool _gateway = false;

	uint32_t now_id = 0;
	uint8_t _count = 0;

	// ----------------------------------------------- //
	// 	buffer
	uint8_t payload_buf[LORANOW_BUF_SIZE];
	uint8_t payload_len = 0;
	uint8_t payload_position = 0;
	// ----------------------------------------------- //
  public:
	uint8_t state = 0;

	LoRaNowClass();

	byte begin();
	byte loop();
	void sleep();
	void end();

	void delay(long _millis);

	void showStatus(Stream &out);

	void setPinsSPI(int sck, int miso, int mosi, int ss = LORA_DEFAULT_SS_PIN, int dio0 = LORA_DEFAULT_DIO0_PIN);
	void setPins(int ss = LORA_DEFAULT_SS_PIN, int dio0 = LORA_DEFAULT_DIO0_PIN);

	void setFrequency(long _frequency);
	void setFrequencyCN();
	void setFrequencyEU();
	void setFrequencyUS();
	void setFrequencyAU();

	void setSpreadingFactor(int _sf);

	void gateway(bool gateway = true);
	void setId(uint32_t _id = 0);
	uint32_t id();
	uint8_t count();

	virtual size_t write(uint8_t byte);
	virtual size_t write(const uint8_t *buffer, size_t size);
	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();

	void send();
	void receive();

	void clear();
	uint8_t *buffer();

	int16_t read16();
	int32_t read24();
	int32_t read32();

	void onMessage(void (*cb)(uint8_t *payload, size_t size));
	void onSleep(void (*cb)());

  private:
	bool isSleep();
	bool isReady();

	int beginPacket();
	int endPacket();
	
	uint32_t makeId();

	// ----------------------------------------------- //
	// 	State machine
	void state_change(byte _state, unsigned long _wait = 0);
	void state_do(byte _state);
	// ----------------------------------------------- //

	unsigned int readInt();
	unsigned long readLong();

	int beginDecode();
	int endDecode();

	void (*messageCallback)(uint8_t *payload, size_t size);
	void (*sleepCallback)();

	byte calcCheckSum(byte *packege, byte length);

	void txMode();
	void rxMode();

	static void onReceive(int packetSize);
	static void onTxDone();
};

extern LoRaNowClass LoRaNow;

#endif
