#include <stdint.h>
#include <printf.h>
#include <malloc.h>
#include <string.h>
#include <platform/errno.h>
#include <platform/mmc_core.h>
#include <platform/partition.h>
#include <target.h>
#include <platform/partition_fastboot.h>


#define TAG "[PART_LK]"

#define LEVEL_ERR   (0x0001)
#define LEVEL_INFO  (0x0004)

#ifdef USER_BUILD
#define DEBUG_LEVEL 0
#else
#define DEBUG_LEVEL (LEVEL_ERR | LEVEL_INFO)
#endif

#define part_err(fmt, args...)   \
do {    \
    if (DEBUG_LEVEL & LEVEL_ERR) {  \
        dprintf(CRITICAL, fmt, ##args); \
    }   \
} while (0)

#define part_info(fmt, args...)  \
do {    \
    if (DEBUG_LEVEL & LEVEL_INFO) {  \
        dprintf(CRITICAL, fmt, ##args);    \
    }   \
} while (0)


part_t tempart;
struct part_meta_info temmeta;

part_t *partition;

static part_dev_t *mt_part_dev;

#if defined(MTK_EMMC_SUPPORT) || defined(MTK_NAND_SUPPORT)
static uchar *mt_part_buf;
#endif

part_t* get_part(char *name)
{
    part_t *part = partition;
    part_t *ret = NULL;

	part_info("%s[%s] %s\n", TAG, __FUNCTION__, name);
	while (part->nr_sects) {
        if (part->info) {
		    if (!strcmp(name, (char *)part->info->name)) {
                memcpy(&tempart, part, sizeof(part_t));
                memcpy(&temmeta, part->info, sizeof(struct part_meta_info));
                tempart.info = &temmeta;
                ret = &tempart;
                break;
            }
        }
		part++;

        //part_info("%s[%s] 0x%lx\n", TAG, __FUNCTION__, tempart.start_sect);
	}
    return ret;
}

void put_part(part_t *part)
{
    if (!part) {
        return;
    }

    if (part->info) {
        free(part->info);
    }
    free(part);
}

unsigned long mt_part_get_part_active_bit(part_t *part)
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
extern int mmc_get_boot_part(int *bootpart);
part_t *mt_part_get_partition(char *name)
{
    int bootpart=1; //default bootpart = 1
    if (!strcmp(name, "PRELOADER") || !strcmp(name, "preloader")) {
        tempart.start_sect = 0x0;
        tempart.nr_sects = 0x200;
        mmc_get_boot_part(&bootpart); // get active boot partition
        if(bootpart == 0x1)
        {
            tempart.part_id = 1;
        }
        else
        {
            tempart.part_id = 2;
        }
        return &tempart;
    }
    return get_part(name);
}

#ifdef MTK_EMMC_SUPPORT
void mt_part_dump(void)
{
    part_t *part = partition;

    part_info("Part Info.(1blk=%dB):\n", BLK_SIZE);
    while (part->nr_sects) {
    	 part_info("[0x%016llx-0x%016llx] (%.8ld blocks): \"%s\"\n",
               (u64)part->start_sect * BLK_SIZE,
              (u64)(part->start_sect + part->nr_sects) * BLK_SIZE - 1,
                               part->nr_sects, (part->info) ? (char *)part->info->name : "unknown");
        part++;
    }
    part_info("\n");
}

int mt_part_generic_read(part_dev_t *dev, u64 src, uchar *dst, int size, unsigned int part_id)
{
    int dev_id = dev->id;
    uchar *buf = &mt_part_buf[0];
    block_dev_desc_t *blkdev = dev->blkdev;
    u64 end, part_start, part_end, part_len, aligned_start, aligned_end;
    ulong blknr, blkcnt;

	if (!blkdev) {
        part_err("No block device registered\n");
        return -ENODEV;
	}

	if (size == 0) {
		return 0;
    }

	end = src + size;

	part_start    = src &  ((u64)BLK_SIZE - 1);
	part_end      = end &  ((u64)BLK_SIZE - 1);
	aligned_start = src & ~((u64)BLK_SIZE - 1);
	aligned_end   = end & ~((u64)BLK_SIZE - 1);

	if (part_start) {
	    blknr = (ulong)(aligned_start >> BLK_BITS);
		part_len = BLK_SIZE - part_start;
        if (part_len > (u64)size) {
            part_len = size;
        }
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
        }
		memcpy(dst, buf + part_start, part_len);
		dst += part_len;
        src += part_len;
    }

	aligned_start = src & ~((u64)BLK_SIZE - 1);
	blknr  = (ulong)(aligned_start >> BLK_BITS);
	blkcnt = (ulong)((aligned_end - aligned_start) >> BLK_BITS);

	if(blkcnt!=0)
	{
	    if ((blkdev->block_read(dev_id, blknr, blkcnt, (unsigned long *)(dst), part_id)) != blkcnt) {
		    return -EIO;
        }
    }

    src += (u64)(blkcnt << BLK_BITS);
    dst += (blkcnt << BLK_BITS);

	if (part_end && src < end) {
	    blknr = (ulong)(aligned_end >> BLK_BITS);
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
        }
		memcpy(dst, buf, part_end);
	}
	return size;
}

static int mt_part_generic_write(part_dev_t *dev, uchar *src, u64 dst, int size, unsigned int part_id)
{
    int dev_id = dev->id;
    uchar *buf = &mt_part_buf[0];
    block_dev_desc_t *blkdev = dev->blkdev;
    u64 end, part_start, part_end, part_len, aligned_start, aligned_end;
    ulong blknr, blkcnt;

	if (!blkdev) {
        part_err("No block device registered\n");
        return -ENODEV;
	}

	if (size == 0) {
		return 0;
    }

	end = dst + size;

	part_start    = dst &  ((u64)BLK_SIZE - 1);
	part_end      = end &  ((u64)BLK_SIZE - 1);
	aligned_start = dst & ~((u64)BLK_SIZE - 1);
	aligned_end   = end & ~((u64)BLK_SIZE - 1);

	if (part_start) {
	    blknr = (ulong)(aligned_start >> BLK_BITS);
		part_len = BLK_SIZE - part_start;
		if (part_len > (u64)size) {
            part_len = size;
        }
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
        }
		memcpy(buf + part_start, src, part_len);
    	if ((blkdev->block_write(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
        	return -EIO;
        }
		dst += part_len;
        src += part_len;
    }

	aligned_start = dst & ~((u64)BLK_SIZE - 1);
	blknr  = (ulong)(aligned_start >> BLK_BITS);
	blkcnt = (ulong)((aligned_end - aligned_start) >> BLK_BITS);

	if(blkcnt!=0)
	{
	    if ((blkdev->block_write(dev_id, blknr, blkcnt, (unsigned long *)(src), part_id)) != blkcnt)
	    	return -EIO;
	}

    src += (blkcnt << BLK_BITS);
    dst += (u64)(blkcnt << BLK_BITS);

	if (part_end && dst < end) {
	    blknr = (ulong)(aligned_end >> BLK_BITS);
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
		}
		memcpy(buf, src, part_end);
    	if ((blkdev->block_write(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
            return -EIO;
    	}
	}
	return size;
}

extern int read_gpt(part_t *part);

int mt_part_register_device(part_dev_t *dev)
{
    part_t *part_ptr;

	part_info("[mt_part_register_device]\n");
    if (!mt_part_dev) {
        if (!dev->read) {
		    dev->read = mt_part_generic_read;
        }
		if (!dev->write) {
		    dev->write = mt_part_generic_write;
        }
		mt_part_dev = dev;

	    mt_part_buf = (uchar*)malloc(BLK_SIZE * 2);
	    //part_info("[mt_part_register_device]malloc %d : %x\n",(BLK_SIZE * 2), mt_part_buf);

        part_ptr = calloc(128, sizeof(part_t));
        if (!part_ptr) {
            return 0;
        }

        partition = part_ptr;

        read_gpt(part_ptr);
    }
    return 0;
}

#elif defined(MTK_NAND_SUPPORT)

void mt_part_dump(void)
{
    part_t *part = &partition_layout[0];

    part_info("\nPart Info from compiler.(1blk=%dB):\n", BLK_SIZE);
    part_info("\nPart Info.(1blk=%dB):\n", BLK_SIZE);
    while (part->name) {
        part_info("[0x%.8x-0x%.8x] (%.8ld blocks): \"%s\"\n",
               part->start_sect * BLK_SIZE,
              (part->start_sect + part->nr_sects) * BLK_SIZE - 1,
			   part->nr_sects, part->name);
        part++;
    }
    part_info("\n");
}

int mt_part_generic_read(part_dev_t *dev, ulong src, uchar *dst, int size)
{
    int dev_id = dev->id;
    uchar *buf = &mt_part_buf[0];
    block_dev_desc_t *blkdev = dev->blkdev;
	ulong end, part_start, part_end, part_len, aligned_start, aligned_end;
    ulong blknr, blkcnt;

	if (!blkdev) {
        part_err("No block device registered\n");
        return -ENODEV;
	}

	if (size == 0) {
		return 0;
    }

	end = src + size;

	part_start    = src &  (BLK_SIZE - 1);
	part_end      = end &  (BLK_SIZE - 1);
	aligned_start = src & ~(BLK_SIZE - 1);
	aligned_end   = end & ~(BLK_SIZE - 1);

	if (part_start) {
	    blknr = aligned_start >> BLK_BITS;
		part_len = BLK_SIZE - part_start;
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf), part_id) != 1) {
			return -EIO;
        }
		memcpy(dst, buf + part_start, part_len);
		dst += part_len;
        src += part_len;
    }

	aligned_start = src & ~(BLK_SIZE - 1);
	blknr  = aligned_start >> BLK_BITS;
	blkcnt = (aligned_end - aligned_start) >> BLK_BITS;

	if ((blkdev->block_read(dev_id, blknr, blkcnt, (unsigned long *)(dst), part_id)) != blkcnt) {
		return -EIO;
    }

    src += (blkcnt << BLK_BITS);
    dst += (blkcnt << BLK_BITS);

	if (part_end && src < end) {
	    blknr = aligned_end >> BLK_BITS;
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf)) != 1) {
			return -EIO;
        }
		memcpy(dst, buf, part_end);
	}
	return size;
}

static int mt_part_generic_write(part_dev_t *dev, uchar *src, ulong dst, int size)
{
    int dev_id = dev->id;
    uchar *buf = &mt_part_buf[0];
    block_dev_desc_t *blkdev = dev->blkdev;
	ulong end, part_start, part_end, part_len, aligned_start, aligned_end;
    ulong blknr, blkcnt;

	if (!blkdev) {
        part_err("No block device registered\n");
        return -ENODEV;
	}

	if (size == 0) {
		return 0;
    }

	end = dst + size;

	part_start    = dst &  (BLK_SIZE - 1);
	part_end      = end &  (BLK_SIZE - 1);
	aligned_start = dst & ~(BLK_SIZE - 1);
	aligned_end   = end & ~(BLK_SIZE - 1);

	if (part_start) {
	    blknr = aligned_start >> BLK_BITS;
		part_len = BLK_SIZE - part_start;
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf), part_id) != 1)
			return -EIO;
		memcpy(buf + part_start, src, part_len);
    	if ((blkdev->block_write(dev_id, blknr, 1, (unsigned long*)buf), part_id) != 1)
        	return -EIO;
		dst += part_len;
        src += part_len;
    }

	aligned_start = dst & ~(BLK_SIZE - 1);
	blknr  = aligned_start >> BLK_BITS;
	blkcnt = (aligned_end - aligned_start) >> BLK_BITS;

	if ((blkdev->block_write(dev_id, blknr, blkcnt, (unsigned long *)(src), part_id)) != blkcnt) {
		return -EIO;
    }

    src += (blkcnt << BLK_BITS);
    dst += (blkcnt << BLK_BITS);

	if (part_end && dst < end) {
	    blknr = aligned_end >> BLK_BITS;
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf), part_id) != 1) {
			return -EIO;
		}
		memcpy(buf, src, part_end);
    	if ((blkdev->block_write(dev_id, blknr, 1, (unsigned long*)buf), part_id) != 1) {
            return -EIO;
    	}
	}
	return size;
}

int mt_part_register_device(part_dev_t *dev)
{
    part_info("[mt_part_register_device]\n");
    if (!mt_part_dev) {
        if (!dev->read) {
            dev->read = mt_part_generic_read;
        }
        if (!dev->write) {
            dev->write = mt_part_generic_write;
        }
        mt_part_dev = dev;
		mt_part_buf = (uchar*)malloc(BLK_SIZE * 2);
    	part_info("[mt_part_register_device]malloc %d : %x\n", (BLK_SIZE * 2), mt_part_buf);

#ifdef PMT
	    part_init_pmt(BLK_NUM(1 * GB), dev);
#else
        mt_part_init(BLK_NUM(1 * GB));
#endif
    }

    return 0;
}

#else

void mt_part_dump(void)
{
}


#endif

part_dev_t *mt_part_get_device(void)
{
    if (mt_part_dev && !mt_part_dev->init && mt_part_dev->init_dev) {
        mt_part_dev->init_dev(mt_part_dev->id);
        mt_part_dev->init = 1;
    }
    return mt_part_dev;
}

/**/
/*fastboot*/
/**/
unsigned int write_partition(unsigned size, unsigned char *partition)
{
    return 0;
}

int partition_get_index(const char * name)
{
	int index;

	for (index = 0; index < PART_MAX_COUNT; index++) {
		if(index >= (int)(sizeof(g_part_name_map)/sizeof(g_part_name_map[0])) )
		    return -1;
		if (!strcmp(name, g_part_name_map[index].fb_name)) {
            part_t *p = mt_part_get_partition(g_part_name_map[index].r_name);
            if (p == NULL) {
                return -1;
            }
			part_info("[%s]find %s %s index %d\n", __FUNCTION__, name, g_part_name_map[index].r_name, g_part_name_map[index].partition_idx);
			return g_part_name_map[index].partition_idx;
		}
	}

	return -1;
}

unsigned int partition_get_region(int index)
{
    part_t *p;
    if(index >= (int)(sizeof(g_part_name_map)/sizeof(g_part_name_map[0])) )
		return -1;

    p = mt_part_get_partition(g_part_name_map[index].r_name);
    if (p == NULL) {
        return -1;
    }
    return p->part_id;
}

u64 partition_get_offset(int index)
{
	part_t *p;
    if(index >= (int)(sizeof(g_part_name_map)/sizeof(g_part_name_map[0])) )
		return -1;

    p = mt_part_get_partition(g_part_name_map[index].r_name);
	if (p == NULL) {
		return -1;
    }
	return (u64)p->start_sect * BLK_SIZE;
}

u64 partition_get_size(int index)
{
	part_t *p;
    if(index >= (int)(sizeof(g_part_name_map)/sizeof(g_part_name_map[0])) )
		return -1;

	p = mt_part_get_partition(g_part_name_map[index].r_name);
	if (p == NULL) {
		return -1;
    }
	return (u64)p->nr_sects * BLK_SIZE;
}

int partition_get_type(int index, char **p_type)
{
	*p_type = g_part_name_map[index].partition_type;
	return 0;
}

int partition_get_name(int index, char **p_name)
{
	*p_name = g_part_name_map[index].fb_name;
	return 0;
}

int is_support_erase(int index)
{
	return g_part_name_map[index].is_support_erase;
}

int is_support_flash(int index)
{
	return g_part_name_map[index].is_support_dl;
}

#ifdef MTK_EMMC_SUPPORT
u64 emmc_write(u32 part_id, u64 offset, void *data, u64 size)
{
	part_dev_t *dev = mt_part_get_device();
	return (u64)dev->write(dev,data,offset,(int)size, part_id);
}

u64 emmc_read(u32 part_id, u64 offset, void *data, u64 size)
{
	part_dev_t *dev = mt_part_get_device();
	return (u64)dev->read(dev,offset,data,(int)size, part_id);
}

extern int mmc_do_erase(int dev_num,u64 start_addr,u64 len,u32 part_id);
int emmc_erase(u32 part_id, u64 offset, u64 size)
{
	return mmc_do_erase(0,offset,size, part_id);
}
#endif
