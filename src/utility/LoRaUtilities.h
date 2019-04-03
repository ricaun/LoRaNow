// Copyright © Luiz H. Cassettari. All rights reserved.
// Licensed under the MIT license.
// Version 1.0

#ifndef _LORA_UTILITIES_H_
#define _LORA_UTILITIES_H_

#define LORA_HTOI(c) ((c<='9')?(c-'0'):((c<='F')?(c-'A'+10):((c<='f')?(c-'a'+10):(0))))
#define LORA_TWO_HTOI(h, l) ((LORA_HTOI(h) << 4) + LORA_HTOI(l))

#define LORA_HEX_PRINT_BYTE(p, digit) \
{ \
  if(digit < 0x10) p.print('0'); \
  p.print(digit,HEX); \
}

#define LORA_HEX_PRINT(p, address, len) \
do { \
  int32_t i; \
  for (i = 0; i < len; i++) \
    LORA_HEX_PRINT_BYTE(p, (address)[i]); \
} while (0)

#define LORA_HEX_PRINTLN(out, address, len) _LORA_HEX_PRINTLN(out, address, len)

#define LORA_HEX_TO_BYTE(address, hex, len) _LORA_HEX_TO_BYTE(address, hex, len);
#define LORA_HEX_TO_EUI(address, hex) _LORA_HEX_TO_BYTE(address, hex, 8);
#define LORA_HEX_TO_KEY(address, hex) _LORA_HEX_TO_BYTE(address, hex, 16);
#define LORA_HEX_TO_DEVICE(device, hex) _LORA_HEX_TO_DEVICE(device, hex);

void _LORA_HEX_PRINTLN(Stream &out, byte * address, int len);
void _LORA_HEX_TO_BYTE(char * address, char * hex, int len);
void _LORA_HEX_TO_DEVICE(uint32_t &device, char * hex);

void _LORA_INT32_TO_ID(uint8_t * a, uint32_t id);
uint32_t _LORA_ID_TO_INT32(uint8_t * a);

#endif