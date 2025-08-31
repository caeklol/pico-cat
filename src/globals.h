#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include "config.h"

// theres a better way than globals, etc, etc. but i just dont feel like it rn so

#ifdef ENABLE_DEBUG
    #include <stdio.h>
	#include "pico/stdlib.h"
    #define DEBUG_INIT() stdio_init_all()
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_INIT() ((void)0)
    #define DEBUG_PRINT(...) ((void)0)
#endif

extern volatile bool g_sleeping;

#endif
