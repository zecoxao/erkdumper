#include "hvcall.h"
#include "peek_poke.h"

#define INSTALL_HVSC_REDIRECT(_hvcall) \
	u64 original_syscall_code_1 = lv2_peek(HVSC_SYSCALL_ADDR); \
	u64 original_syscall_code_2 = lv2_peek(HVSC_SYSCALL_ADDR + 8); \
	u64 original_syscall_code_3 = lv2_peek(HVSC_SYSCALL_ADDR + 16); \
	u64 original_syscall_code_4 = lv2_peek(HVSC_SYSCALL_ADDR + 24); \
	lv2_poke(HVSC_SYSCALL_ADDR +  0, 0x7C0802A6F8010010ULL); \
	lv2_poke(HVSC_SYSCALL_ADDR +  8, 0x3960000044000022ULL | ((u64)(_hvcall) << 32)); \
	lv2_poke(HVSC_SYSCALL_ADDR + 16, 0xE80100107C0803A6ULL); \
	lv2_poke(HVSC_SYSCALL_ADDR + 24, 0x4e80002060000000ULL);
	
#define REMOVE_HVSC_REDIRECT() \
	lv2_poke(HVSC_SYSCALL_ADDR, original_syscall_code_1); \
	lv2_poke(HVSC_SYSCALL_ADDR + 8, original_syscall_code_2); \
	lv2_poke(HVSC_SYSCALL_ADDR + 16, original_syscall_code_3); \
	lv2_poke(HVSC_SYSCALL_ADDR + 24, original_syscall_code_4);

#define HVSC_SYSCALL_ADDR 0x8000000000195540ULL

#define SC_QUOTE_(_x) #_x
#define SYSCALL(_num) "li %%r11, " SC_QUOTE_(_num) "; sc;"

s64 lv1_insert_htab_entry(u64 htab_id, u64 hpte_group, u64 hpte_v, u64 hpte_r, u64 bolted_flag, u64 flags, u64 *hpte_index, u64 *hpte_evicted_v, u64 *hpte_evicted_r) {
	s64 result;
	u64 ret_hpte_index = 0, ret_hpte_evicted_v = 0, ret_hpte_evicted_r = 0;

	INSTALL_HVSC_REDIRECT(0x9E);

	__asm__ __volatile__(
		"mr %%r3, %4;"
		"mr %%r4, %5;"
		"mr %%r5, %6;"
		"mr %%r6, %7;"
		"mr %%r7, %8;"
		"mr %%r8, %9;"
		SYSCALL(HVSC_SYSCALL)
		"mr %0, %%r3;"
		"mr %1, %%r4;"
		"mr %2, %%r5;"
		"mr %3, %%r6;"
		: "=r"(result), "=r"(ret_hpte_index), "=r"(ret_hpte_evicted_v), "=r"(ret_hpte_evicted_r)
		: "r"(htab_id), "r"(hpte_group), "r"(hpte_v), "r"(hpte_r), "r"(bolted_flag), "r"(flags)
		: "r0", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory"
	);
		
	REMOVE_HVSC_REDIRECT();

	*hpte_index = ret_hpte_index;
	*hpte_evicted_v = ret_hpte_evicted_v;
	*hpte_evicted_r = ret_hpte_evicted_r;

	return result;
}

s64 lv1_write_htab_entry(u64 vas_id, u64 hpte_index, u64 hpte_v, u64 hpte_r) {
	s64 result;

	INSTALL_HVSC_REDIRECT(1);

	__asm__ __volatile__ (
		"mr %%r3, %1;"
		"mr %%r4, %2;"
		"mr %%r5, %3;"
		"mr %%r6, %4;"
		SYSCALL(HVSC_SYSCALL)
		"mr %0, %%r3;"
		: "=r"(result)
		: "r"(vas_id), "r"(hpte_index), "r"(hpte_v), "r"(hpte_r)
		: "r0", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory"
	);
		
	REMOVE_HVSC_REDIRECT();

	return result;
}

s64 lv1_undocumented_function_114(u64 start, u64 page_size, u64 size, u64 *lpar_addr) {	
	s64 result;
	u64 ret_lpar_addr = 0;

	INSTALL_HVSC_REDIRECT(0x72);

	__asm__ __volatile__(
		"mr %%r3, %2;"
		"mr %%r4, %3;"
		"mr %%r5, %4;"
		SYSCALL(HVSC_SYSCALL)
		"mr %0, %%r3;"
		"mr %1, %%r4;"
		: "=r"(result), "=r"(ret_lpar_addr)
		: "r"(start), "r"(page_size), "r"(size)
		: "r0", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory"
	);
	
	REMOVE_HVSC_REDIRECT();
	
	*lpar_addr = ret_lpar_addr;

	return result;
}

s64 lv1_undocumented_function_115(u64 lpar_addr) {
	INSTALL_HVSC_REDIRECT(0x73);
	
	__asm__ __volatile__(
		"mr %%r3, %0;"
		SYSCALL(HVSC_SYSCALL)
		:
		: "r"(lpar_addr)
		: "r0", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory"
	);
	
	REMOVE_HVSC_REDIRECT();

	return 0;
}

s64 lv1_panic(u64 flag) {
	INSTALL_HVSC_REDIRECT(0xFF);
	
	__asm__ __volatile__(
		"mr %%r3, %0;"
		SYSCALL(HVSC_SYSCALL)
		:
		: "r"(flag)
		: "r0", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr", "ctr", "xer", "cr0", "cr1", "cr5", "cr6", "cr7", "memory"
	);
	
	REMOVE_HVSC_REDIRECT();

	return 0;
}
