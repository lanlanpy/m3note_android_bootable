/*****************************************************************************
*
* Filename:
* ---------
*   fan5405.h
*
* Project:
* --------
*   Android
*
* Description:
* ------------
*   fan5405 header file
*
* Author:
* -------
*
****************************************************************************/

#ifndef _fan5405_SW_H_
#define _fan5405_SW_H_

//#define HIGH_BATTERY_VOLTAGE_SUPPORT

#define fan5405_CON0      0x00
#define fan5405_CON1      0x01
#define fan5405_CON2      0x02
#define fan5405_CON3      0x03
#define fan5405_CON4      0x04
#define fan5405_CON5      0x05
#define fan5405_CON6      0x06

/**********************************************************
  *
  *   [MASK/SHIFT] 
  *
  *********************************************************/
    //CON0
#define CON0_TMR_RST_MASK   0x01
#define CON0_TMR_RST_SHIFT  7
    
#define CON0_OTG_MASK       0x01
#define CON0_OTG_SHIFT      7
    
#define CON0_EN_STAT_MASK   0x01
#define CON0_EN_STAT_SHIFT  6
    
#define CON0_STAT_MASK      0x03
#define CON0_STAT_SHIFT     4
    
#define CON0_BOOST_MASK     0x01
#define CON0_BOOST_SHIFT    3
    
#define CON0_FAULT_MASK     0x07
#define CON0_FAULT_SHIFT    0
    
    //CON1
#define CON1_LIN_LIMIT_MASK     0x03
#define CON1_LIN_LIMIT_SHIFT    6
    
#define CON1_LOW_V_MASK     0x03
#define CON1_LOW_V_SHIFT    4
    
#define CON1_TE_MASK        0x01
#define CON1_TE_SHIFT       3
    
#define CON1_CE_MASK        0x01
#define CON1_CE_SHIFT       2
    
#define CON1_HZ_MODE_MASK   0x01
#define CON1_HZ_MODE_SHIFT  1
    
#define CON1_OPA_MODE_MASK  0x01
#define CON1_OPA_MODE_SHIFT 0
    
    //CON2
#define CON2_OREG_MASK    0x3F
#define CON2_OREG_SHIFT   2
    
#define CON2_OTG_PL_MASK    0x01
#define CON2_OTG_PL_SHIFT   1
    
#define CON2_OTG_EN_MASK    0x01
#define CON2_OTG_EN_SHIFT   0
    
    //CON3
#define CON3_VENDER_CODE_MASK   0x07
#define CON3_VENDER_CODE_SHIFT  5
    
#define CON3_PIN_MASK           0x03
#define CON3_PIN_SHIFT          3
    
#define CON3_REVISION_MASK      0x07
#define CON3_REVISION_SHIFT     0
    
    //CON4
#define CON4_RESET_MASK     0x01
#define CON4_RESET_SHIFT    7
    
#define CON4_I_CHR_MASK     0x07
#define CON4_I_CHR_SHIFT    4
    
#define CON4_I_TERM_MASK    0x07
#define CON4_I_TERM_SHIFT   0
    
    //CON5
#define CON5_DIS_VREG_MASK      0x01
#define CON5_DIS_VREG_SHIFT     6
    
#define CON5_IO_LEVEL_MASK      0x01
#define CON5_IO_LEVEL_SHIFT     5
    
#define CON5_SP_STATUS_MASK     0x01
#define CON5_SP_STATUS_SHIFT    4
    
#define CON5_EN_LEVEL_MASK      0x01
#define CON5_EN_LEVEL_SHIFT     3
    
#define CON5_VSP_MASK           0x07
#define CON5_VSP_SHIFT          0
    
    //CON6
#define CON6_ISAFE_MASK     0x07
#define CON6_ISAFE_SHIFT    4
    
#define CON6_VSAFE_MASK     0x0F
#define CON6_VSAFE_SHIFT    0

/**********************************************************
  *
  *   [Extern Function] 
  *
  *********************************************************/
    //CON0----------------------------------------------------
extern void fan5405_set_tmr_rst(kal_uint32 val);
extern kal_uint32 fan5405_get_otg_status(void);
extern void fan5405_set_en_stat(kal_uint32 val);
extern kal_uint32 fan5405_get_chip_status(void);
extern kal_uint32 fan5405_get_boost_status(void);
extern kal_uint32 fan5405_get_fault_status(void);
//CON1----------------------------------------------------
extern void fan5405_set_input_charging_current(kal_uint32 val);
extern void fan5405_set_v_low(kal_uint32 val);
extern void fan5405_set_te(kal_uint32 val);
extern void fan5405_set_ce(kal_uint32 val);
extern void fan5405_set_hz_mode(kal_uint32 val);
extern void fan5405_set_opa_mode(kal_uint32 val);
//CON2----------------------------------------------------
extern void fan5405_set_oreg(kal_uint32 val);
extern void fan5405_set_otg_pl(kal_uint32 val);
extern void fan5405_set_otg_en(kal_uint32 val);
//CON3----------------------------------------------------
extern kal_uint32 fan5405_get_vender_code(void);
extern kal_uint32 fan5405_get_pn(void);
extern kal_uint32 fan5405_get_revision(void);
//CON4----------------------------------------------------
extern void fan5405_set_reset(kal_uint32 val);
extern void fan5405_set_iocharge(kal_uint32 val);
extern void fan5405_set_iterm(kal_uint32 val);
//CON5----------------------------------------------------
extern void fan5405_set_dis_vreg(kal_uint32 val);
extern void fan5405_set_io_level(kal_uint32 val);
extern kal_uint32 fan5405_get_sp_status(void);
extern kal_uint32 fan5405_get_en_level(void);
extern void fan5405_set_vsp(kal_uint32 val);
//CON6----------------------------------------------------
extern void fan5405_set_i_safe(kal_uint32 val);
extern void fan5405_set_v_safe(kal_uint32 val);
//---------------------------------------------------------
extern void fan5405_hw_init(void);
extern void fan5405_charging_enable(kal_uint32 bEnable);
extern void fan5405_dump_register(void);
extern unsigned int  fan5405_reg_config_interface (unsigned char RegNum, unsigned char val);
extern void fan5405_hw_init(void);

#endif // _fan5405_SW_H_
