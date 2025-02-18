/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
#ifndef MT_COMMON_H_
#define MT_COMMON_H_

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef LOGE
// TODO: restore ui_print for LOGE
#define LOGE(...) fprintf(stdout, "E:" __VA_ARGS__)

typedef struct {
    const char* mount_point;  // eg. "/cache".  must live in the root directory.

    const char* fs_type;      // "yaffs2" or "ext4" or "vfat"

    const char* blk_device;   // MTD partition name if fs_type == "yaffs"
                              // block device if fs_type == "ext4" or "vfat"

    const char* blk_device2;  // alternative device to try if fs_type
                              // == "ext4" or "vfat" and mounting
                              // 'device' fails

    long long length;         // (ext4 partition only) when
                              // formatting, size to use for the
                              // partition.  0 or negative number
                              // means to format all but the last
                              // (that much).
} Volume;


bool is_support_gpt(void);

#define PRELOADER_PART "/dev/block/mmcblk0boot0"
#define PRELOADER2_PART "/dev/block/mmcblk0boot1"
#define BOOT_PART      "/dev/block/platform/mtk-msdc.0/by-name/boot"
#define CACHE_PART     "/dev/block/platform/mtk-msdc.0/by-name/cache"
#define FAT_PART       "/dev/block/platform/mtk-msdc.0/by-name/intsd"
#define SYSTEM_PART    "/dev/block/platform/mtk-msdc.0/by-name/system"
#define DATA_PART      "/dev/block/platform/mtk-msdc.0/by-name/userdata"
#define MISC_PART      "/dev/block/platform/mtk-msdc.0/by-name/para"
#define RECOVERY_PART  "/dev/block/platform/mtk-msdc.0/by-name/recovery"
#define CUSTOM_PART    "/dev/block/platform/mtk-msdc.0/by-name/custom"
#define VENDOR_PART    "/dev/block/platform/mtk-msdc.0/by-name/vendor"
#define LOGO_PART      "/dev/block/platform/mtk-msdc.0/by-name/logo"
#define LK_PART        "/dev/block/platform/mtk-msdc.0/by-name/lk"
#define TEE1_PART      "/dev/block/platform/mtk-msdc.0/by-name/tee1"
#define TEE2_PART      "/dev/block/platform/mtk-msdc.0/by-name/tee2"
#define PERSIST_PART   "/dev/block/platform/mtk-msdc.0/by-name/persist"
#define NVDATA_PART    "/dev/block/platform/mtk-msdc.0/by-name/nvdata"
#define MT_GPT_PART    "/dev/block/platform/mtk-msdc.0/by-name"

static inline bool support_gpt(void) {\
    int fd = open(MISC_PART, O_RDONLY);\
    if (fd == -1) {\
        return false;\
    } else {\
        close(fd);\
        return true;\
    }\
}

#ifdef __cplusplus
}
#endif

#endif
