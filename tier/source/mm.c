/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <tier/mm.h>
#include <tier/lv1.h>
#include <tier/util.h>

s64 mm_insert_htab_entry(u64 vas_id, u64 va_addr, u64 lpar_addr, u64 page_shift, u64 vflags, u64 rflags, u64 *index) {
	s64 result;

	u64 hpte_group, hpte_index, hpte_v, hpte_r, hpte_evicted_v, hpte_evicted_r;

	hpte_index = 0;
	hpte_group = (((va_addr >> 28) ^ ((va_addr & 0xFFFFFFFULL) >> page_shift)) & HTAB_HASH_MASK) * HPTES_PER_GROUP;

	hpte_v = ((va_addr >> 23) << HPTE_V_AVPN_SHIFT) | HPTE_V_VALID | (vflags & HPTE_V_FLAGS_MASK);
	if (page_shift != PAGE_SIZE_4KB)
		hpte_v |= HPTE_V_LARGE;

	hpte_r = (lpar_addr & ~((1ULL << page_shift) - 1)) | (rflags & HPTE_R_FLAGS_MASK);
	if (page_shift == PAGE_SIZE_1MB)
		hpte_r |= (1 << 12);

	result = lv1_insert_htab_entry(vas_id, hpte_group, hpte_v, hpte_r, HPTE_V_BOLTED, 0, &hpte_index, &hpte_evicted_v, &hpte_evicted_r);
	if (result != 0)
		return result;

	if (index)
		*index = hpte_index;

	return 0;
}

s64 mm_map_lpar_memory_region(u64 vas_id, u64 va_start_addr, u64 lpar_start_addr, u64 size, u64 page_shift, u64 vflags, u64 rflags) {
	s64 result;

	s64 i;
	for (i = 0; i < (size >> page_shift); ++i) {
		result = mm_insert_htab_entry(vas_id, va_start_addr, lpar_start_addr, page_shift, vflags, rflags, 0);
 		if (result != 0) 
 			return result;

		va_start_addr += 1 << page_shift;
		lpar_start_addr += 1 << page_shift;
	}

	return 0;
}

s64 mm_lpar_addr_to_ra(u64 lpar_addr, u64 *ra) {
	s64 result;

	static volatile u64 va_addr = 0x5000000000000ULL;

	u64 hpte_index, hpte_v, hpte_r, *htab;

	hpte_index = GAMEOS_HTAB_SIZE / 16 - 1;

	result = lv1_write_htab_entry(0, hpte_index, 0, 0);
	if (result != 0)
		return result;

	hpte_v = ((va_addr >> 23) << HPTE_V_AVPN_SHIFT) | HPTE_V_VALID;
	hpte_r = lpar_addr & ~((1ULL << 12) - 1);

	result = lv1_write_htab_entry(0, hpte_index, hpte_v, hpte_r);
	if (result != 0)
		return result;

	MM_LOAD_BASE(htab, GAMEOS_HTAB_OFFSET);

	if (ra) {
		*ra = (htab[hpte_index * 2 + 1] & ~((1ULL << 12) - 1)) + (lpar_addr & ((1ULL << 12) - 1));
	}

	result = lv1_write_htab_entry(0, hpte_index, 0, 0);
	if (result != 0)
		return result;

	return 0;
}

s64 mm_phys_ra_to_lpar_addr(u64 ra, u64 *lpar_addr) {
	static struct {
		u64 lpar_addr;
		u64 size;
		s32 num_regions;

		struct {
			u64 ra;
			u64 size;
		} ra_regions[8];
	} lpar_ra_regions[] = {
		{
			0x0000000000000000ULL,
			0x1000000ULL,
			1,
			{
				{ 0x1000000ULL, 0x1000000ULL },
			}
		},
		{
			0x500000300000ULL,
			0xA0000ULL,
			5,
			{
				{ 0x380000ULL, 0x10000ULL },
				{ 0x3B0000ULL, 0x10000ULL },
				{ 0x1E0000ULL, 0x20000ULL },
				{ 0x3C0000ULL, 0x40000ULL },
				{ 0xFF00000ULL, 0x20000ULL },
			}
		},
		{
			0x700020000000ULL,
			0xE900000ULL,
			3,
			{
				{ 0x400000ULL, 0x200000ULL },
				{ 0x800000ULL, 0x800000ULL },
				{ 0x2000000ULL, 0xDF00000ULL },
			}
		},
	};

	u64 ra_region_lpar_addr, ra_region_start, ra_region_size;
	s32 i, j;

	for (i = 0; i < COUNTOF(lpar_ra_regions); ++i) {
		ra_region_lpar_addr = lpar_ra_regions[i].lpar_addr;

		for (j = 0; j < lpar_ra_regions[i].num_regions; ++j) {
			ra_region_start = lpar_ra_regions[i].ra_regions[j].ra;
			ra_region_size = lpar_ra_regions[i].ra_regions[j].size;

			if ((ra >= ra_region_start) && (ra < (ra_region_start + ra_region_size))) {
				if (lpar_addr)
					*lpar_addr = ra_region_lpar_addr + (ra - ra_region_start);

				return 0;
			}

			ra_region_lpar_addr += ra_region_size;
		}
	}

	return -1;
}
