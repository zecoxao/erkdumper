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

#include <tier/slb.h>
#include <tier/lv2.h>

static slbe_t slb[SLB_NUM_ENTRIES];

void slb_clear_entry(slbe_t *slbe) {
	lv2_memset(slbe, 0, sizeof(*slbe));
}

void slb_invalidate_entry(slbe_esid_t esid) {
	esid.esid1.zero = 0;
	esid.esid1.index = 0;
	
	__asm__ __volatile__ (
		"slbie %0\n"
		:
		: "r"(esid)
	);
}

void slb_move_to_entry(slbe_esid_t esid, slbe_vsid_t vsid, u32 index) {
	esid.esid1.index = index;

	__asm__ __volatile__ (
		"slbmte %0, %1\n"
		:
		: "r"(vsid), "r"(esid)
	);
}

void slb_move_from_entry_esid(slbe_esid_t *esid, u32 index) {
	slbe_esid_t slb_index;
	SLBE_ESID_ZERO(slb_index);
	slb_index.esid1.index = index;
	
	__asm__ __volatile__ (
		"slbmfee %0, %1\n"
		: "=r"(*esid)
		: "r"(slb_index)
	);
	
	esid->esid1.index = index;
}

void slb_move_from_entry_vsid(slbe_vsid_t *vsid, u32 index) {
	slbe_esid_t slb_index;
	SLBE_ESID_ZERO(slb_index);
	slb_index.esid1.index = index;
	
	__asm__ __volatile__ (
		"slbmfev %0, %1\n"
		: "=r"(*vsid)
		: "r"(slb_index)
	);
}

s32 slb_add_entry(u64 ea_addr, u64 va_addr, u64 flags) {
	return slb_add_entry_ex(ea_addr, va_addr, SLB_FLAGS_TO_KS(flags), SLB_FLAGS_TO_KP(flags), SLB_FLAGS_TO_N(flags), SLB_FLAGS_TO_L(flags), SLB_FLAGS_TO_C(flags));

#if 0
	u64 esid, vsid;
	int i;

	for (i = 0; i < SLB_NUM_ENTRIES; ++i) {
		__asm__ __volatile__ (
			"slbmfee %0, %1"
			: "=r"(esid)
			: "r"(i)
		);

		if ((esid & (1ULL << 27)) != 0)
			continue;

		esid = (ea_addr & ~((1ULL << 28) - 1)) | (1ULL << 27) | (i & 0xFFFULL);
		vsid = ((va_addr >> 16) & ~((1ULL << 12) - 1)) | 0x400ULL;

		__asm__ __volatile__ (
			"slbmte %0, %1"
			:
			: "r"(vsid), "r"(esid)
		);

		return 0;
	}

	return -1;
#endif
}

s32 slb_add_entry_ex(u64 ea_addr, u64 va_addr, u32 ks, u32 kp, u32 n, u32 l, u32 c) {
	slbe_t slbe;
	s32 slb_idx;
	
	/* invalidate any existing slb entry that is the same as
	   the entry being added to ensure there is only ever one
	   entry for the values being added */
	slb_clear_entry(&slbe);
	slbe.esid.esid1.esid = EA_TO_ESID(ea_addr);
	slb_invalidate_entry(slbe.esid);
	
	/* find the first unused (invalid) slb entry and then add
	   this new slb entry there */
	slb_read_all_entries();
	for (slb_idx = 0; slb_idx < SLB_NUM_ENTRIES; ++slb_idx) {
		if (!SLBE_IS_VALID(slb[slb_idx])) {
			/* setup the ESID values for the segment to add */
			slb_clear_entry(&slbe);
			slbe.esid.esid1.v = 1;
			slbe.esid.esid1.index = slb_idx;
			slbe.esid.esid1.esid = EA_TO_ESID(ea_addr);
			
			/* setup the VSID values for the segment to add */
			slbe.vsid.vsid1.ks = ks;
			slbe.vsid.vsid1.kp = kp;
			slbe.vsid.vsid1.n = n;
			slbe.vsid.vsid1.l = l;
			slbe.vsid.vsid1.l = c;
			slbe.vsid.vsid1.vsid = VA_TO_VSID(va_addr);
			
			slb_move_to_entry(slbe.esid, slbe.vsid, slb_idx);
			return 0;
		}
	}

	return -1;
}

void slb_read_all_entries(void) {
	s32 slb_idx;
	for (slb_idx = 0; slb_idx < SLB_NUM_ENTRIES; ++slb_idx) {
		slb_move_from_entry_esid(&slb[slb_idx].esid, slb_idx);
		slb_move_from_entry_vsid(&slb[slb_idx].vsid, slb_idx);
	}
}

u64 slb_calc_kernel_vsid(u64 ea_addr) {
	u64 proto_vsid = EA_TO_ESID(ea_addr);
	return (proto_vsid * 0xBF6E61BULL) % 0xFFFFFFFFFULL;
}

u64 slb_calc_user_vsid(u64 ea_addr, u64 context) {
	u64 proto_vsid = (context << 16) | EA_TO_ESID(ea_addr);
	return (proto_vsid * 0xBF6E61BULL) % 0xFFFFFFFFFULL;
}
