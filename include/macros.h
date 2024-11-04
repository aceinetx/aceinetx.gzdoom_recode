#include <MinHook.h>

#define CreateAndEnableHook(from, to, trampoline)                               \
  if (MH_CreateHook((LPVOID)(from), (LPVOID)to, (LPVOID *)trampoline) == MH_OK) \
    if (MH_EnableHook((LPVOID)(from)) == MH_OK)                                 \
      hooked_functions++;

#define fnptr(retType, name, ...) retType (*name)(__VA_ARGS__)