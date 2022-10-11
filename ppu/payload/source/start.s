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

#include <tier/asm_util.h>

.text
.section .start, "ax", @progbits
.align 3

.global _start
_start:
	mflr %r0
	std %r0, 0x10(%r1)
	stdu %r1, -0x80(%r1)

	std %r2, 0x28(%r1)

	li %r2, 1
	sldi %r2, %r2, 63
	oris %r2, %r2, (__toc_start + 0x8000)@h
	ori %r2, %r2, (__toc_start + 0x8000)@l

	bl payload

	ld %r2, 0x28(%r1)

	addi %r1, %r1, 0x80
	ld %r0, 0x10(%r1)
	mtlr %r0
	blr
