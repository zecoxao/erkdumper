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

#ifndef _TIER_TYPES_H_
#define _TIER_TYPES_H_

#if !defined(JOIN_HELPER)
#	define JOIN_HELPER(_a, _b) _a##_b
#endif
#if !defined(JOIN)
#	define JOIN(_a, _b) JOIN_HELPER(_a, _b)
#endif

#if !defined(STRINGIFY_HELPER)
#	define STRINGIFY_HELPER(_x) #_x
#endif
#if !defined(STRINGIFY)
#	define STRINGIFY(_x) STRINGIFY_HELPER(_x)
#endif

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long int s64;
typedef unsigned long long int u64;

#if !defined(NULL)
#	define NULL ((void *)0)
#endif

#define UNIMPLEMENTED(...)

#define INLINE    inline
#define NO_INLINE __attribute__ ((noinline))

#define UNUSED __attribute__ ((unused))

#endif /* !_TIER_TYPES_H_ */
