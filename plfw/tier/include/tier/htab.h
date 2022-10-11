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

#ifndef _TIER_HTAB_H
#define _TIER_HTAB_H

#include <tier/types.h>

/* hashed page table entry - 128 bits */
typedef union {
	struct {
		/* 1st part: 1st 64 bits starts here */
		u64 reserved : 15;  /* 0 */
		u64 avpn : 42;      /* abbreviated virtual page number (vsid | api == avpn) */
		u64 sw : 4;         /* available for software use */
		u64 l : 1;          /* large page indicator (1=large, 0=4kb) */
		u64 h : 1;          /* hash function identifier */
		u64 v : 1;          /* valid (1=valid, 0=invalid) */
	
		/* 2nd part: 2nd 64 bits starts here */
		u64 reserved2 : 22; /* 0 */
		u64 rpn : 30;       /* real page number */
		u64 reserved3 : 2;  /* 0 */
		u64 ac : 1;         /* address compare bit */
		u64 r : 1;          /* reference bit */
		u64 c : 1;          /* change bit */
		u64 w : 1;          /* write-through. hardware always treats this as '0' */
		u64 i : 1;          /* caching-inhibited bit */
		u64 m : 1;          /* memory coherence. Hardware always treats this as '0' */
		u64 g : 1;          /* guarded bit */
		u64 n : 1;          /* no execute bit (1=no-execute, 0=allow execute) */
		u64 pp : 2;         /* page protection bits */
	};
	u64 num[2];
} htabe_t;

#endif /* !_TIER_HTAB_H */
