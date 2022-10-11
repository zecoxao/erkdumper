#include "peek_poke.h"
#include "hvcall.h"

static s32 poke_syscall = 7;

#define NEW_POKE_SYSCALL 813
#define NEW_POKE_SYSCALL_ADDR 0x8000000000195A68ULL

void lv2_copy_memory(u64 addr, const void *data, u64 size) {
	if (size == 0)
		return;
	const u8 *in = (const u8 *)data;
	while (size >= 8) {
		lv2_poke(addr, *(const u64 *)in);
		addr += 8; in += 8;
		size -= 8;
	}
	while (size >= 4) {
		lv2_poke32(addr, *(const u32 *)in);
		addr += 4; in += 4;
		size -= 4;
	}
	while (size >= 2) {
		lv2_poke16(addr, *(const u16 *)in);
		addr += 2; in += 2;
		size -= 2;
	}
	if (size > 0) {
		lv2_poke8(addr, *(const u8 *)in);
		size--;
	}
}

u64 lv2_peek(u64 address) {
	lv2syscall1(6, address);
	return_to_user_prog(u64);
}

void lv2_poke(u64 address, u64 value) {
	lv2syscall2(poke_syscall, address, value);
}

void lv2_poke32(u64 address, u32 value) {
	u32 old_value = lv2_peek(address);
	lv2_poke(address, ((u64)value << 32) | (old_value & 0xFFFFFFFFULL));
}

void lv2_poke16(u64 addr, u16 value) {
	u64 old_value = lv2_peek(addr);
	lv2_poke(addr, ((u64)value << 48) | (old_value & 0xFFFFFFFFFFFFULL));
}

void lv2_poke8(u64 addr, u8 value) {
	u64 old_value = lv2_peek(addr);
	lv2_poke(addr, ((u64)value << 56) | (old_value & 0xFFFFFFFFFFFFFFULL));
}

u64 lv1_peek(u64 address) {
	lv2syscall1(6, HV_BASE + address);
	return_to_user_prog(u64);
}

void lv1_poke(u64 address, u64 value) {
	lv2syscall2(7, HV_BASE + address, value);
}

void install_new_poke() {
	/* install poke with icbi instruction */
	lv2_poke(NEW_POKE_SYSCALL_ADDR, 0xF88300007C001FACULL);
	lv2_poke(NEW_POKE_SYSCALL_ADDR + 8, 0x4C00012C4E800020ULL);

	poke_syscall = NEW_POKE_SYSCALL;
}

void remove_new_poke() {
	poke_syscall = 7;

	lv2_poke(NEW_POKE_SYSCALL_ADDR, 0xF821FF017C0802A6ULL);
	lv2_poke(NEW_POKE_SYSCALL_ADDR + 8, 0xFBC100F0FBE100F8ULL);	
}
