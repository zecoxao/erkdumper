#ifndef _PEEK_POKE_H
#define _PEEK_POKE_H

#include "common.h"

void lv2_copy_memory(u64 addr, const void *data, u64 size);

u64 lv2_peek(u64 address);

void lv2_poke(u64 address, u64 value);
void lv2_poke32(u64 address, u32 value);
void lv2_poke16(u64 addr, u16 value);
void lv2_poke8(u64 addr, u8 value);

u64 lv1_peek(u64 address);
void lv1_poke(u64 address, u64 value);

void install_new_poke();
void remove_new_poke();

#endif /* !_PEEK_POKE_H */
