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

#ifndef _TIER_UTIL_H_
#define _TIER_UTIL_H_

#include <tier/types.h>

#define mb()     __asm__ __volatile__ ("sync" : : : "memory")
#define rmb()    __asm__ __volatile__ ("sync" : : : "memory")
#define wmb()    __asm__ __volatile__ ("sync" : : : "memory")
#define lwsync() __asm__ __volatile__ ("lwsync" : : : "memory")
#define eieio()  __asm__ __volatile__ ("eieio" : : : "memory")
#define nop()    __asm__ __volatile__ ("nop")

#define MAKE_FOURCC(_a, _b, _c, _d) \
	(((u32)(_a) << 24) | ((u32)(_b) << 16) | ((u32)(_c) << 8) | ((u32)(_d) << 0))

#define MIN(_a, _b) ((_a) <= (_b) ? (_a) : (_b))
#define MAX(_a, _b) ((_a) >= (_b) ? (_a) : (_b))

#define COUNTOF(_a) (sizeof(_a) / sizeof((_a)[0]))

#endif /* !_TIER_UTIL_H_ */
