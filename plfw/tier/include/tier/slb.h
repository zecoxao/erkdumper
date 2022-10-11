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

#ifndef _TIER_SLB_H
#define _TIER_SLB_H

#include <tier/types.h>

/* ESID part of SLB entry */
typedef union {
	struct {
		u64 esid : 36;  /* effective segment ID */
		u64 v : 1;      /* valid */
		u64 zero : 15;  /* must be set to 0 */
		u64 index : 12; /* index to select the SLB entry */
	}esid1;
	u64 num;
} slbe_esid_t;

/* VSID part of SLB entry */
typedef union {
	struct {
		u64 vsid : 52; /* virtual segment ID */
		u64 ks : 1;    /* supervisor key */
		u64 kp : 1;    /* problem key */
		u64 n : 1;     /* no-execute */
		u64 l : 1;     /* large page indicator */
		u64 c : 1;     /* class */
		u64 zero : 7;  /* must be set to 0 */
	}vsid1;
	u64 num;
} slbe_vsid_t;

/* SLB entry */
typedef struct {
	slbe_esid_t esid;
	slbe_vsid_t vsid;
} slbe_t;

#define SLB_NUM_ENTRIES 64

#define SLBE_IS_VALID(_slbe) ((_slbe).esid.esid1.v != 0)

#define SLBE_VSID_ZERO(_vsid) _vsid.num = 0
#define SLBE_ESID_ZERO(_esid) _esid.num = 0

#define SLBE_GET_EA(_slbe)      (((u64)(_slbe).esid.esid) << 28)
#define SLBE_SET_EA(_slbe, _ea) (_slbe).esid.esid = ((_ea) >> 28)
#define ESID_TO_EA(_esid)       ((u64)(_esid) << 28)
#define EA_TO_ESID(_ea)         ((_ea) >> 28)

#define SLBE_GET_VA(_slbe)      (((u64)(_slbe).vsid.vsid) << 12)
#define SLBE_SET_VA(_slbe, _va) (_slbe).vsid.vsid = ((_va) >> 12)
#define VSID_TO_VA(_vsid)       ((u64)(_vsid) << 12)
#define VA_TO_VSID(_va)         ((_va) >> 12)

#define SLBE_KS 0x800
#define SLBE_KP 0x400
#define SLBE_N  0x200
#define SLBE_L  0x100
#define SLBE_C  0x080

#define SLBE_KERNEL (SLBE_KP | SLBE_C)
#define SLBE_USER   (SLBE_KS | SLBE_KP)

#define SLB_FLAGS_TO_KS(_flags) (((_flags) & SLBE_KS) ? 1 : 0)
#define SLB_FLAGS_TO_KP(_flags) (((_flags) & SLBE_KP) ? 1 : 0)
#define SLB_FLAGS_TO_N(_flags)  (((_flags) & SLBE_N) ? 1 : 0)
#define SLB_FLAGS_TO_L(_flags)  (((_flags) & SLBE_L) ? 1 : 0)
#define SLB_FLAGS_TO_C(_flags)  (((_flags) & SLBE_C) ? 1 : 0)

void slb_clear_entry(slbe_t *slbe);
void slb_invalidate_entry(slbe_esid_t esid);

void slb_move_to_entry(slbe_esid_t esid, slbe_vsid_t vsid, u32 index);
void slb_move_from_entry_esid(slbe_esid_t *esid, u32 index);
void slb_move_from_entry_vsid(slbe_vsid_t *vsid, u32 index);

s32 slb_add_entry(u64 ea_addr, u64 va_addr, u64 flags);
s32 slb_add_entry_ex(u64 ea_addr, u64 va_addr, u32 ks, u32 kp, u32 n, u32 l, u32 c);

void slb_read_all_entries(void);

u64 slb_calc_kernel_vsid(u64 ea);
u64 slb_calc_user_vsid(u64 ea, u64 context);

#endif /* !_TIER_SLB_H */
