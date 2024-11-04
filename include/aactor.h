#if !defined(AACTOR_H)
#define AACTOR_H

#include <stdint.h>

typedef struct
{
  char pad_0000[432]; // 0x0000
  double Speed;       // 0x01B0
  char pad_01B8[136]; // 0x01B8
  double CameraFOV;   // 0x0240
  char pad_0248[88];  // 0x0248
  int32_t Health;     // 0x02A0
  char pad_02A4[236]; // 0x02A4
  double MeleeRange;  // 0x0390
  char pad_0398[40];  // 0x0398
  double Gravity;     // 0x03C0
  char pad_03C8[576]; // 0x03C8
} AActor;             // Size: 0x0608

#endif // AACTOR_H