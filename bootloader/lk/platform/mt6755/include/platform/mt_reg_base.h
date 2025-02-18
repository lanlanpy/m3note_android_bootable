#ifndef __MT_REG_BASE
#define __MT_REG_BASE

#define BOOTROM_BASE (0x00000000)
#define BOOTSRAM_BASE (0x00100000)
#define IO_PHYS (0x10000000)

// APB Module cksys
#define CKSYS_BASE (0x10000000)
#define TOPCKGEN_BASE (0x10000000)

// APB Module infracfg_ao
#define INFRACFG_AO_BASE (0x10001000)

#define IOCFG_0_BASE (0x10002000)
#define IOCFG_1_BASE (0x10002200)
#define IOCFG_2_BASE (0x10002400)
#define IOCFG_3_BASE (0x10002600)
#define IOCFG_4_BASE (0x10002800)
#define IOCFG_5_BASE (0x10002A00)


// APB Module pericfg
#define PERICFG_BASE (0x10003000)

// APB Module dramc
#define DRAMC0_BASE (0x10004000)

// APB Module gpio
#define GPIO_BASE (0x10005000)

// APB Module sleep
#define SLEEP_BASE (0x10006000)

// APB Module toprgu
#define TOPRGU_BASE (0x10007000)

// APB Module apxgpt
#define APXGPT_BASE (0x10008000)

// APB Module rsvd
#define RSVD_BASE (0x10009000)

// APB Module sej
#define SEJ_BASE (0x1000A000)

// APB Module ap_cirq_eint
#define APIRQ_BASE (0x1000B000)

// APB Module smi
//#define SMI_COMMON_AO_BASE (0x1000C000)

// APB Module pmic_wrap
#define PWRAP_BASE (0x1000D000)

// APB Module device_apc_ao
#define DEVAPC_AO_BASE (0x1000E000)

// APB Module ddrphy
#define DDRPHY_BASE (0x1000F000)

// APB Module kp
#define KP_BASE (0x10010000)

// APB Module DRAMC1_BASE
//#define DRAMC1_BASE (0x10011000)

// APB Module DDRPHY1_BASE
//#define DDRPHY1_BASE (0x10012000)

// APB Module md32
#define MD32_BASE (0x10058000)

// APB Module dbgapb
#define DBGAPB_BASE (0x10100000)

// APB Module mcucfg
#define MCUCFG_BASE (0x10200000)

// APB Module ca7mcucfg
#define CA7MCUCFG_BASE (0x10200000)

// APB Module infracfg
#define INFRACFG_BASE (0x10201000)

// APB Module sramrom
#define SRAMROM_BASE (0x10202000)

// APB Module emi
#define EMI_BASE (0x10203000)

// APB Module sys_cirq
#define SYS_CIRQ_BASE (0x10204000)

// APB Module mm_iommu
#define M4U_BASE (0x10205000)

// APB Module efusec
#define EFUSEC_BASE (0x10206000)

// APB Module device_apc
#define DEVAPC_BASE (0x10207000)

// APB Module bus_dbg_tracker_cfg
#define BUS_DBG_BASE (0x10208000)

// APB Module apmixed
//#define APMIXED_BASE (0x10209000)

// APB Module fhctl
#define FHCTL_BASE (0x1000cF00)

// APB Module ccif
//#define AP_CCIF0_BASE (0x1020A000)

// APB Module ccif
//#define MD_CCIF0_BASE (0xA020B000)

// APB Module gpio1
//#define GPIO1_BASE (0x1020C000)

// APB Module infra_mbist
#define INFRA_MBIST_BASE (0x1020D000)

// APB Module dramc_conf_nao
#define DRAMC_NAO_BASE (0x1020E000)

// APB Module trng
#define TRNG_BASE (0x1020F000)

// APB Module gcpu
#define GCPU_BASE (0x10210000)

// APB Module gcpu_ns
#define GCPU_NS_BASE (0x10211000)

// APB Module gcpu_ns
#define GCE_BASE (0x10212000)

// APB Module dramc_conf_nao
#define DRAMC1_NAO_BASE (0x10213000)

// APB Module perisys_iommu
#define PERISYS_IOMMU_BASE (0x10214000)

// APB Module mipi_tx_config
#define MIPI_TX0_BASE (0x10215000)

// APB Module mipi_tx_config
#define MIPI_TX1_BASE (0x10216000)

// MIPI TX Config
#define MIPI_TX_CONFIG_BASE	(0x10012000)

// APB Module mipi_rx_ana_csi0
#define MIPI_RX_ANA_CSI0_BASE (0x10217000)

// APB Module mipi_rx_ana_csi1
#define MIPI_RX_ANA_CSI1_BASE (0x10218000)

// APB Module mipi_rx_ana_csi2
#define MIPI_RX_ANA_CSI2_BASE (0x10219000)

// APB Module ca9
#define CA9_BASE (0x10230000)

// APB Module gce
#define GCE_BASE (0x10212000)

// APB Module cq_dma
#define CQ_DMA_BASE (0x10212c00)

// APB Module ap_dma
#define AP_DMA_BASE (0x11000000)

// APB Module auxadc
#define AUXADC_BASE (0x11001000)

// APB Module uart
#define AP_UART0_BASE (0x11002000)

// APB Module uart
#define AP_UART1_BASE (0x11003000)

// APB Module uart
#define AP_UART2_BASE (0x11004000)

// APB Module uart
#define AP_UART3_BASE (0x11005000)

// APB Module pwm
#define PWM_BASE (0x11006000)

// APB Module i2c
#define I2C0_BASE (0x11007000)

// APB Module i2c
#define I2C1_BASE (0x11008000)

// APB Module i2c
#define I2C2_BASE (0x11009000)

// APB Module spi
#define SPI1_BASE (0x1100A000)

// APB Module therm_ctrl
#define THERM_CTRL_BASE (0x1100B000)

// APB Module btif
#define BTIF_BASE (0x1100C000)


// APB Module nfi
#define NFI_BASE (0x1100D000)

// APB Module nfiecc
#define NFIECC_BASE (0x1100E000)

// APB Module nli_arb
//#define NLI_ARB_BASE (0x1100F000)

// APB Module i2c
//#define I2C3_BASE (0x11010000)

// APB Module i2c
//#define I2C4_BASE (0x11011000)

// APB Module usb2
//#define USB_BASE (0x11200000)

// APB Module usb_sif
//#define USBSIF_BASE (0x11210000)

// APB Module audio
#define AUDIO_BASE (0x11220000)

// APB Module msdc
#define MSDC0_BASE (0x11230000)

// APB Module msdc
#define MSDC1_BASE (0x11240000)

// APB Module msdc
#define MSDC2_BASE (0x11250000)

// APB Module msdc
#define MSDC3_BASE (0x11260000)

// APB Module USB_1p
#define ICUSB_BASE (0x11270000)

// APB Module ssusb_top
#define USB3_BASE (0x11270000)

// APB Module ssusb_top_sif
#define USB3_SIF_BASE (0x11280000)

// APB Module ssusb_top_sif2
#define USB3_SIF2_BASE (0x11290000)

// APB Module mfg_top
//#define MFGCFG_BASE (0x13FFF000)

// APB Module han
//#define HAN_BASE (0x13000000)

// APB Module mmsys_config
#define MMSYS_CONFIG_BASE (0x14000000)

// APB Module mdp_rdma
#define MDP_RDMA0_BASE (0x14001000)

// APB Module mdp_rdma
#define MDP_RDMA1_BASE (0x14002000)

// APB Module mdp_rsz
#define MDP_RSZ0_BASE (0x14003000)

// APB Module mdp_rsz
#define MDP_RSZ1_BASE (0x14004000)

// APB Module mdp_rsz
#define MDP_RSZ2_BASE (0x14005000)

// APB Module disp_wdma
#define MDP_WDMA_BASE (0x14006000)

// APB Module mdp_wrot
#define MDP_WROT0_BASE (0x14007000)

// APB Module mdp_wrot
#define MDP_WROT1_BASE (0x14008000)

// APB Module mdp_tdshp
#define MDP_TDSHP0_BASE (0x14009000)

// APB Module mdp_tdshp
#define MDP_TDSHP1_BASE (0x1400a000)

// APB Module mdp_tdshp
#define MDP_CROP_BASE (0x1400b000)

// APB Module ovl
#define OVL0_BASE              (0x14008000)

// APB Module ovl
#define OVL1_BASE              (0x14009000)

// APB Module disp_rdma
#define DISP_RDMA0_BASE        (0x1400A000)

// APB Module disp_rdma
#define DISP_RDMA1_BASE        (0x1400B000)

// APB Module disp_wdma
#define DISP_WDMA0_BASE        (0x1400C000)

// APB Module disp_color
#define COLOR0_BASE            (0x1400D000)
// APB Module disp_ccorr
#define CCORR_BASE             (0x1400E000)
// APB Module disp_aal
#define DISP_AAL_BASE          (0x1400F000)

// APB Module disp_gamma
#define DISP_GAMMA_BASE        (0x14010000)

// APB Module disp_ufoe
//#define DISP_UFOE_BASE             (0x14011000)
#define DITHER_BASE            (0x14011000)

// APB Module disp_dsi
#define DSI0_BASE              (0x14012000)

// APB Module disp_dpi
#define DPI_BASE               (0x14013000)

// APB Module disp_pwm
#define DISP_PWM0_BASE         (0x1100E000)

// APB Module disp_mutex
#define MM_MUTEX_BASE          (0x14014000)

// APB Module smi_larb
#define SMI_LARB0_BASE         (0x14015000)

// APB Module smi
#define SMI_COMMON_BASE        (0x14016000)

// APB Module disp_wdma1
#define DISP_WDMA1_BASE        (0x14017000)

// APB Module disp_ovl0_2l
#define DISP_OVL0_2L_BASE      (0x14018000)

// APB Module disp_ovl1_2l
#define DISP_OVL1_2L_BASE      (0x14019000)

// APB Module smi_larb
#define SMI_LARB2_BASE (0x15001000)

// APB Module fake_eng
#define FAKE_ENG_BASE (0x15002000)

// APB Module imgsys
#define IMGSYS_BASE (0x15000000)

// APB Module cam1
#define CAM1_BASE (0x15004000)

// APB Module cam2
#define CAM2_BASE (0x15005000)

// APB Module cam3
#define CAM3_BASE (0x15006000)

// APB Module cam4
#define CAM4_BASE (0x15007000)

// APB Module camsv
#define CAMSV_BASE (0x15009000)

// APB Module camsv_top
#define CAMSV_TOP_BASE (0x15009000)

// APB Module csi2
#define CSI2_BASE (0x15008000)

// APB Module seninf
#define SENINF_BASE (0x15008000)

// APB Module seninf_tg
#define SENINF_TG_BASE (0x15008000)

// APB Module seninf_top
#define SENINF_TOP_BASE (0x15008000)

// APB Module seninf_mux
#define SENINF_MUX_BASE (0x15008000)

// APB Module mipi_rx_config
#define MIPI_RX_CONFIG_BASE (0x15008000)

// APB Module scam
#define SCAM_BASE (0x15008C00)

// APB Module ncsi2
#define NCSI2_BASE (0x15008000)

// APB Module ccir656
#define CCIR656_BASE (0x15008000)

// APB Module n3d_ctl
#define N3D_CTL_BASE (0x15008000)

// APB Module fdvt
#define FDVT_BASE (0x1500B000)

// APB Module vdecsys_config
#define VDEC_GCON_BASE (0x16000000)

// APB Module smi_larb
#define SMI_LARB1_BASE (0x16010000)

// APB Module vdtop
#define VDEC_BASE (0x16020000)

// APB Module vdtop
#define VDTOP_BASE (0x16020000)

// APB Module vld
#define VLD_BASE (0x16021000)

// APB Module vld_top
#define VLD_TOP_BASE (0x16021800)

// APB Module mc
#define MC_BASE (0x16022000)

// APB Module avc_vld
#define AVC_VLD_BASE (0x16023000)

// APB Module avc_mv
#define AVC_MV_BASE (0x16024000)

// APB Module vdec_pp
#define VDEC_PP_BASE (0x16025000)

// APB Module hevc_vld
#define HEVC_VLD_BASE (0x16028000)

// APB Module vp8_vld
#define VP8_VLD_BASE (0x16026800)

// APB Module vp6
#define VP6_BASE (0x16027000)

// APB Module vld2
#define VLD2_BASE (0x16027800)

// APB Module mc_vmmu
#define MC_VMMU_BASE (0x16028000)

// APB Module pp_vmmu
#define PP_VMMU_BASE (0x16029000)

// APB Module mjc_config
#define MJC_CONFIG_BASE (0x17000000)

// APB Module mjc_top
#define MJC_TOP_BASE (0x17001000)

// APB Module smi_larb
#define SMI_LARB4_BASE (0x17002000)

// APB Module venc_config
#define VENC_GCON_BASE (0x18000000)

// APB Module smi_larb
#define SMI_LARB3_BASE (0x18001000)

// APB Module venc
#define VENC_BASE (0x18002000)

// APB Module jpgenc
#define JPGENC_BASE (0x18003000)

// APB Module jpgdec
#define JPGDEC_BASE (0x18004000)

// APB Module audiosys
#define AUDIOSYS_BASE (0x11220000)

// rtc
#define RTC_BASE (0x4000)

//Marcos add for early porting
#define SYSRAM_BASE (0x19000000)
#define GIC_DIST_BASE (CA9_BASE + 0x1000)
#define GIC_CPU_BASE  (CA9_BASE + 0x2000)
#if defined(MACH_FPGA)
// FPGA only
#define DEVINFO_BASE (0x10001000)
#endif

/* hardware version register */
#define VER_BASE 0x08000000
#define APHW_CODE           (VER_BASE)
#define APHW_SUBCODE        (VER_BASE + 0x04)
#define APHW_VER            (VER_BASE + 0x08)
#define APSW_VER            (VER_BASE + 0x0C)

////////////////////////////////////////


/* Other define */
#define NON_BOOTABLE                0
#define RAW_BOOT                    1
#define FAT_BOOT                    2

#define CONFIG_STACKSIZE	    (128*1024)	  /* regular stack */

// xuecheng, define this because we use zlib for boot logo compression
#define CONFIG_ZLIB 	1

/*MTK Memory layout configuration*/
#define MAX_NR_BANK    4

#define DRAM_PHY_ADDR   0x40000000

#define RIL_SIZE 0

#define CFG_RAMDISK_LOAD_ADDR           (DRAM_PHY_ADDR + 0x4000000)
#define CFG_BOOTIMG_LOAD_ADDR           (DRAM_PHY_ADDR + 0x8000)
#define CFG_BOOTARGS_ADDR               (DRAM_PHY_ADDR + 0x100)

/*Command passing to Kernel */
#ifdef MACH_FPGA
#define COMMANDLINE_TO_KERNEL  "console=tty0 console=ttyMT0,921600n1 root=/dev/ram"
#else
#define COMMANDLINE_TO_KERNEL  "console=tty0 console=ttyMT3,921600n1 root=/dev/ram vmalloc=496M slub_max_order=0 slub_debug=O androidboot.hardware=mt6755"
#endif
#define CFG_FACTORY_NAME	"factory.img"
#define HAVE_LK_TEXT_MENU

#ifdef CONFIG_MTK_USB_UNIQUE_SERIAL
#define EFUSE_CTR_BASE		0x10206000	/* IO_PHYS + 0x01019000 */
//#define PDN_CTR			0xC0001020	/* IO_PHYS + 0x00001020 */
#define SERIAL_KEY_HI		(EFUSE_CTR_BASE + 0x0144)
#define SERIAL_KEY_LO		(EFUSE_CTR_BASE + 0x0140)
#endif

//ALPS00427972, implement the analog register formula
//Add here for eFuse, chip version checking -> analog register calibration
#define M_HW_RES3	                    0x10009170
//#define M_HW_RES3_PHY                   IO_PHYS+M_HW_RES3
#define RG_USB20_TERM_VREF_SEL_MASK     0xE000      //0b 1110,0000,0000,0000     15~13
#define RG_USB20_CLKREF_REF_MASK        0x1C00      //0b 0001,1100,0000,0000     12~10
#define RG_USB20_VRT_VREF_SEL_MASK      0x0380      //0b 0000,0011,1000,0000     9~7
//ALPS00427972, implement the analog register formula

#endif








