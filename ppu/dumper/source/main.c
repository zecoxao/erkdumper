#include "common.h"
#include "hvcall.h"
#include "mm.h"
#include "peek_poke.h"

#define HTAB_BASE       0x800000000F000000ULL
#define HTAB_SIZE       0x40000ULL
#define HTAB_ENTRY_SIZE 0x10

#define SC_TABLE_OFFSET 0x8000000000346570ULL
#define SC_SM_SHUTDOWN  379
#define SC_RING_BUZZER  392
#define SC_MAGIC        699

#define PAYLOAD_OFFSET  0x80000000007F0000ULL
#define REAL_OFFSET     0x80000000002904B0ULL

#define DUMP_SIZE 0x30

static const char *dump_file_name = "/dev_hdd0/tmp/eid_root_key";

static u64 mmap_lpar_addr;

static u8 __attribute__ ((aligned ((128)))) dump[1024];

extern const long int payload_size;
extern const unsigned char payload[];

static s64 map_lv1() {
	s64 result;

	result = lv1_undocumented_function_114(0, HV_PAGE_SIZE, HV_SIZE, &mmap_lpar_addr);
	if (result != 0)
		return result;
	
	result = mm_map_lpar_memory_region(mmap_lpar_addr, HV_BASE, HV_SIZE, HV_PAGE_SIZE, 0);
	if (result != 0)
		return result;
	
	return 0;
}

static s64 unmap_lv1() {
	if (mmap_lpar_addr != 0)
		lv1_undocumented_function_115(mmap_lpar_addr);

	return 0;
}

static void patch_lv2_protection() {
	/* patching system manager to disable integrity check */
	lv1_poke(0x44D0B0, 0x409E004838600000ULL);

	/* patching hypervisor to enable skipping of acl checks for all storage devices */
	lv1_poke(0x25B340, 0x386000012F830000ULL);
	lv1_poke(0x25B34C, 0x38000001540007FEULL);

	/* patching hypervisor to allow setting all bits of SPE register MFC_SR1 */
	lv1_poke(0x2F2678, 0x3920FFFFE9430000ULL);

	/* patching hypervisor to allow mapping of htab with write protection */
	lv1_poke(0x2D5EB0, 0x60000000E97F00A8ULL);

	/* changes protected area of lv2 to first byte only */
	lv1_poke(0x363A78, 0x0000000000000001ULL);
	lv1_poke(0x363A80, 0xE0D251B556C59F05ULL);
	lv1_poke(0x363A88, 0xC232FCAD552C80D7ULL);
	lv1_poke(0x363A90, 0x65140CD200000000ULL);

	/* patching hypervisor to disallow lv2 modification panic */
	lv1_poke(0x2AE21C, 0x2F83003360000000ULL);
}

static s64 patch_htab_entry(u64 vas_id, u64 hpte_index) {
	s64 result;

	u64 hpte_offset = HTAB_BASE + HTAB_ENTRY_SIZE * hpte_index;
	u64 hpte_pte0 = lv2_peek(hpte_offset + 0);
	u64 hpte_pte1 = lv2_peek(hpte_offset + HTAB_ENTRY_SIZE / 2);
	u64 hpte_va = hpte_pte0;
	u64 hpte_pa = (hpte_pte1 & 0xFF0000ULL) | HPTE_R_R | HPTE_R_C | HPTE_R_M;

	result = lv1_write_htab_entry(vas_id, hpte_index, hpte_va, hpte_pa);
	if (result != 0)
		return result;

	return 0;
}

static s64 patch_htab_group_entry(u64 vas_id, u64 hpte_group, u64 hpte_group_index) {
	return patch_htab_entry(vas_id, hpte_group * HPTES_PER_GROUP + hpte_group_index);
}

static s64 patch_htab_entries(u64 vas_id) {
	s64 result;

	const u64 num_hpte_groups = 128;
	u64 hpte_group;

	for (hpte_group = 0; hpte_group < num_hpte_groups; ++hpte_group) {
		result = patch_htab_group_entry(vas_id, hpte_group, 0);
		if (result != 0)
			return result;
	}

	return 0;
}

static inline s32 lv2_ring_buzzer(u64 unknown1, u8 unknown2, u32 unknown3) {
	lv2syscall3(SC_RING_BUZZER, unknown1, unknown2, unknown3);
	return_to_user_prog(s32);
}

static inline s32 lv2_do_magic(u8 *data, u64 size) {
	lv2syscall2(SC_MAGIC, (u64)data, (u64)size);
	return_to_user_prog(s32);
}

static inline s32 lv2_reboot() {
	lv2syscall4(SC_SM_SHUTDOWN, 0x200, 0, 0, 0);
	return_to_user_prog(s32);
}

static s64 install_hook() {
	if (payload_size > 0) {
		u64 magic_offset;
		lv2_copy_memory(PAYLOAD_OFFSET, (const void *)payload, payload_size);
		magic_offset = lv2_peek(SC_TABLE_OFFSET + SC_MAGIC * sizeof(u64));
		lv2_poke(magic_offset + 0, PAYLOAD_OFFSET);

		return 0;
	}

	return -1;
}

static s64 remove_hook() {
	if (payload_size > 0) {
		u64 magic_offset;

		magic_offset = lv2_peek(SC_TABLE_OFFSET + SC_MAGIC * sizeof(u64));
		lv2_poke(magic_offset + 0, REAL_OFFSET);

		return 0;
	}

	return -1;
}

s32 main(int argc, char *argv[]) {
	s32 hook_installed;

	FILE *fp;

	install_new_poke();
	
	if (map_lv1() != 0) {
		remove_new_poke();
		return 0;
	}

	patch_lv2_protection();

	if (patch_htab_entries(0) == 0)
		hook_installed = install_hook() == 0;
	else
		hook_installed = 0;

	if (hook_installed) {
		s64 result;

		memset(dump, 0, sizeof(dump));
		result = lv2_do_magic(dump, sizeof(dump));

		hook_installed = remove_hook() == 0;

		if (result == 0) {
			fp = fopen(dump_file_name, "wb");
			if (fp) {
				fwrite(dump, 1, DUMP_SIZE, fp);
				fclose(fp);
			}
		}
	}

	unmap_lv1();
	remove_new_poke();

	lv2_ring_buzzer(0x1004, 0x0A, 0x1B6);
	lv2_reboot();

	return 0;
}
