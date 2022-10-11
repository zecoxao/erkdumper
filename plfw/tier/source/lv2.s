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

#define LV2_TOC_BASE 0x8000000000330540
#define LV2_EXTERNAL(_offset, _name) EXTERNAL(_offset, _name, LV2_TOC_BASE)

.text

LV2_EXTERNAL(0x800000000001409C, lv2_get_current_proc)
LV2_EXTERNAL(0x8000000000013F88, lv2_proc_get_pid)

LV2_EXTERNAL(0x8000000000195130, lv2_sys_open)
LV2_EXTERNAL(0x800000000019031C, lv2_sys_read)
LV2_EXTERNAL(0x80000000001901E0, lv2_sys_write)
LV2_EXTERNAL(0x8000000000193500, lv2_sys_close)

LV2_EXTERNAL(0x80000000002B5438, lv2_fs_open)
LV2_EXTERNAL(0x80000000002B34E0, lv2_fs_read)
LV2_EXTERNAL(0x80000000002B344C, lv2_fs_write)
LV2_EXTERNAL(0x80000000002B2CA0, lv2_fs_lseek)
LV2_EXTERNAL(0x80000000002B2D58, lv2_fs_stat)
LV2_EXTERNAL(0x80000000002B33A4, lv2_fs_close)

LV2_EXTERNAL(0x800000000004E4D8, lv2_memset)
LV2_EXTERNAL(0x800000000007C3A4, lv2_memcpy)
LV2_EXTERNAL(0x800000000004D7E8, lv2_memcmp)

LV2_EXTERNAL(0x800000000004E6AC, lv2_strlen)
LV2_EXTERNAL(0x800000000004E684, lv2_strcpy)
LV2_EXTERNAL(0x800000000004E6D8, lv2_strncmp)

LV2_EXTERNAL(0x8000000000060B78, lv2_allocate)
LV2_EXTERNAL(0x8000000000060FB4, lv2_deallocate)

LV2_EXTERNAL(0x800000000007D09C, lv2_allocate_0x35)
LV2_EXTERNAL(0x800000000007CCD0, lv2_deallocate_0x35)

LV2_EXTERNAL(0x800000000000F8C0, lv2_copy_from_user)
LV2_EXTERNAL(0x800000000000F6A4, lv2_copy_to_user)

LV2_EXTERNAL(0x80000000001F23C0, lv2_get_milliseconds)

LV2_EXTERNAL(0x800000000004F86C, lv2_snprintf)
LV2_EXTERNAL(0x800000000004F900, lv2_sprintf)
LV2_EXTERNAL(0x800000000029285C, lv2_printf)
