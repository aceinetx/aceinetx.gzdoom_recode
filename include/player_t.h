#if !defined(PLAYER_T_H)
#define PLAYER_T_H

#include <stdint.h>

typedef struct
{
  char pad_0000[88];  // 0x0000
  float FOV;          // 0x0058
  char pad_005C[60];  // 0x005C
  int32_t Health;     // 0x0098
  char pad_009C[92];  // 0x009C
  int32_t killCount;  // 0x00F8
  int32_t itemCount;  // 0x00FC
  char pad_0100[112]; // 0x0100
  uint8_t onground;   // 0x0170
  char pad_0171[351]; // 0x0171
} player_t;           // Size: 0x02D0

#endif // PLAYER_T_H