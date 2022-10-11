#ifndef _MM_H
#define _MM_H

#include "common.h"

#define HTAB_HASH_MASK  0x7FFULL
#define HPTES_PER_GROUP 8

#define HPTE_V_AVPN_SHIFT 7
#define HPTE_V_BOLTED     0x0000000000000010ULL
#define HPTE_V_LARGE      0x0000000000000004ULL
#define HPTE_V_VALID      0x0000000000000001ULL
#define HPTE_V_FLAGS_MASK 0x000000000000007FULL

#define HPTE_R_R          0x0000000000000100ULL
#define HPTE_R_C          0x0000000000000080ULL
#define HPTE_R_W          0x0000000000000040ULL
#define HPTE_R_I          0x0000000000000020ULL
#define HPTE_R_M          0x0000000000000010ULL
#define HPTE_R_G          0x0000000000000008ULL
#define HPTE_R_N          0x0000000000000004ULL
#define HPTE_R_PROT_MASK  0x0000000000000003ULL
#define HPTE_R_FLAGS_MASK 0x000000000000FFFFULL

#define PAGE_SIZES(_ps0, _ps1) (((u64)(_ps0) << 56) | ((u64)(_ps1) << 48))
#define PAGE_SIZE_4KB          12
#define PAGE_SIZE_64KB         16
#define PAGE_SIZE_1MB          20
#define PAGE_SIZE_16MB         24

#define MM_LOAD_BASE(_ptr, _offset) \
	__asm__ __volatile__ ( \
		"li %0, 1;" \
		"rldicr %0, %0, 63, 0;" \
		"oris %0, %0, %1;" \
		"ori %0, %0, %2" \
		: "=r"(_ptr) \
		: "g"(((_offset) >> 16) & 0xFFFF), "g"((_offset) & 0xFFFF) \
	)

#define MM_EA2VA(_ea_addr) ((u64)(_ea_addr) & ~(1ULL << 63))

s64 mm_insert_htab_entry(u64 va_addr, u64 lpar_addr, u64 prot, u64 *index);
s64 mm_map_lpar_memory_region(u64 lpar_start_addr, u64 ea_start_addr, u64 size, u64 page_shift, u64 prot);

#endif /* !_MM_H */
