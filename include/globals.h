#if !defined(GLOBALS_H)
#define GLOBALS_H

#include <Windows.h>

bool initalized = false;
bool menu_open = true;

float screenWidth = (float)GetSystemMetrics(SM_CXSCREEN);
float screenHeight = (float)GetSystemMetrics(SM_CYSCREEN);
float menu_depth = 4;
float menu_size = (screenWidth + screenHeight) / menu_depth;
float scale_factor = 0.0f;
bool animating = false;
float animation_duration = 0.3f;
float animation_start_time = 0.0f;
int page = 0;

int hooked_functions = 0;
#endif // GLOBALS_H