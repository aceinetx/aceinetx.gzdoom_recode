#if !defined(GZDOOM_H)
#define GZDOOM_H

#include <stdint.h>
#include <stddef.h>
#include <FLevelLocals.h>
#include <aactor.h>
#include <macros.h>
#include <stdio.h>

extern bool invincibility;
extern bool one_hit_kill;
extern bool cant_hit_yourself;
extern bool infinite_melee_range;
extern bool target_hud;

extern bool debug_mode;

extern char popup_message[256];
extern int popup_frame;

extern int target_hud_frame;
extern bool target_hud_wait_until_close;
extern AActor *target_hud_actor;

namespace GZDoom
{
  typedef fnptr(int, AActorGetMaxHealth_t, AActor *, bool);

  extern uintptr_t base;
  extern uintptr_t doDamage;

  extern FLevelLocals *level;
  extern AActor *local_player_actor;

  extern float *fov;

  extern AActorGetMaxHealth_t AActorGetMaxHealth;

  extern fnptr(void, DoDamageMobj, AActor *dmgtarget, AActor *inflictor, AActor *source, int amount, short fname, int flags, long angle);
  void DoDamageMobj_Hook(AActor *dmgtarget, AActor *inflictor, AActor *source, int amount, short fname, int flags, long angle);

  extern fnptr(__int64, PatchAmmo, unsigned int ammoNum, int flags);
  __int64 PatchAmmo_Hook(unsigned int ammoNum, int flags);
};

#endif // GZDOOM_H