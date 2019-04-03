// Copyright © Luiz H. Cassettari. All rights reserved.
// Licensed under the MIT license.
// Version 1.0

#include <Arduino.h>
#include "LoRaUtilities.h"

void _LORA_HEX_PRINTLN(Stream &out, byte * address, int len) 
{
  LORA_HEX_PRINT(out, address, len);
  out.println();
}

void _LORA_HEX_TO_BYTE(char * address, char * hex, int len)
{
  for (int i = 0; i < len; i++)
    address[i] = LORA_TWO_HTOI(hex[2*i], hex[2*i + 1]);
}

void _LORA_HEX_TO_DEVICE(uint32_t &device, char * hex)
{
  byte len = 4;
  device = 0;
  for (int i = 0; i < len; i++) {
    device += LORA_TWO_HTOI(hex[2*i], hex[2*i + 1]);
    if (i != len -1) device = device << 8;
  }
}

void _LORA_INT32_TO_ID(uint8_t * a, uint32_t id)
{
  (a)[0] = (id & 0xFF);
  (a)[1] = ((id >> 8) & 0xFF);
  (a)[2] = ((id >> 16) & 0xFF);
  (a)[3] = ((id >> 24) & 0xFF);
}

uint32_t _LORA_ID_TO_INT32(uint8_t * a)
{
  return (uint32_t) ((uint32_t) (a)[0] | (uint32_t) (a)[1] << 8 | (uint32_t) (a)[2] << 16 | (uint32_t) (a)[3] << 24);
}


