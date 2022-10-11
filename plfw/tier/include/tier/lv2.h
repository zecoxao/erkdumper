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

#ifndef _TIER_LV2_H_
#define _TIER_LV2_H_

#include <tier/types.h>

#define S_IFMT   0170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000
#define S_IRWXU  00700
#define S_IRUSR  00400
#define S_IWUSR  00200
#define S_IXUSR  00100
#define S_IRWXG  00070
#define S_IRGRP  00040
#define S_IWGRP  00020
#define S_IXGRP  00010
#define S_IRWXO  00007
#define S_IROTH  00004
#define S_IWOTH  00002
#define S_IXOTH  00001

#define O_RDONLY  000000
#define O_WRONLY  000001
#define O_RDWR    000002
#define O_ACCMODE 000003
#define O_CREAT   000100
#define O_EXCL    000200
#define O_TRUNC   001000
#define O_APPEND  002000
#define O_MSELF   010000

#define MAXPATHLEN 1024

typedef long time_t;

struct lv2_fs_stat {
	s32 st_mode;
	s32 st_uid;
	s32 st_gid;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
	u64 st_size;
	u64 st_blksize;
} __attribute__((packed));

struct lv2_fs_dirent {
	u8 d_type;
	u8 d_namlen;
	char d_name[MAXPATHLEN + 1];
} lv2_fs_dirent;

struct lv2_fs_utime {
	time_t actime;
	time_t modtime;
};

extern u64 lv2_get_current_proc(void);
extern s32 lv2_proc_get_pid(void);

extern s32 lv2_sys_open(const char *path, u32 flags, s64 *fd, u32 mode, void *args, u64 args_size);
extern s32 lv2_sys_read(s64 fd, void *ptr, u32 size, u64 *num_read);
extern s32 lv2_sys_write(s64 fd, const void *ptr, u32 size, u64 *num_written);
extern s32 lv2_sys_close(s64 fd);

extern s32 lv2_fs_open(const char *path, u32 flags, s64 *fd, u32 mode, void *args, u64 args_size);
extern s32 lv2_fs_read(s64 fd, void *ptr, u32 size, u64 *num_read);
extern s32 lv2_fs_write(s64 fd, const void *ptr, u32 size, u64 *num_written);
extern s32 lv2_fs_lseek(s64 fd, s64 offset, s32 whence, u64 *position);
extern s32 lv2_fs_stat(const char *path, struct lv2_fs_stat *stat);
extern s32 lv2_fs_close(s64 fd);

extern s32 lv2_strlen(const char *s);
extern char * lv2_strcpy(char *dst, const char *src);
extern void lv2_strncmp(const char *s1, const char *s2, s32 n);

extern void * lv2_memset(const void *ptr, s32 value, u64 size);
extern void * lv2_memcpy(const void *dst, const void *src, u64 size);
extern s32 lv2_memcmp(const void *s1, const void *s2, u64 size);

extern void * lv2_allocate(u64 size, u64 pool);
extern void lv2_deallocate(void *ptr, u64 pool);

extern void * lv2_allocate_0x35(u64 size);
extern void lv2_deallocate_0x35(u64 size);

extern s32 lv2_copy_from_user(const void *from, void *to, u64 size);
extern s32 lv2_copy_to_user(const void *from, void *to, u64 size);

extern s32 lv2_get_milliseconds(void *ptr1, void *ptr2);

extern s32 lv2_snprintf(char *s, u64 size, const char *format, ...);
extern s32 lv2_sprintf(char *s, const char *format, ...);
extern s32 lv2_printf(const char *format, ...);

#endif /* !_TIER_LV2_H_ */
