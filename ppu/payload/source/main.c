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

#include <tier/lv1.h>
#include <tier/lv2.h>
#include <tier/mm.h>
#include <tier/time.h>
#include <tier/types.h>
#include <tier/util.h>

#include "data.h"
#include "spu.h"

#define HV_BASE       0x8000000014000000ULL
#define HV_OFFSET     0x14000000ULL
#define HV_PAGE_SIZE  PAGE_SIZE_4KB
#define HV_SIZE       0x1000000ULL

#define METLDR_OFFSET 0x11000ULL

#define LDR_BASE      0x8000000015000000ULL
#define LDR_OFFSET    0x15000000ULL
#define LDR_PAGE_SIZE PAGE_SIZE_4KB
#define LDR_SIZE      0x30000ULL

#define METLDR_SIZE    0x10000
#define DUMPER_SIZE    0x10000
#define WORK_DATA_SIZE 0x10000

#define DUMP_LS_ADDR  0x3E000
#define DUMP_SIZE     0x30

#define SPU_SHADOW_BASE    0x80000000133D0000ULL
#define SPU_SHADOW_OFFSET  0x133D0000ULL
#define SPU_SHADOW_SIZE    0x1000UL

#define SPU_PROBLEM_BASE   0x8000000013400000ULL
#define SPU_PROBLEM_OFFSET 0x13400000ULL
#define SPU_PROBLEM_SIZE   0x20000UL

#define SPU_PRIV2_BASE   0x8000000013420000ULL
#define SPU_PRIV2_OFFSET 0x13420000ULL
#define SPU_PRIV2_SIZE   0x20000UL

#define SPU_LS_BASE      0x8000000013440000ULL
#define SPU_LS_OFFSET    0x13440000ULL
#define SPU_LS_SIZE      0x40000UL

#define failure(_msg) { \
		goto error; \
	}

s64 payload(u8 *output_data, u64 output_size) {
	s32 result = 0;

	u64 ticks;

	u8 *hv;
	u8 *ldr;
	u8 *metldr;
	u8 *dumper;
	u8 *work_data;

	u64 ldr_lpar_addr;
	u64 gameos_lpar_base, gameos_lpar_size;
	u64 vas_id, spu_id;
	u64 esid, vsid;
	u64 priv2_addr, problem_phys, local_store_phys, shadow_addr;
	u64 intr_status, unused;

	struct spu_shadow volatile *spu_shadow;
	struct spu_problem volatile *spu_problem;
	struct spu_priv2 volatile *spu_priv2;

	u8 volatile *spu_ls;
	u32 spu_out_intr_mbox_value, spu_out_mbox_value;
	u8 mfc_cmd_tag;

	u32 force_exit;

	metldr = NULL;
	dumper = NULL;
	work_data = NULL;

	gameos_lpar_base = *(u64 *)0x800000000035A728ULL;
	gameos_lpar_size = *(u64 *)0x800000000035A730ULL;

	vas_id = vas_get_id();
	ldr_lpar_addr = gameos_lpar_base + gameos_lpar_size - LDR_SIZE;

	MM_LOAD_BASE(hv, HV_OFFSET);
	MM_LOAD_BASE(ldr, LDR_OFFSET);

	result = mm_map_lpar_memory_region(0, MM_EA2VA(ldr), ldr_lpar_addr, LDR_SIZE, LDR_PAGE_SIZE, 0, 0);
	if (result != 0)
		failure();

	lv2_memset(ldr, 0, LDR_SIZE);

	metldr = ldr;
	dumper = ldr + METLDR_SIZE;
	work_data = dumper + DUMPER_SIZE;

	lv2_memcpy(metldr, hv + METLDR_OFFSET, METLDR_SIZE);
	lv2_memcpy(dumper, dumper_spu_self, dumper_spu_self_size);
	lv2_memset(work_data, 0, WORK_DATA_SIZE);

	result = lv1_construct_logical_spu(PAGE_SIZE_4KB, PAGE_SIZE_4KB, PAGE_SIZE_4KB, PAGE_SIZE_4KB, PAGE_SIZE_4KB, vas_id, 0, &priv2_addr, &problem_phys, &local_store_phys, &unused, &shadow_addr, &spu_id);
	if (result != 0)
		failure();

	result = lv1_enable_logical_spu(spu_id, 6);
	if (result != 0)
		failure();

	result = lv1_set_spu_interrupt_mask(spu_id, 0, 0x7);
	if (result != 0)
		failure();

	result = lv1_set_spu_interrupt_mask(spu_id, 1, 0xF);
	if (result != 0)
		failure();

	result = lv1_set_spu_interrupt_mask(spu_id, 2, 0xF);
	if (result != 0)
		failure();

	MM_LOAD_BASE(spu_shadow, SPU_SHADOW_OFFSET);

	result = mm_map_lpar_memory_region(0, MM_EA2VA(spu_shadow), shadow_addr, SPU_SHADOW_SIZE, PAGE_SIZE_4KB, 0, 0x3);
	if (result != 0)
		failure();

	MM_LOAD_BASE(spu_problem, SPU_PROBLEM_OFFSET);

	result = mm_map_lpar_memory_region(0, MM_EA2VA(spu_problem), problem_phys, SPU_PROBLEM_SIZE, PAGE_SIZE_4KB, 0, 0);
	if (result != 0)
		failure();

	MM_LOAD_BASE(spu_priv2, SPU_PRIV2_OFFSET);

	result = mm_map_lpar_memory_region(0, MM_EA2VA(spu_priv2), priv2_addr, SPU_PRIV2_SIZE, PAGE_SIZE_4KB, 0, 0);
	if (result != 0)
		failure();

	MM_LOAD_BASE(spu_ls, SPU_LS_OFFSET);

	result = mm_map_lpar_memory_region(0, MM_EA2VA(spu_ls), local_store_phys, SPU_LS_SIZE, PAGE_SIZE_4KB, 0, 0);
	if (result != 0)
		failure();

	result = lv1_set_spu_privilege_state_area_1_register(spu_id, MFC_SR1, 0x10);
	if (result != 0)
		failure();

	spu_slb_invalidate_all(spu_priv2);

	esid = 0x8000000018000000ULL;
	vsid = 0x0000000000001400ULL;
	spu_slb_set_entry(spu_priv2, 0, esid, vsid);

	spu_priv2->spu_cfg = 0;

	eieio();

	spu_in_mbox_write_64(spu_problem, (u64)dumper);

	spu_sig_notify_1_2_write_64(spu_problem, (u64)metldr);

	spu_iso_load_req_enable(spu_priv2);
	spu_iso_load_req(spu_problem);

	force_exit = 0;

	while (1) {
		if (force_exit)
			goto bad;

		result = lv1_get_spu_interrupt_status(spu_id, 0, &intr_status);
		if (result != 0)
			failure();

		if (intr_status) {
			result = lv1_clear_spu_interrupt_status(spu_id, 0, intr_status, 0);
			if (result != 0)
				failure();
		}

		result = lv1_get_spu_interrupt_status(spu_id, 1, &intr_status);
		if (result != 0)
			failure();
			
		if (intr_status) {
			result = lv1_clear_spu_interrupt_status(spu_id, 1, intr_status, 0);
			if (result != 0)
				failure();
		}

		result = lv1_get_spu_interrupt_status(spu_id, 2, &intr_status);
		if (result != 0)
			failure();

		if (intr_status) {
			result = lv1_clear_spu_interrupt_status(spu_id, 2, intr_status, 0);
			if (result != 0)
				failure();

			if (intr_status & 0x1) {
				if (spu_mbox_stat_intr_out_mbox_count(spu_problem) != 0) {
					spu_out_intr_mbox_value = spu_priv2->spu_out_intr_mbox;

					if (spu_out_intr_mbox_value == 1) {
						if (spu_mbox_stat_out_mbox_count(spu_problem) == 0)
							goto bad;

						spu_out_mbox_value = spu_problem->spu_out_mbox;
						if (spu_out_mbox_value != 1)
							break;

						ticks = 3 * TB_TICKS_PER_SEC;
						sleep(ticks);

						mfc_cmd_tag = 1;

						if (spu_mfc_cmd_exec(spu_problem, DUMP_LS_ADDR, (u64)work_data, DUMP_SIZE, mfc_cmd_tag, 0, MFC_CMD_PUT))
							goto bad;

						while (spu_mfc_cmd_tag_status(spu_problem, mfc_cmd_tag) == 0) {
							if (force_exit)
								goto bad;
						}

						force_exit = 1;
					} else if (spu_out_intr_mbox_value == 2) {
						spu_out_mbox_value = spu_problem->spu_out_mbox;

						force_exit = 1;
					}
				}
			}
		}

		if ((spu_problem->spu_status & 0x1) == 0)
			break;

		ticks = 1 * TB_TICKS_PER_SEC;
		sleep(ticks);
	}

bad:
	if (spu_shadow) {
	}

	if (spu_problem) {
		spu_iso_exit_req(spu_problem);
		spu_stop_req(spu_problem);
	}

	if (spu_priv2) {
	}

	result = lv1_destruct_logical_spu(spu_id);
	if (result != 0)
		failure();

	if (work_data) {
		lv2_copy_to_user(work_data, output_data, DUMP_SIZE);
		result = 0;
	}

error:
	ticks = 5 * TB_TICKS_PER_SEC;
	sleep(ticks);

	return result;
}
