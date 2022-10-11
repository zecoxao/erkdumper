#ifndef _COMMON_H
#define _COMMON_H

#include <assert.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <ppu-types.h>
#include <ppu-lv2.h>

#define HV_BASE                 0x8000000014000000ULL
#define HV_SIZE                 0x1000000ULL
#define HV_PAGE_SIZE            12

#define HVSC_SYSCALL 811

#define countof(_array) (sizeof(_array) / sizeof((_array)[0]))

#endif /* !_COMMON_H */
