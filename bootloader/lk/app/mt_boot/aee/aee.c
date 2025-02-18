#include <malloc.h>
#ifdef MTK_GPT_SCHEME_SUPPORT
#include <platform/partition.h>
#else
#include <mt_partition.h>
#endif
#include <printf.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <video.h>
#include <dev/mrdump.h>
#include <platform/mtk_key.h>
#include <platform/mtk_wdt.h>
#include <platform/mt_gpt.h>
#include <target/cust_key.h>
#include <platform/boot_mode.h>
#include <platform/ram_console.h>
#include <arch/ops.h>
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
#endif
#include "aee.h"
#include "kdump.h"

extern BOOT_ARGUMENT *g_boot_arg;
extern bool cmdline_append(const char* append_string);

#define MRDUMP_DELAY_TIME 10

static struct mrdump_control_block *mrdump_cblock = NULL;

static struct mrdump_cblock_result cblock_result;
static unsigned int log_size;

static void voprintf(char type, const char *msg, va_list ap)
{
    char msgbuf[128], *p;

    p = msgbuf;
    if (msg[0] == '\r') {
        *p++ = msg[0];
        msg++;
    }

    *p++ = type;
    *p++ = ':';
    vsnprintf(p, sizeof(msgbuf) - (p - msgbuf), msg, ap);
    switch (type) {
    case 'I':
    case 'W':
    case 'E':
        video_printf("%s", msgbuf);
        break;
    }

    dprintf(CRITICAL,"%s", msgbuf);
    
    /* Write log buffer */
    p = msgbuf;
    while ((*p != 0) && (log_size < sizeof(cblock_result.log_buf))) {
	cblock_result.log_buf[log_size] = *p++;
	log_size++;
    }
}

void voprintf_verbose(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    voprintf('V', msg, ap);
    va_end(ap);
}

void voprintf_debug(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    voprintf('D', msg, ap);
    va_end(ap);
}

void voprintf_info(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    voprintf('I', msg, ap);
    va_end(ap);
}

void voprintf_warning(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    voprintf('W', msg, ap);
    va_end(ap);
}

void voprintf_error(const char *msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    voprintf('E', msg, ap);
    va_end(ap);
}

void vo_show_progress(int sizeM)
{
    video_set_cursor((video_get_rows() / 4) * 3, (video_get_colums() - 22)/ 2);
    video_printf("=====================\n");
    video_set_cursor((video_get_rows() / 4) * 3 + 1, (video_get_colums() - 22)/ 2);
    video_printf(">>> Written %4dM <<<\n", sizeM);
    video_set_cursor((video_get_rows() / 4) * 3 + 2, (video_get_colums() - 22)/ 2);
    video_printf("=====================\n");
    video_set_cursor(video_get_rows() - 1, 0);

    dprintf(CRITICAL,"... Written %dM\n", sizeM);
}

static void mrdump_status(const char *status, const char *fmt, va_list ap)
{
    if (mrdump_cblock != NULL) {
        char *dest = strcpy(cblock_result.status, status);
        dest += strlen(dest);
        *dest++ = '\n';
    
        vsnprintf(dest, sizeof(cblock_result.status) - (dest - cblock_result.status), fmt, ap);
    }
}

void mrdump_status_ok(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    mrdump_status("OK", fmt, ap);
    va_end(ap);
}

void mrdump_status_none(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    mrdump_status("NONE", fmt, ap);
    va_end(ap);
}

void mrdump_status_error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    mrdump_status("FAILED", fmt, ap);
    va_end(ap);
}

uint32_t g_aee_mode = AEE_MODE_MTK_ENG;

const const char *mrdump_mode2string(uint8_t mode)
{
  switch (mode) {
  case AEE_REBOOT_MODE_NORMAL:
    return "NORMAL-BOOT";

  case AEE_REBOOT_MODE_KERNEL_OOPS:
    return "KERNEL-OOPS";

  case AEE_REBOOT_MODE_KERNEL_PANIC:
    return "KERNEL-PANIC";

  case AEE_REBOOT_MODE_NESTED_EXCEPTION:
    return "NESTED-CPU-EXCEPTION";

  case AEE_REBOOT_MODE_WDT:
    return "HWT";

  case AEE_REBOOT_MODE_EXCEPTION_KDUMP:
    return "MANUALDUMP";

  default:
    return "UNKNOWN-BOOT";
  }
}

#define MRDUMP_EXPDB_OFFSET 3145728

static void mrdump_write_result(void)
{
	dprintf(CRITICAL, "%s: Enter\n", __func__);

        int index = partition_get_index("expdb");
	part_dev_t *dev = mt_part_get_device();
        if(index == -1 || dev == NULL) {
	    dprintf(CRITICAL, "%s: no %s partition[%d]\n", __func__, "expdb", index);
	    return;
        }

#ifdef MTK_NEW_COMBO_EMMC_SUPPORT
        int part_id = partition_get_region(index);
#endif
	u64 part_size = partition_get_size(index);
	if (part_size < MRDUMP_EXPDB_OFFSET) {
	    dprintf(CRITICAL, "%s: partition size(%llx) is less then reserved (%x)\n", __func__, part_size, MRDUMP_EXPDB_OFFSET);
	    return;
	}
        u64 part_offset = partition_get_offset(index) + part_size - MRDUMP_EXPDB_OFFSET;

	dprintf(CRITICAL, "%s: offset %lld size %lld\n", __func__, part_offset, part_size);

#ifdef MTK_NEW_COMBO_EMMC_SUPPORT
        dev->write(dev, (uchar *)&cblock_result, part_offset, sizeof(cblock_result), part_id);
#else
        dev->write(dev, (uchar *)&cblock_result, part_offset, sizeof(cblock_result));
#endif
}

static void kdump_ui(struct mrdump_control_block *mrdump_cblock)
{
    video_clean_screen();
    video_set_cursor(0, 0);

    mrdump_status_error("Unknown error\n");
    voprintf_info("Kdump triggerd by '%s'\n", mrdump_mode2string(mrdump_cblock->crash_record.reboot_mode));

    struct aee_timer elapse_time;
    aee_timer_init(&elapse_time);

    uint32_t total_dump_size = memory_size();
    
    aee_timer_start(&elapse_time);
    switch (mrdump_cblock->machdesc.output_device) {
    case MRDUMP_DEV_NULL:
        kdump_null_output(mrdump_cblock, total_dump_size);
        break;
#if 0
    case MRDUMP_DEV_SDCARD:
        kdump_sdcard_output(mrdump_cblock, total_dump_size);
        break;
#endif
    case MRDUMP_DEV_EMMC:
        kdump_emmc_output(mrdump_cblock, total_dump_size);
        break;

    default:
        voprintf_error("Unknown device id %d\n", mrdump_cblock->machdesc.output_device);
    }
    aee_mrdump_flush_cblock(mrdump_cblock);
    aee_timer_stop(&elapse_time);
    
    voprintf_info("Reset count down %d ...\n", MRDUMP_DELAY_TIME);
    mtk_wdt_restart();

    int timeout = MRDUMP_DELAY_TIME;
    while(timeout-- >= 0) {
        mdelay(1000);
        mtk_wdt_restart();
	voprintf_info("\rsec %d", timeout);
    }

    video_clean_screen();
    video_set_cursor(0, 0);
}
int check_mrdump_status(void)
{
	return 0;
}
int mrdump_detection(void)
{
	char mrdump_rsv_mem[128] = {0};
    cmdline_append("mrdump.lk=" MRDUMP_GO_DUMP);
	/*
	 * mrdump_rsv_mem format , a pair start address,size
	 * preloader will use g_dram_buf to store g_boot_arg, the size is almost 0xA0000
	 * we reserve 1MB for all project to prevent pass size info from preloader
	 * */

	sprintf(mrdump_rsv_mem, "mrdump_rsvmem=0x%x,0x%x,0x%x,0x%x",
			MEMBASE, AEE_MRDUMP_LK_RSV_SIZE, (unsigned int)BOOT_ARGUMENT_LOCATION&0xfff00000
				, (g_boot_arg->dram_buf_size)?g_boot_arg->dram_buf_size:0x100000);
	cmdline_append(mrdump_rsv_mem);
    if (!ram_console_is_abnormal_boot()) {
        dprintf(CRITICAL, "MT-RAMDUMP: No exception detected, skipped\n");
	return 0;
    }

    mrdump_cblock = aee_mrdump_get_params();
    if (mrdump_cblock == NULL) {
	dprintf(CRITICAL, "MT-RADMUMP control block not found\n");
	return 0;
    }

    memset(&cblock_result, 0, sizeof(struct mrdump_cblock_result));
    log_size = 0;
    strcpy(cblock_result.sig, MRDUMP_GO_DUMP);

    if (!g_boot_arg->ddr_reserve_enable) {
	voprintf_debug("DDR reserve mode disabled\n");
	mrdump_status_none("DDR reserve mode disabled\n");
	goto error;
    }

    if (!g_boot_arg->ddr_reserve_success) {
	voprintf_debug("DDR reserve mode failed\n");
	mrdump_status_none("DDR reserve mode failed\n");
	goto error;
    }
    uint8_t reboot_mode = mrdump_cblock->crash_record.reboot_mode;
    if (mrdump_cblock->machdesc.nr_cpus == 0) {
	voprintf_debug("Runtime disabled\n");
	mrdump_status_none("Runtime disabled\n");
	goto error;
    }

    voprintf_debug("sram record with mode %d\n", reboot_mode);
    switch (reboot_mode) {
	/* SoC trigger WDT/HWT or HW REBOOT */
        case AEE_REBOOT_MODE_WDT:
        case AEE_REBOOT_MODE_NORMAL:
	    return 0;

	/* SoC trigger KE type exception */
        case AEE_REBOOT_MODE_KERNEL_OOPS:
        case AEE_REBOOT_MODE_KERNEL_PANIC:
        case AEE_REBOOT_MODE_NESTED_EXCEPTION:
        case AEE_REBOOT_MODE_EXCEPTION_KDUMP:
            return 1;
    }
    voprintf_debug("Unsupport exception type\n");
    mrdump_status_none("Unsupport exception type\n");

 error:
    mrdump_write_result();
    return 0;
}

int mrdump_run2(void)
{
    if (mrdump_cblock != NULL) {
        kdump_ui(mrdump_cblock);
	mrdump_write_result();
	mtk_arch_reset(1);
	return 1;
    }
    return 0;
}

void aee_timer_init(struct aee_timer *t)
{
    memset(t, 0, sizeof(struct aee_timer));
}

void aee_timer_start(struct aee_timer *t)
{
    t->start_ms = get_timer_masked();
}

void aee_timer_stop(struct aee_timer *t)
{
    t->acc_ms += (get_timer_masked() - t->start_ms);
    t->start_ms = 0;
}

void *kdump_core_header_init(const struct mrdump_control_block *kparams, uint64_t kmem_address, uint64_t kmem_size)
{
    voprintf_info("kernel page offset %llu\n", kparams->machdesc.page_offset);
    if (kparams->machdesc.page_offset <= 0xffffffffULL) {
	voprintf_info("32b kernel detected\n");
        return kdump_core32_header_init(kparams, kmem_address, kmem_size);
    }
    else {
	voprintf_info("64b kernel detected\n");
        return kdump_core64_header_init(kparams, kmem_address, kmem_size);
    }
}
