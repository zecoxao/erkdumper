#ifndef _HVCALL_H
#define _HVCALL_H

#include "common.h"

s64 lv1_insert_htab_entry(u64 htab_id, u64 hpte_group, u64 hpte_v, u64 hpte_r, u64 bolted_flag, u64 flags, u64 *hpte_index, u64 *hpte_evicted_v, u64 *hpte_evicted_r);
s64 lv1_write_htab_entry(u64 vas_id, u64 hpte_index, u64 hpte_v, u64 hpte_r);
s64 lv1_allocate_memory(u64 size, u64 page_size_exp, u64 flags, u64 *addr, u64 *muid);
s64 lv1_undocumented_function_114(u64 start, u64 page_size, u64 size, u64 *lpar_addr);
s64 lv1_undocumented_function_115(u64 lpar_addr);
s64 lv1_panic(u64 flag);

#endif /* !_HVCALL_H */
