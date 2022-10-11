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

#ifndef _TIER_ASM_UTIL_H_
#define _TIER_ASM_UTIL_H_

#ifndef XGLUE
#	define XGLUE(_a, _b) _a##_b
#endif
#ifndef GLUE
#	define GLUE(_a, _b) XGLUE(_a, _b)
#endif

#define GLOBAL(_name) \
		.section .func_##_name, "ax", @progbits; \
		.align 3; \
		.globl _name; \
		.globl ._name; \
		.section ".opd", "aw"; \
		.align 3; \
	_name: \
		.quad ._name; \
		.quad .TOC.@tocbase; \
		.quad 0; \
		.previous; \
		.type ._name, @function; \
	._name:

#define EXTERNAL(_offset, _name, _toc_base) \
		.section .func_##_name, "ax", @progbits; \
		.align 2; \
		.globl _name; \
		.globl ._name; \
		.section ".opd64", "aw"; \
		.align 2; \
	_name: \
		.quad _offset; \
		.quad _toc_base; \
		.previous; \
		.type ._name, @function; \
	._name: \
		mflr %r0; \
		std %r0, 16(%r1); \
		stdu %r1, -128(%r1); \
		\
		std %r2, 112(%r1); \
		\
		ld %r12, _name@got(%r2); \
		ld %r0, 0(%r12); \
		ld %r2, 8(%r12); \
		ld %r11, 16(%r12); \
		mtctr %r0; \
		bctrl; \
		\
		ld %r2, 112(%r1); \
		\
		addi %r1, %r1, 128; \
		ld %r0, 16(%r1); \
		mtlr %r0; \
		blr;

#endif /* !_TIER_ASM_UTIL_H_ */
