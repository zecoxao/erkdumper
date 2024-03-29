#include "mm.h"
#include "hvcall.h"

s64 mm_insert_htab_entry(u64 va_addr, u64 lpar_addr, u64 prot, u64 *index) {
	s64 result;
	u64 hpte_group, hpte_index, hpte_v, hpte_r, hpte_evicted_v, hpte_evicted_r;

	hpte_index = 0;
	hpte_group = (((va_addr >> 28) ^ ((va_addr & 0xFFFFFFFULL) >> 12)) & 0x7FF) << 3;
	hpte_v = ((va_addr >> 23) << 7) | HPTE_V_VALID;
	hpte_r = lpar_addr | 0x38 | (prot & HPTE_R_PROT_MASK);

	result = lv1_insert_htab_entry(0, hpte_group, hpte_v, hpte_r, HPTE_V_BOLTED, 0, &hpte_index, &hpte_evicted_v, &hpte_evicted_r);
	if (result != 0)
		return result;

	if (index)
		*index = hpte_index;

	return 0;
}

s64 mm_map_lpar_memory_region(u64 lpar_start_addr, u64 ea_start_addr, u64 size, u64 page_shift, u64 prot) {
	s64 result;
	s64 i;

	for (i = 0; i < (size >> page_shift); ++i) {
		result = mm_insert_htab_entry(MM_EA2VA(ea_start_addr), lpar_start_addr, prot, 0);
 		if (result != 0) 
 			return result;

		lpar_start_addr += (1 << page_shift);
		ea_start_addr += (1 << page_shift);
	}

	return 0;
}
