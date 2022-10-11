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

#include "types.h"

void *memcpy(void *dst, const void *src, u32 length) {
	u8 *d = (u8 *)dst;
	const u8 *s = (u8 *)src;
	u32 i;
	for (i = 0; i < length; ++i)
		d[i] = s[i];
	return dst;
}

int main() {
	/* Copy eid root key/iv to shared LS. */
	memcpy((u8 *)0x3E000, (const u8 *)0x00000, 0x30);

	/* Hang (the PPU should copy the key/iv from shared LS now). */
	for (;;) {
	}

	return 0;
}
