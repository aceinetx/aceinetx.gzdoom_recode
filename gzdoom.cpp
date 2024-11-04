#include <gzdoom.h>

bool invincibility = false;
bool one_hit_kill = false;
bool cant_hit_yourself = false;
bool infinite_melee_range = false;
bool target_hud = true;

bool debug_mode = false;

char popup_message[256];
int popup_frame = 0;

int target_hud_frame = 0;
bool target_hud_wait_until_close = false;
AActor *target_hud_actor = 0;

uintptr_t GZDoom::base = 0;
uintptr_t GZDoom::doDamage;

FLevelLocals *GZDoom::level = 0;
AActor *GZDoom::local_player_actor = 0;

float *GZDoom::fov = 0;

GZDoom::AActorGetMaxHealth_t GZDoom::AActorGetMaxHealth = 0;

fnptr(void, GZDoom::DoDamageMobj, AActor *dmgtarget, AActor *inflictor, AActor *source, int amount, short fname, int flags, long angle) = 0;
void GZDoom::DoDamageMobj_Hook(AActor *dmgtarget, AActor *inflictor, AActor *source, int amount, short fname, int flags, long angle)
{
  if (source == local_player_actor)
  {
    if (one_hit_kill)
    {
      amount = *(int *)((uintptr_t)dmgtarget + 0x2A0);
    }
    if (cant_hit_yourself)
    {
      if (dmgtarget == local_player_actor && source == local_player_actor)
      {
        amount = 0;
      }
    }

    snprintf(popup_message, sizeof(popup_message), "Dealt %d damage", amount);
    if (debug_mode)
    {
      snprintf(popup_message, sizeof(popup_message), "%p Dealt %d damage to %p", source, amount, dmgtarget);
    }
    popup_frame = 500;
  }
  if (invincibility)
  {
    if (dmgtarget == local_player_actor)
    {
      amount = 0;
    }
  }

  if (target_hud_actor != dmgtarget && source == GZDoom::local_player_actor && target_hud)
  {
    target_hud_actor = dmgtarget;
    target_hud_frame = 1000;
  }

  if (target_hud_actor == dmgtarget && source == GZDoom::local_player_actor && target_hud && target_hud_frame > 0)
  {
    target_hud_frame = 1000;
  }

  DoDamageMobj(dmgtarget, inflictor, source, amount, fname, flags, angle);
}

fnptr(__int64, GZDoom::PatchAmmo, unsigned int ammoNum, int flags) = 0;
__int64 GZDoom::PatchAmmo_Hook(unsigned int ammoNum, int flags)
{
  if (ammoNum < 0)
  {
    ammoNum = 0;
  }
  ammoNum = 0;

  return PatchAmmo(ammoNum, flags);
}