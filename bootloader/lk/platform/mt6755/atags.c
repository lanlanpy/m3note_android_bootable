/* Copyright (c) 2009-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <reg.h>
#include <debug.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <platform.h>
#include <platform/mt_typedefs.h>
#include <platform/boot_mode.h>
#include <platform/mt_reg_base.h>
#include <platform/sec_devinfo.h>
#include <platform/env.h>
#include <platform/sec_export.h>
//#include <dfo_boot_default.h>
extern int g_nr_bank;
extern BOOT_ARGUMENT *g_boot_arg;
extern BI_DRAM bi_dram[MAX_NR_BANK];
extern int get_meta_port_id(void);


struct tag_header {
	u32 size;
	u32 tag;
};

#define tag_size(type)  ((sizeof(struct tag_header) + sizeof(struct type)) >> 2)

#define SIZE_1M             (1024 * 1024)
#define SIZE_2M             (2 * SIZE_1M)
#define SIZE_256M           (256 * SIZE_1M)
#define SIZE_512M           (512 * SIZE_1M)

/* The list must start with an ATAG_CORE node */
#define ATAG_CORE   0x54410001
struct tag_core {
	u32 flags;      /* bit 0 = read-only */
	u32 pagesize;
	u32 rootdev;
};

/* it is allowed to have multiple ATAG_MEM nodes */
#define ATAG_MEM  0x54410002
typedef struct {
	uint32_t size;
	uint32_t start_addr;
} mem_info;

#define ATAG_MEM64  0x54420002
typedef struct {
	uint64_t size;
	uint64_t start_addr;
} mem64_info;

#define ATAG_EXT_MEM64  0x54430002
typedef struct {
	uint64_t size;
	uint64_t start_addr;
} ext_mem64_info;

/* command line: \0 terminated string */
#define ATAG_CMDLINE    0x54410009
struct tag_cmdline {
	char    cmdline[1]; /* this is the minimum size */
};

/* describes where the compressed ramdisk image lives (physical address) */
#define ATAG_INITRD2    0x54420005
struct tag_initrd {
	u32 start;  /* physical start address */
	u32 size;   /* size of compressed ramdisk image in bytes */
};

#define ATAG_VIDEOLFB   0x54410008
struct tag_videolfb {
	u64 fb_base;
	u32 islcmfound;
	u32 fps;
	u32 vram;
	char lcmname[1]; /* this is the minimum size */
};

/* boot information */
#define ATAG_BOOT   0x41000802
struct tag_boot {
	u32 bootmode;
};

/*META com port information*/
#define ATAG_META_COM 0x41000803
struct tag_meta_com {
	u32 meta_com_type; /* identify meta via uart or usb */
	u32 meta_com_id;  /* multiple meta need to know com port id */
	u32 meta_uart_port; /* identify meta uart port */
};

/*device information*/
#define ATAG_DEVINFO_DATA         0x41000804
#define ATAG_DEVINFO_DATA_SIZE    50
struct tag_devinfo_data {
	u32 devinfo_data[ATAG_DEVINFO_DATA_SIZE];
	u32 devinfo_data_size;
};

#define ATAG_MDINFO_DATA 0x41000806
struct tag_mdinfo_data {
	u8 md_type[4];
};

#define ATAG_VCORE_DVFS_INFO 0x54410007
struct tag_vcore_dvfs_info {
	u32 pllgrpreg_size;
	u32 freqreg_size;
	u32* low_freq_pll_val;
	u32* low_freq_cha_val;
	u32* low_freq_chb_val;
	u32* high_freq_pll_val;
	u32* high_freq_cha_val;
	u32* high_freq_chb_val;
};

#define ATAG_PTP_INFO 0x54410008
struct tag_ptp_info {
	u32 first_volt;
	u32 second_volt;
	u32 third_volt;
	u32 have_550;
};

/* The list ends with an ATAG_NONE node. */
#define ATAG_NONE   0x00000000

unsigned *target_atag_nand_data(unsigned *ptr)
{
	return ptr;
}


unsigned *target_atag_partition_data(unsigned *ptr)
{
	return ptr;
}

unsigned *target_atag_boot(unsigned *ptr)
{
	*ptr++ = tag_size(tag_boot);
	*ptr++ = ATAG_BOOT;
	*ptr++ = g_boot_mode;

	return ptr;
}


extern int imix_r;
unsigned *target_atag_imix_r(unsigned *ptr)
{
//    *ptr++ = tag_size(tag_imix_r);
//    *ptr++ = ATAG_IMIX;
	*ptr++ = imix_r;

	dprintf(CRITICAL, "target_atag_imix_r:%d\n",imix_r);
	return ptr;
}

unsigned *target_atag_devinfo_data(unsigned *ptr)
{
	int i = 0;
	*ptr++ = tag_size(tag_devinfo_data);
	*ptr++ = ATAG_DEVINFO_DATA;
	for (i=0; i<ATAG_DEVINFO_DATA_SIZE; i++) {
		*ptr++ = get_devinfo_with_index(i);
	}
	*ptr++ = ATAG_DEVINFO_DATA_SIZE;

	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(1));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(2));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(3));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(4));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(20));
	dprintf(INFO, "SSSS:0x%x\n", get_devinfo_with_index(21));

	return ptr;
}



unsigned *target_atag_masp_data(unsigned *ptr)
{

	/*tag size*/
	*ptr++ = tag_size(tag_masp_data);
	/*tag name*/
	*ptr++ = ATAG_MASP_DATA;

	ptr = fill_atag_masp_data(ptr);

	return ptr;
}

unsigned *target_atag_mem(unsigned *ptr)
{
	int i;

	for (i = 0; i < g_nr_bank; i++) {
#ifndef MTK_LM_MODE
		*ptr++ = 4; //tag size
		*ptr++ = ATAG_MEM; //tag name
		*ptr++ = bi_dram[i].size;
		*ptr++ = bi_dram[i].start;
#else
		*ptr++ = 6; //tag size
		*ptr++ = ATAG_MEM64; //tag name
		//*((unsigned long long*)ptr)++ = bi_dram[i].size;
		//*((unsigned long long*)ptr)++ = bi_dram[i].start;
		unsigned long long *ptr64 = (unsigned long long *)ptr;
		*ptr64++ = bi_dram[i].size;
		*ptr64++ = bi_dram[i].start;
		ptr = (unsigned int *)ptr64;
#endif
	}
	return ptr;
}

unsigned *target_atag_meta(unsigned *ptr)
{
	*ptr++ = tag_size(tag_meta_com);
	*ptr++ = ATAG_META_COM;
	*ptr++ = g_boot_arg->meta_com_type;
	*ptr++ = g_boot_arg->meta_com_id;
	*ptr++ = get_meta_port_id();
	dprintf(CRITICAL, "meta com type = %d\n", g_boot_arg->meta_com_type);
	dprintf(CRITICAL, "meta com id = %d\n", g_boot_arg->meta_com_id);
	dprintf(CRITICAL, "meta uart port = %d\n", get_meta_port_id());
	return ptr;
}

/* todo: give lk strtoul and nuke this */
static unsigned hex2unsigned(const char *x)
{
	unsigned n = 0;

	while (*x) {
		switch (*x) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				n = (n << 4) | (*x - '0');
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				n = (n << 4) | (*x - 'a' + 10);
				break;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				n = (n << 4) | (*x - 'A' + 10);
				break;
			default:
				return n;
		}
		x++;
	}

	return n;
}
#if 0
#define ATAG_DFO_DATA 0x41000805
unsigned *target_atag_dfo(unsigned *ptr)
{
	int i, j;
	dfo_boot_info *dfo_p;
	char tmp[11];
	unsigned  char *buffer;

	*ptr++ = ((sizeof(struct tag_header) + DFO_BOOT_COUNT * sizeof(dfo_boot_info)) >> 2);
	*ptr++ = ATAG_DFO_DATA;

	memcpy((void *)ptr, (void *)dfo_boot_default, DFO_BOOT_COUNT * sizeof(dfo_boot_info));
	dfo_p = (dfo_boot_info *)ptr;

	ptr += DFO_BOOT_COUNT * sizeof(dfo_boot_info) >> 2;

	buffer = (unsigned char *)get_env("DFO");

	if (buffer != NULL) {

		for (i = 0; i < DFO_BOOT_COUNT; i++) {
			j = 0;
			do {
				dfo_p[i].name[j] = *buffer;
				j++;
			} while (*buffer++ != ',' && j < 31);

			dfo_p[i].name[j-1] = '\0';
			j = 0;

			do {
				tmp[j] = *buffer;
				j++;
			} while (*buffer++ != ',' && j < 10);

			tmp[j] = '\0';

			if ((strncmp("0x", tmp, 2) == 0) || (strncmp("0X", tmp, 2) == 0))
				dfo_p[i].value = hex2unsigned(&tmp[2]);
			else
				dfo_p[i].value = atoi(tmp);
		}

		for (i = 0; i < DFO_BOOT_COUNT; i++)
			dprintf(INFO, "[DFO-%d] NAME:%s, Value:%lu\n",i , dfo_p[i].name, dfo_p[i].value);

	} else
		dprintf(INFO, "No DFO. Use default values.\n");

	return ptr;
}
#endif

unsigned *target_atag_commmandline(unsigned *ptr, char *commandline)
{
	char *p;

	if (!commandline)
		return NULL;

	for (p = commandline; *p == ' '; p++);

	if (*p == '\0')
		return NULL;

	*ptr++ = (sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;; //size
	*ptr++ = ATAG_CMDLINE;
	strcpy((char *)ptr, p);  //copy to atags memory region
	ptr += (strlen (p) + 1 + 4) >> 2;
	return ptr;
}

unsigned *target_atag_initrd(unsigned *ptr, ulong initrd_start, ulong initrd_size)
{
	*ptr++ = tag_size(tag_initrd);
	*ptr++ = ATAG_INITRD2;
//TMP for bring up testing
//  *ptr++ = CFG_RAMDISK_LOAD_ADDR;
	// *ptr++ = 0x1072F9;

	*ptr++ = initrd_start;
	*ptr++ = initrd_size;
	return ptr;
}

#include <platform/mt_disp_drv.h>
unsigned *target_atag_videolfb(unsigned *ptr)
{
	extern unsigned long long fb_addr_pa_k;
	const char   *lcmname = mt_disp_get_lcm_id();
	unsigned int *p       = NULL;
	unsigned long long *phy_p   = (unsigned long long *)ptr;
	*phy_p = fb_addr_pa_k;
	p = (unsigned int*)(phy_p + 1);
	*p++ = DISP_IsLcmFound();
	*p++ = mt_disp_get_lcd_time();
	*p++ = DISP_GetVRamSize();
	strcpy((char *)p,lcmname);
	p += (strlen(lcmname) + 1 + 4) >> 2;

	dprintf(CRITICAL, "videolfb - fb_base    = 0x%llx\n",fb_addr_pa_k);
	dprintf(CRITICAL, "videolfb - islcmfound = %d\n",DISP_IsLcmFound());
	dprintf(CRITICAL, "videolfb - fps        = %d\n",mt_disp_get_lcd_time());
	dprintf(CRITICAL, "videolfb - vram       = %d\n",DISP_GetVRamSize());
	dprintf(CRITICAL, "videolfb - lcmname    = %s\n",lcmname);

	return (unsigned *)p;

}


unsigned *target_atag_mdinfo(unsigned *ptr)
{
	unsigned char *p;
	*ptr++=tag_size(tag_mdinfo_data);
	*ptr++=ATAG_MDINFO_DATA;
	p=(unsigned char *)ptr;
	*p++=g_boot_arg->md_type[0];
	*p++=g_boot_arg->md_type[1];
	*p++=g_boot_arg->md_type[2];
	*p++=g_boot_arg->md_type[3];
	return (unsigned *)p;
}

#if 0
unsigned *target_atag_isram(unsigned *ptr)
{
	unsigned char *p;
	p=(unsigned char *)ptr;
	memcpy(p, (unsigned char*)&g_boot_arg->non_secure_sram_addr, sizeof(u32));
	p=p+sizeof(u32);
	memcpy(p, (unsigned char*)&g_boot_arg->non_secure_sram_size, sizeof(u32));
	p=p+sizeof(u32);
	dprintf(CRITICAL,"[LK] non_secure_sram (0x%x, 0x%x)\n", g_boot_arg->non_secure_sram_addr, g_boot_arg->non_secure_sram_size);
	return (unsigned *)p;
}
#endif

unsigned *target_atag_ptp(unsigned *ptr)
{
	ptp_info_t* ptp_info = &g_boot_arg->ptp_volt_info;
	u32 first_volt = ptp_info->first_volt;
	u32 second_volt = ptp_info->second_volt;
	u32 third_volt = ptp_info->third_volt;
	u32 have_550 = ptp_info->have_550;

	*ptr++=tag_size(tag_ptp_info);
	*ptr++=ATAG_PTP_INFO;

	*ptr++=first_volt;
	*ptr++=second_volt;
	*ptr++=third_volt;
	*ptr++=have_550;

	return ptr;
}

void *target_get_scratch_address(void)
{
	return ((void *)SCRATCH_ADDR);
}


#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>

#define NUM_CLUSTERS  2
#define NUM_CORES_CLUSTER0  4
#define NUM_CORES_CLUSTER1  4
#define MAX_CLK_FREQ_PLL0   1200000000
#define MAX_CLK_FREQ_PLL0_M 1000000000
#define MAX_CLK_FREQ_PLL2   2000000000
#define MAX_CLK_FREQ_PLL2_ENG   1950000000


/*
 * 0000: Qual core
 * 1000: Triple core
 * 1100: Dual core
 * 1110: Single core
 * 1111: All disable
 */
#define DEVINFO_CPU_QUAL_CORE   0x0
#define DEVINFO_CPU_TRIPLE_CORE 0x8
#define DEVINFO_CPU_DUAL_CORE   0xC
#define DEVINFO_CPU_SINGLE_CORE 0xE
#define DEVINFO_CPU_ZERO_CORE   0xF

#define DEVINFO_CLUSTER0_QUAL_CORE	0x0
#define DEVINFO_CLUSTER0_TRIPLE_CORE	0x4
#define DEVINFO_CLUSTER0_DUAL_CORE	0x6
#define DEVINFO_CLUSTER0_SINGLE_CORE	0x7

#define DEVINFO_CLUSTER1_QUAL_CORE      0x0
#define DEVINFO_CLUSTER1_TRIPLE_CORE    0x8
#define DEVINFO_CLUSTER1_DUAL_CORE      0xC
#define DEVINFO_CLUSTER1_SINGLE_CORE    0xE
#define DEVINFO_CLUSTER1_ZERO_CORE	0xF




struct cpu_dev_info {
	unsigned int speed    : 4;
	unsigned int crypto   : 1;
	unsigned int cluster0 : 3;
	unsigned int cluster1 : 4;
	unsigned int dfd      : 1;
	unsigned int reserve  : 19;
};

struct cpu_dev_info_2 {
	unsigned int reserve  : 23;
	unsigned int speed    :  4;
};





unsigned int dev_info_max_clk_freq_pll0(void)
{
	unsigned int segment = (get_devinfo_with_index(21)) & 0xFF;
	unsigned int devinfo = get_devinfo_with_index(3);
	struct cpu_dev_info *info = (struct cpu_dev_info *)&devinfo;
	unsigned int max_clk_freq;

	/*
	  0000:Free
	  0001:1.4 GHz
	  0010:1.3 GHz
	  0011:1.2 GHz
	  0100:1.1 GHz
	  0101:1.0 GHz
	  0110:0.9 GHz
	  0111:0.8 GHz
	  1000:0.8 GHz
	  1001:0.8 GHz
	  1010:0.8 GHz
	  1011:0.8 GHz
	  1100:0.8 GHz
	  1101:0.8 GHz
	  1110:0.8 GHz
	  1111:0.8 GHz
	*/

	if ((unsigned int)info->speed == 0) {
		if ((segment == 1) || (segment == 3))
			max_clk_freq = MAX_CLK_FREQ_PLL0_M;
		else
			max_clk_freq = MAX_CLK_FREQ_PLL0;
	}

	else if ((unsigned int)info->speed >= 0x7) {
		max_clk_freq = 800 * 1000 * 1000;
	}

	else {
		unsigned int decrease = (info->speed - 1);
		max_clk_freq = (1400 - (decrease * 100)) * 1000 * 1000;
	}

	return max_clk_freq;
}

unsigned int dev_info_max_clk_freq_pll2(void)
{
	unsigned int segment = (get_devinfo_with_index(21)) & 0xFF;
	unsigned int devinfo_2 = get_devinfo_with_index(4); //>> 23;
	struct cpu_dev_info_2 *info = (struct cpu_dev_info_2 *)&devinfo_2;
	unsigned int max_clk_freq;


	/*
	  0000:Free
	  0001:2.2 GHz
	  0010:2.1 GHz
	  0011:2.0 GHz
	  0100:1.9 GHz
	  0101:1.8 GHz
	  0110:1.7 GHz
	  0111:1.5 GHz
	  1000:1.5 GHz
	  1001:1.5 GHz
	  1010:1.5 GHz
	  1011:1.5 GHz
	  1100:1.5 GHz
	  1101:1.5 GHz
	  1110:1.5 GHz
	  1111:1.5 GHz
	*/

	if ((unsigned int)info->speed == 0) {
		if (segment == 0)
			max_clk_freq = MAX_CLK_FREQ_PLL2_ENG;
		else
			max_clk_freq = MAX_CLK_FREQ_PLL2;
	}

	else if ((unsigned int)info->speed >= 0x7) {
		max_clk_freq = 1500 * 1000 * 1000;
	}

	else {
		unsigned int decrease = (info->speed - 1);
		max_clk_freq = (2200 - (decrease * 100)) * 1000 * 1000;
	}

	return max_clk_freq;
}


int dev_info_nr_cpu(void)
{
	unsigned int devinfo = get_devinfo_with_index(3);
	struct cpu_dev_info *info = (struct cpu_dev_info *)&devinfo;
	int cluster[NUM_CLUSTERS];
	int cluster_idx;
	int nr_cpu = 0;

	memset(cluster, 0, sizeof(cluster));

	cluster[0] = info->cluster0;
	cluster[1] = info->cluster1;

	switch (cluster[0]) {
		case DEVINFO_CLUSTER0_QUAL_CORE:
			nr_cpu += 4;
			break;
		case DEVINFO_CLUSTER0_TRIPLE_CORE:
			nr_cpu += 3;
			break;
		case DEVINFO_CLUSTER0_DUAL_CORE:
			nr_cpu += 2;
			break;
		case DEVINFO_CLUSTER0_SINGLE_CORE:
			nr_cpu += 1;
			break;
		/* core 0 doesn't have zore core case*/
		default:
			nr_cpu += 4;
			break;
	}

       switch (cluster[1]) {
                case DEVINFO_CLUSTER1_QUAL_CORE:
                        nr_cpu += 4;
                        break;
                case DEVINFO_CLUSTER1_TRIPLE_CORE:
                        nr_cpu += 3;
                        break;
                case DEVINFO_CLUSTER1_DUAL_CORE:
                        nr_cpu += 2;
                        break;
                case DEVINFO_CLUSTER1_SINGLE_CORE:
                        nr_cpu += 1;
                        break;
                case DEVINFO_CLUSTER1_ZERO_CORE:
                        nr_cpu += 0;
                        break;
        }

	return nr_cpu;
}

#define PLATFORM_MAX_MODEL_NUM  8
#define HW_CODE_MT6755  0x326

int target_fdt_model(void *fdt)
{
	unsigned int segment = (get_devinfo_with_index(21)) & 0xFF;
	unsigned int hw_code = mt_get_chip_hw_code();
	unsigned int code_max = PLATFORM_MAX_MODEL_NUM;
	int code;
	int len;
	int nodeoffset;
	const struct fdt_property *prop;
	char *prop_name = "model";
	const char *model_name[] = {
		"MT6755",
		"MT6755M",
		"MT6755",
		"MT6755BM",
		"MT6755B",
		"MT6750T",
		"MT6750",
		"MT6738",
	};

	ptp_info_t* ptp_info = &g_boot_arg->ptp_volt_info;
	u32 first_volt = ptp_info->first_volt;
	u32 second_volt = ptp_info->second_volt;
	u32 third_volt = ptp_info->third_volt;
	u32 have_550 = ptp_info->have_550;
	dprintf(CRITICAL, "[PTP][LK] first_volt = 0x%X\n", first_volt);
	dprintf(CRITICAL, "[PTP][LK] second_volt = 0x%X\n", second_volt);
	dprintf(CRITICAL, "[PTP][LK] third_volt = 0x%X\n", third_volt);
	dprintf(CRITICAL, "[PTP][LK] have_550 = 0x%X\n", have_550);

	code = -1;

	/* for segment == 0, handles chip without efuse */
	switch (hw_code) {
		case HW_CODE_MT6755:
			if ((segment == 0x41) || (segment == 0x40))
				code = 5;
			else if (segment == 0x42)
				code = 6;
			else if (segment == 0x43)
				code = 7;
			else
				code = segment & 0x0F;
			break;
	}
	dprintf(CRITICAL, "[LK] hw_code = 0x%x, segment = 0x%x, code = %d\n", hw_code, segment, code);

	if ((code == -1) || (code >= code_max))
		return -1;

	/* Becuase the model is at the begin of device tree.
	 * use nodeoffset=0
	 */
	nodeoffset = 0;
	prop = fdt_get_property(fdt, nodeoffset, prop_name, &len);

	if (prop) {
		int namestroff;
		const char *str = model_name[code];
		//printf("prop->data=0x%8x\n", (uint32_t *)prop->data);
		fdt_setprop_string(fdt, nodeoffset, prop_name, str);
		prop = fdt_get_property(fdt, nodeoffset, prop_name, &len);
		namestroff = fdt32_to_cpu(prop->nameoff);
		dprintf(CRITICAL, "%s=%s\n", fdt_string(fdt, namestroff), (char *)prop->data);
	}
	return 0;
}

int target_fdt_cpus(void *fdt)
{
	int cpus_offset, cpu_node, last_node = -1;
	int len;
	const struct fdt_property *prop;
	unsigned int *data;
	unsigned int reg, clk_freq;

	unsigned int cluster_idx;
	unsigned int core_num;

	unsigned int activated_cores[NUM_CLUSTERS] = {0};
	unsigned int available_cores[NUM_CLUSTERS] = {NUM_CORES_CLUSTER0, NUM_CORES_CLUSTER1};
	unsigned int max_clk_freq = dev_info_max_clk_freq_pll0();
	unsigned int devinfo = get_devinfo_with_index(3);
	unsigned int devinfo_2 = get_devinfo_with_index(4);
	struct cpu_dev_info *info = (struct cpu_dev_info *)&devinfo;
	struct cpu_dev_info_2 *info_2 = (struct cpu_dev_info *)&devinfo_2;

	dprintf(INFO, "info->cluster0=0x%x\n", info->cluster0);
	dprintf(INFO, "info->cluster1=0x%x\n", info->cluster1);
	dprintf(INFO, "info->speed=0x%x\n", info->speed);
	dprintf(INFO, "info_2->speed=0x%x\n", info_2->speed);

	switch ((unsigned int)info->cluster0) {
		case DEVINFO_CLUSTER0_QUAL_CORE:
			available_cores[0] = 4;
			break;
		case DEVINFO_CLUSTER0_TRIPLE_CORE:
			available_cores[0] = 3;
			break;
		case DEVINFO_CLUSTER0_DUAL_CORE:
			available_cores[0] = 2;
			break;
		case DEVINFO_CLUSTER0_SINGLE_CORE:
			available_cores[0] = 1;
			break;
		/* core 0 doesn't have zore core case*/
		default:
			available_cores[0] = 4;
			break;
	}

	switch ((unsigned int)info->cluster1) {
		case DEVINFO_CLUSTER1_QUAL_CORE:
			available_cores[1] = 4;
			break;
		case DEVINFO_CLUSTER1_TRIPLE_CORE:
			available_cores[1] = 3;
			break;
		case DEVINFO_CLUSTER1_DUAL_CORE:
			available_cores[1] = 2;
			break;
		case DEVINFO_CLUSTER1_SINGLE_CORE:
			available_cores[1] = 1;
			break;
		case DEVINFO_CLUSTER1_ZERO_CORE:
			available_cores[1] = 0;
			break;
		default:
			available_cores[1] = 4;
			break;
	}

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	if (cpus_offset < 0) {
		dprintf(CRITICAL, "couldn't find /cpus\n");
		return cpus_offset;
	}

	for (cpu_node = fdt_first_subnode(fdt, cpus_offset); cpu_node >= 0;
	        cpu_node = ((last_node >= 0) ? fdt_next_subnode(fdt, last_node) : fdt_first_subnode(fdt, cpus_offset))) {
		prop = fdt_get_property(fdt, cpu_node, "device_type", &len);
		if ((!prop) || (len < 4) || (strcmp(prop->data, "cpu"))) {
			last_node = cpu_node;
			continue;
		}

		prop = fdt_get_property(fdt, cpu_node, "reg", &len);

		data = (uint32_t *)prop->data;
		reg = fdt32_to_cpu(*data);

		dprintf(INFO, "reg = 0x%x\n", reg);
		core_num = reg & 0xFF;
		cluster_idx = (reg & 0x100) >> 8;
		dprintf(INFO, "cluster_idx=%d, core_num=%d\n", cluster_idx, core_num);

		activated_cores[cluster_idx]++;

		prop = fdt_get_property(fdt, cpu_node, "clock-frequency", &len);
		data = (uint32_t *)prop->data;
		clk_freq = fdt32_to_cpu(*data);

		if (cluster_idx == 1) {
			max_clk_freq = dev_info_max_clk_freq_pll2();
		}

		dprintf(INFO, "cluster_idx=%d, core_num=%d, clock-frequency = %u => %u\n",
		        cluster_idx, core_num, clk_freq, max_clk_freq);

		if (clk_freq > max_clk_freq) {
			dprintf(INFO, "setprop: clock-frequency = %u => %u\n", clk_freq, max_clk_freq);
			fdt_setprop_cell(fdt, cpu_node, "clock-frequency", max_clk_freq);
		}

		last_node = cpu_node;
		

		if (cluster_idx == NUM_CLUSTERS) {
			dprintf(CRITICAL, "Warning: unknown cpu type in device tree\n");
			last_node = cpu_node;
		}
	}

	for (cluster_idx = 0; cluster_idx < NUM_CLUSTERS; cluster_idx++) {
		if (activated_cores[cluster_idx] > available_cores[cluster_idx])
			dprintf(CRITICAL, "Warning: unexpected reg value in device tree\n");

		dprintf(CRITICAL, "cluster-%d: %d core\n", cluster_idx, available_cores[cluster_idx]);
	}


	return 0;
}

#ifdef NEW_MEMORY_RESERVED_MODEL
int setup_mem_property_use_mblock_info(dt_dram_info *property, size_t p_size)
{
	mblock_info_t *mblock_info = &g_boot_arg->mblock_info;
	dt_dram_info *p;
	unsigned int i;
	if (mblock_info->mblock_num > p_size) {
		dprintf(CRITICAL, "mblock_info->mblock_num =%d is bigger than mem_property=%d\n", mblock_info->mblock_num, p_size);
		return 1;
	}

	for (i = 0; i < mblock_info->mblock_num; ++i) {
		p = (property + i);

		p->start_hi = cpu_to_fdt32(mblock_info->mblock[i].start>>32);
		p->start_lo = cpu_to_fdt32(mblock_info->mblock[i].start);
		p->size_hi = cpu_to_fdt32((mblock_info->mblock[i].size)>>32);
		p->size_lo = cpu_to_fdt32(mblock_info->mblock[i].size);
		dprintf(CRITICAL, "mblock[%d].start: 0x%llx, size: 0x%llx\n",
		        i,
		        mblock_info->mblock[i].start,
		        mblock_info->mblock[i].size);

		dprintf(INFO, " mem_reg_property[%d].start_hi = 0x%08X\n", i, p->start_hi);
		dprintf(INFO, " mem_reg_property[%d].start_lo = 0x%08X\n", i, p->start_lo);
		dprintf(INFO, " mem_reg_property[%d].size_hi  = 0x%08X\n", i, p->size_hi);
		dprintf(INFO, " mem_reg_property[%d].size_lo  = 0x%08X\n", i, p->size_lo);
	}

	return 0;
}


int platform_atag_append(void *fdt)
{
	char *ptr;
	int offset;
	int ret = 0;

	offset = fdt_path_offset(fdt, "/memory");

	ptr = (char *)&g_boot_arg->orig_dram_info;
	ret = fdt_setprop(fdt, offset, "orig_dram_info", ptr, sizeof(dram_info_t));
	if (ret) goto exit;

	ptr = (char *)&g_boot_arg->mblock_info;
	ret = fdt_setprop(fdt, offset, "mblock_info", ptr, sizeof(mblock_info_t));
	if (ret) goto exit;

	ptr = (char *)&g_boot_arg->lca_reserved_mem;
	ret = fdt_setprop(fdt, offset, "lca_reserved_mem", ptr, sizeof(mem_desc_t));
	if (ret) goto exit;

	ptr = (char *)&g_boot_arg->tee_reserved_mem;
	ret = fdt_setprop(fdt, offset, "tee_reserved_mem", ptr, sizeof(mem_desc_t));
	if (ret) goto exit;

exit:

	if (ret)
		return 1;

	return 0;
}
#endif // NEW_MEMORY_RESERVED_MODEL

#endif //DEVICE_TREE_SUPPORT
