/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "typedefs.h"
#include "platform.h"
#include "blkdev.h"
#include "partition.h"
#include "gfh.h"
#include "dram_buffer.h"

#define MOD "PART"

#define TO_BLKS_ALIGN(size, blksz)  (((size) + (blksz) - 1) / (blksz))

u8 g_cur_part_name[32] = {0};

typedef union {
    part_hdr_t      part_hdr;
    gfh_file_info_t file_info_hdr;
} img_hdr_t;

#define IMG_HDR_BUF_SIZE 512

#define img_hdr_buf (g_dram_buf->img_hdr_buf)
#define part_num (g_dram_buf->part_num)
#define part_info (g_dram_buf->part_info)

#define partition_info  (g_dram_buf->partition_info)
#define meta_info       (g_dram_buf->meta_info)

extern int read_gpt(part_t *part);

static int (*read_partition[])(part_t *) = {
    read_gpt,
    NULL
};

static void if_overlap_with_dram_buffer(u32 img_start, u32 img_end)
{
    u32 dram_buf_start, dram_buf_end;

    dram_buf_start = (u32)g_dram_buf;
    dram_buf_end = (u32)g_dram_buf + sizeof(dram_buf_t); 
#if 0
    print("[%s]dram_buf_start=0x%x\n", MOD, dram_buf_start);
    print("[%s]dram_buf_end=0x%x\n", MOD, dram_buf_end);
    print("[%s]img_start=0x%x\n", MOD, img_start);   
    print("[%s]img_end=0x%x\n", MOD, img_end);
#endif    
    //check image start
    if ((dram_buf_start <= img_start) && (img_start < dram_buf_end) ||
        (dram_buf_start < img_end) && (img_end <= dram_buf_end)) {
        print("[%s] dram_buf overlap\n", MOD);
        ASSERT(0);
    }
    // to check more if img_start < dram_buf_start < dram_buf_end < img_end
    if ((img_start <= dram_buf_start ) && (dram_buf_start < img_end) ||
        (img_start < dram_buf_end) && (dram_buf_end <= img_end)) {
        print("[%s] dram_buf overlap\n", MOD);
        ASSERT(0);
    }
}

int part_init(void)
{
    int err, i;
    part_num = 0;
    memset(part_info, 0x00, sizeof(part_info));

    memset(partition_info, 0x0, sizeof(partition_info));
    memset(meta_info, 0x0, sizeof(meta_info));

    err = 0;
    i = 0;
    while (read_partition[i]) {
        err = read_partition[i](partition_info);
        if (!err) {
            break;
        }
        i++;
    }

    return 0;
}


static part_t tempart;
static struct part_meta_info temmeta;

part_t *get_part(char *name)
{
    part_t *part = partition_info;
    part_t *ret = NULL;

    while (part->nr_sects) {
        if (part->info) {
            if (!strcmp(name, part->info->name)) {
                memcpy(&tempart, part, sizeof(part_t));
                memcpy(&temmeta, part->info, sizeof(struct part_meta_info));
                tempart.info = &temmeta;
                ret = &tempart;
                break;
            }
        }
        part++;
    }

    return ret;
}

part_t *part_get(char *name)
{
    /* this api corrupts if length of name is > 32 */
    memcpy(g_cur_part_name, name, 32);
    return get_part(name);
}

void put_part(part_t *part)
{
    return;
}

/* retun > 0 if active bit exists, return = 0 if active bit is not existed */
unsigned long mt_part_get_active_bit(part_t *part)
{
    return (part->part_attr & PART_ATTR_LEGACY_BIOS_BOOTABLE);
}

unsigned long mt_part_get_start_addr(part_t *part)
{
    return part->start_sect*512;
}

unsigned long mt_part_get_size(part_t *part)
{
    return part->nr_sects*512;
}

unsigned int mt_part_get_part_id(part_t *part)
{
    return part->part_id;
}

int get_part_info(u8 *buf, u32 *maddr, u32 *dsize, u32 *mode, u8 bMsg)
{
    int ret = 0;
    part_hdr_t *part_hdr = (part_hdr_t *)buf;

    if (part_hdr->info.magic == PART_MAGIC) {

        /* load image with partition header */
        part_hdr->info.name[31] = '\0';

        if (bMsg) {
            print("[%s] partition hdr (1)\n", MOD);
            print("[%s] Image with part header\n", MOD);
            print("[%s] name : %s\n", MOD, part_hdr->info.name);
            print("[%s] addr : %xh mode : %d\n", MOD, part_hdr->info.maddr, part_hdr->info.mode);
            print("[%s] size : %d\n", MOD, part_hdr->info.dsize);
            print("[%s] magic: %xh\n", MOD, part_hdr->info.magic);
        }
    
        *maddr = part_hdr->info.maddr;
        *dsize = part_hdr->info.dsize;
        *mode = part_hdr->info.mode;
	
    } else {
        if (bMsg) {
            print("[%s] partition hdr (0)\n", MOD);
        }
        return -1;
    }

    return ret;
}

int part_load(blkdev_t *bdev, part_t *part, u32 *addr, u32 offset, u32 *size)
{
    int ret;
    img_hdr_t *hdr = (img_hdr_t *)img_hdr_buf;
    part_hdr_t *part_hdr = &hdr->part_hdr;
    gfh_file_info_t *file_info_hdr = &hdr->file_info_hdr;
    unsigned int sec_policy_idx = 0;
    unsigned int img_auth_required = 0;

    /* specify the read offset */
    u64 src = (u64)(part->start_sect * bdev->blksz) + offset;
    u32 dsize = 0, maddr = 0, mode = 0;
    u32 ms;

#ifdef MTK_SECURITY_SW_SUPPORT
    /* get security policy of current partition */
    print("[%s] partition name = %s\n", MOD, g_cur_part_name);
    sec_policy_idx = get_policy_entry_idx(g_cur_part_name);
    img_auth_required = get_vfy_policy(sec_policy_idx);
    print("[%s] img_auth_required = %x\n", MOD, img_auth_required);
#endif

    /* retrieve partition header. */
    if (blkdev_read(bdev, src, sizeof(img_hdr_t), (u8*)hdr, part->part_id) != 0) {
        //print("[%s] bdev(%d) read error (%s)\n", MOD, bdev->type, part->name);
        return -1;
    }

    ret = get_part_info((u8 *)part_hdr, &maddr, &dsize, &mode, 1);
    if (ret) {
        print("[%s] image doesn't exist\n", MOD);
        return ret;
    }

#ifdef MTK_SECURITY_SW_SUPPORT
    ms = get_timer(0);
    if (img_auth_required) {
        sec_malloc_buf_reset();
        ret = sec_img_auth_init(g_cur_part_name, part_hdr->info.name);
        if (ret) {
            print("[%s] cert chain vfy fail...\n", MOD);
            ASSERT(0);
        }
    }
    ms = get_timer(ms);
    print("[%s] part: %s img: %s cert vfy(%d ms)\n", MOD, g_cur_part_name, part_hdr->info.name, ms);
#endif

    /* discard partition header */
    src += sizeof(part_hdr_t);

	memcpy(part_info + part_num, part_hdr, sizeof(part_hdr_t));
	part_num++;

    if (maddr == PART_HEADER_DEFAULT_ADDR) {
        maddr = *addr;
#if CFG_ATF_SUPPORT
    } else if (mode == LOAD_ADDR_MODE_BACKWARD) {
        maddr = tee_get_load_addr(maddr);
#elif CFG_TRUSTONIC_TEE_SUPPORT
    } else if (mode == LOAD_ADDR_MODE_BACKWARD) {
        maddr = tee_get_load_addr(maddr);
#endif
    }

    if_overlap_with_dram_buffer((u32)maddr, ((u32)maddr + dsize));

    ms = get_timer(0);
    if (0 == (ret = blkdev_read(bdev, src, dsize, (u8*)maddr, part->part_id))) {
        if (addr) *addr = maddr;
        if (size) *size = dsize;
    }
    ms = get_timer(ms);

    print("[%s] load \"%s\" from 0x%llx (dev) to 0x%x (mem) [%s]\n", MOD,
        part->info->name, src, maddr, (ret == 0) ? "SUCCESS" : "FAILED");

    ms = (ms == 0) ? 1 : ms;

    print("[%s] load speed: %dKB/s, %d bytes, %dms\n", MOD, ((dsize / ms) * 1000) / 1024, dsize, ms);

#ifdef MTK_SECURITY_SW_SUPPORT
    ms = get_timer(0);
    if (img_auth_required) {
        unsigned int cert_chain_sz = 0;
        unsigned int padding_sz = 0;
        print("[%s] img vfy...", MOD);
        ret = sec_img_auth(maddr, dsize);
        if (ret) {
            print("fail\n");
            ASSERT(0);
        }
        else {
            print("ok\n");
        }
    }
    ms = get_timer(ms);
    print("[%s] part: %s img: %s vfy(%d ms)\n", MOD, g_cur_part_name, part_hdr->info.name, ms);

    ret = sec_get_img_size(g_cur_part_name, part_hdr->info.name, size);
    if (ret)
        return ret;
#endif

    return ret;
}

int part_load_raw_part(blkdev_t *bdev, part_t *part, u32 *addr, u32 offset, u32 *size)
{
    int ret;
    img_hdr_t *hdr = (img_hdr_t *)img_hdr_buf;
    part_hdr_t *part_hdr = &hdr->part_hdr;
    gfh_file_info_t *file_info_hdr = &hdr->file_info_hdr;

    /* specify the read offset */
    u64 src = (u64)(part->start_sect * bdev->blksz) + offset;
    u32 dsize = 0, maddr = 0;
    u32 ms;

    ret = 0;
    /* retrieve partition header. */
    if (blkdev_read(bdev, src, *size, (u8*)(*addr), part->part_id) != 0) {
        //print("[%s] bdev(%d) read error (%s)\n", MOD, bdev->type, part->name);
        ret = -1;
    }

    return ret;
}


void part_dump(void)
{
    blkdev_t *bdev;
    part_t *part;
    u32 blksz;
    u64 start, end;

    bdev = blkdev_get(CFG_BOOT_DEV);
    part = partition_info;
    blksz = bdev->blksz;

    print("\n[%s] blksz: %dB\n", MOD, blksz);
    while (part->nr_sects) {
        start = (u64)part->start_sect * blksz;
        end = (u64)(part->start_sect + part->nr_sects) * blksz - 1;
        print("[%s] [0x%llx-0x%llx] \"%s\" (%d blocks) \n", MOD, start, end,
            (part->info) ? (char *)part->info->name : "unknown", part->nr_sects);
        part++;
    }
}
