#if !defined(FLEVELLOCALS_H)
#define FLEVELLOCALS_H

#include <stdint.h>

typedef struct
{
  char pad_0000[2512];    // 0x0000
  int8_t *LevelName;      // 0x09D0
  char pad_09D8[2472];    // 0x09D8
  void **Players;         // 0x1380
  char pad_1388[168];     // 0x1388
  int32_t TotalSecrets;   // 0x1430
  int32_t FoundSecrets;   // 0x1434
  int32_t TotalItems;     // 0x1438
  int32_t FoundItems;     // 0x143C
  int32_t TotalMonsters;  // 0x1440
  int32_t KilledMonsters; // 0x1444
  char pad_1448[2280];    // 0x1448
} FLevelLocals;           // Size: 0x1D30

#endif // FLEVELLOCALS_H
