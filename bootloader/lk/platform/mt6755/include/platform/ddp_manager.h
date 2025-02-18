#ifndef __DDP_PATH_MANAGER_H__
#define __DDP_PATH_MANAGER_H__

#include "ddp_info.h"
#include "disp_event.h"
#include "ddp_path.h"

typedef void * cmdqRecHandle;

typedef enum {
	ENGINE_UFOE  = DISP_MODULE_UFOE,    /* add wdma0 after UFOE */
	ENGINE_OVL0  = DISP_MODULE_OVL0 ,   /* add wdma0 after OVL0 */
	ENGINE_OVL1  = DISP_MODULE_OVL1,    /* add wdma1 after OVL1*/
	ENGINE_OD    = DISP_MODULE_OD,      /* add wdma0 after OD */
} ENGINE_DUMP;

#define MAKE_DDP_IRQ_BIT(module, shift)   ((module<<16)|(0x1<<shift))
#define IRQBIT_MODULE(irqbit)             (irqbit >> 16)
#define IRQBIT_BIT(irqbit)                (irqbit & 0xffff)

/* IRQ and module are combined to consist DDP IRQ */
typedef enum {
	DDP_IRQ_RDMA0_REG_UPDATE =      (DISP_MODULE_RDMA0 <<16  | 0x1<<0),
	DDP_IRQ_RDMA0_START =           (DISP_MODULE_RDMA0 <<16  | 0x1<<1),
	DDP_IRQ_RDMA0_DONE  =           (DISP_MODULE_RDMA0 <<16  | 0x1<<2),
	DDP_IRQ_RDMA0_UNDERFLOW =       (DISP_MODULE_RDMA0 <<16  | 0x1<<3),
	DDP_IRQ_RDMA0_TARGET_LINE =     (DISP_MODULE_RDMA0 <<16  | 0x1<<4),

	DDP_IRQ_RDMA1_REG_UPDATE =      (DISP_MODULE_RDMA1 <<16  | 0x1<<0),
	DDP_IRQ_RDMA1_START =           (DISP_MODULE_RDMA1 <<16  | 0x1<<1),
	DDP_IRQ_RDMA1_DONE  =           (DISP_MODULE_RDMA1 <<16  | 0x1<<2),
	DDP_IRQ_RDMA1_UNDERFLOW =       (DISP_MODULE_RDMA1 <<16  | 0x1<<3),
	DDP_IRQ_RDMA1_TARGET_LINE =     (DISP_MODULE_RDMA1 <<16  | 0x1<<4),

	DDP_IRQ_RDMA2_REG_UPDATE =      (DISP_MODULE_RDMA2 <<16  | 0x1<<0),
	DDP_IRQ_RDMA2_START =           (DISP_MODULE_RDMA2 <<16  | 0x1<<1),
	DDP_IRQ_RDMA2_DONE  =           (DISP_MODULE_RDMA2 <<16  | 0x1<<2),
	DDP_IRQ_RDMA2_UNDERFLOW =       (DISP_MODULE_RDMA2 <<16  | 0x1<<3),
	DDP_IRQ_RDMA2_TARGET_LINE =     (DISP_MODULE_RDMA2 <<16  | 0x1<<4),

	DDP_IRQ_WDMA0_FRAME_COMPLETE =  (DISP_MODULE_WDMA0<<16   | 0x1<<0),
	DDP_IRQ_WDMA1_FRAME_COMPLETE =  (DISP_MODULE_WDMA1<<16   | 0x1<<0),

	DDP_IRQ_DSI0_EXT_TE =           (DISP_MODULE_DSI0 <<16  | 0x1<<2),
	DDP_IRQ_DSI0_FRAME_DONE =       (DISP_MODULE_DSI0 <<16  | 0x1<<4),

	DDP_IRQ_UNKNOW =                (DISP_MODULE_UNKNOWN<<16 | 0x1<<0),

} DDP_IRQ_BIT;
/* path handle */
typedef void* disp_path_handle;

/* Init ddp manager, now only register irq handler to ddp_irq.c
  * return 0 if ok or -1 if fail.
*/
int dpmgr_init(void);

/* create disp path handle , it will assign mutex to this handle, and cache this handle
  *  to modules in scenario, and will assign default irq event to this handle.
 * return NULL if fail to create handle.
 * scenario:  used to demininate path, and  ddp manager will do operations
 *                according to current scenario.
 *  cmdq_handle :  will save current config cmdqhandle, and if cmdq is enable , will
                            use cmdq to write regitsers.
*/
disp_path_handle dpmgr_create_path(DDP_SCENARIO_ENUM scenario, cmdqRecHandle cmdq_handle);

/* destroy path, it will release mutex to pool, and disconnect path,
  * clear  mapping between handle and modules.
 * return 0;
*/
int dpmgr_destroy_path(disp_path_handle dp_handle);

/* add dump to path, for primary path , support OVL0, UFOE and OD.
  * for sub path ,support OVL1.
 * return 0 if success or -1 if fail.
 * dp_handle: disp path handle.
*/
int dpmgr_path_add_dump(disp_path_handle dp_handle, ENGINE_DUMP engine,int encmdq);

/* remove dump to path. shoud match between  add dump and remove dump.
 * return 0 if success or -1 if fail.
 * dp_handle: disp path handle.
 * encmdq: 1 use command queue, 0 not.
*/
int dpmgr_path_remove_dump(disp_path_handle dp_handle, int encmdq);


/* query current dst module.
 * return module enum.
 * dp_handle: disp path handle.
*/
DISP_MODULE_ENUM dpmgr_path_get_dst_module(disp_path_handle dp_handle);


/* set dst module, the default dst module maybe not right, so set real dst module on this path.
 * return 0.
 * dp_handle: disp path handle.
 * dst_module(one of bellow):
 *        DISP_MODULE_DSI0
 *        DISP_MODULE_DSI1
 *        DISP_MODULE_DSIDUAL(DISP_MODULE_DSI0+DISP_MODULE_DSI1)
*         DISP_MODULE_DPI
*/
int dpmgr_path_set_dst_module(disp_path_handle dp_handle,DISP_MODULE_ENUM dst_module);

/* set mode type(sof source): cmd or video mode.
 * return 0.
 * dp_handle: disp path handle.
 * is_vdo_mode:  0 is cmd mode, 1 is video mode.
*/
int dpmgr_path_set_video_mode(disp_path_handle dp_handle, int is_vdo_mode);

/* init path , it will set mutex according to modules on this path and sof sorce.
 * and it will connect path , then initialize modules on this path.
 * return 0.
 * dp_handle: disp path handle.
 * encmdq: 1 use command queue, 0 not.
*/
int dpmgr_path_init(disp_path_handle dp_handle, int encmdq);


/* deinit path , it will clear mutex and dissconnect path.
 * return 0.
 * dp_handle: disp path handle.
 * encmdq: 1 use command queue, 0 not.
*/
int dpmgr_path_deinit(disp_path_handle dp_handle, int encmdq);


/* start path , it will start this path by calling each drviers start function.
 * return 0.
 * dp_handle: disp path handle.
 * encmdq: 1 use command queue, 0 not.
*/
int dpmgr_path_start(disp_path_handle dp_handle, int encmdq);


/* start path , it will stop this path by calling each drviers stop function.
 * return 0.
 * dp_handle: disp path handle.
 * encmdq: 1 use command queue, 0 not.
*/
int dpmgr_path_stop(disp_path_handle dp_handle, int encmdq);


/* reset path , it will reset this path by calling each drviers reset function.
 * return 0.
 * dp_handle: disp path handle.
 * encmdq: 1 use command queue, 0 not.
*/
int dpmgr_path_reset(disp_path_handle dp_handle, int encmdq);


/* config data , it will config input or output data of  this path.
 * now config contains three parts:
 *    1. dst dirty .it means dst with & dst hight need be updated.
 *    2. ovl dirty. it means ovl config need be updated.
 *    3. rdma dirty. it means rdma need be updated.
 *    4. wdma dirty. it means wdam need be updated.
 * return 0.
 * dp_handle: disp path handle.
 * encmdq: 1 use command queue, 0 not.
*/
int dpmgr_path_config(disp_path_handle dp_handle, disp_ddp_path_config * config, int encmdq);


/* path flush, this will enable mutex
 * return 0.
 * dp_handle: disp path handle.
 * encmdq: 1 use command queue, 0 not.
*/
int dpmgr_path_flush(disp_path_handle dp_handle,int encmdq);


/* this will dump modules info on this path.
 * return 0.
 * dp_handle: disp path handle.
*/
int dpmgr_check_status(disp_path_handle dp_handle);


/* this will deal with cmdq message:
 * return 0.
 * dp_handle: disp path handle.
  *  trigger_loop_handle:  triger thread.
 * state :
       CMDQ_BEFORE_STREAM_SOF                      // before sof.
    CMDQ_WAIT_STREAM_EOF_EVENT               // wait sof
    CMDQ_CHECK_IDLE_AFTER_STREAM_EOF    // check sof
    CMDQ_AFTER_STREAM_EOF                        // after sof
*/
int dpmgr_path_build_cmdq(disp_path_handle dp_handle, void *trigger_loop_handle, CMDQ_STATE state);


/* this will trigger this path. it will trigger each module and enable mutex.
 * return 0.
 * dp_handle:               disp path handle.
 * trigger_loop_handle : trigger thread.
*/
int dpmgr_path_trigger(disp_path_handle dp_handle, void *trigger_loop_handle, int encmdq);


/* set signal to event. this if not irq signal, but user send event
 * return 0.
 * dp_handle:   disp path handle.
 * event: path event.
*/
int dpmgr_signal_event(disp_path_handle dp_handle, DISP_PATH_EVENT event);


/* enable init will initialize wakequeue.
 * return 0.
 * dp_handle:   disp path handle.
 * event: path event.
*/
int dpmgr_enable_event(disp_path_handle dp_handle, DISP_PATH_EVENT event);


/* disable event, related irq will not be received.
 * return 0.
 * dp_handle:   disp path handle.
 * event: path event.
*/
int dpmgr_disable_event(disp_path_handle dp_handle, DISP_PATH_EVENT event);

/* map event to irq can change mappling between path event and irq .
 * return 0.
 * dp_handle:   disp path handle.
 * event: path event.
 * irq_bit
*/
int dpmgr_map_event_to_irq(disp_path_handle dp_handle, DISP_PATH_EVENT event, DDP_IRQ_BIT irq_bit);


/* wait event, timeout (ms).
 * return
 *     < 0,  error.
 *     0      timeout
 *     > 0   no  timeout
 * event : disp event.
 * timeout :(ms).
*/
int dpmgr_wait_event_timeout(disp_path_handle dp_handle, DISP_PATH_EVENT event, int timeout);

/* wait event
 * return :
 * 0 , wait succesfull.
 * <0, wait error.
 * event : disp event.
 * timeout :(ms).
*/
int dpmgr_wait_event(disp_path_handle dp_handle, DISP_PATH_EVENT event);


/* power on, turn on each modules clk.
 * return 0.
* dp_handle: disp path handle.
* encmdq: 1 use command queue, 0 not.
*/
int dpmgr_path_power_on(disp_path_handle dp_handle, CMDQ_SWITCH encmdq);

/* power 0ff,  turn off each modules clk, if all hande are closed. top clock will be off.
 * return 0.
* dp_handle: disp path handle.
* encmdq: 1 use command queue, 0 not.
*/
int dpmgr_path_power_off(disp_path_handle dp_handle, CMDQ_SWITCH encmdq);


/* set lcm utils. now only dis/dpi used.
 * return 0.
* dp_handle: disp path handle.
* lcm_drv: lcm driver
*/
int dpmgr_set_lcm_utils(disp_path_handle dp_handle, void *lcm_drv);


/* check if this path is busy. it wil check each module on this path.
 * return 0.
* dp_handle: disp path handle.
*/
int dpmgr_path_is_busy(disp_path_handle dp_handle);


/* add parameter to this path.
 * return 0.
 * dp_handle: disp path handle.
 * io_evnet: not defined.
 * data    :  data.
*/

int dpmgr_path_is_idle(disp_path_handle dp_handle);

int dpmgr_path_set_parameter(disp_path_handle dp_handle, int io_evnet, void * data);


/* get parameter of this path.
 * return 0.
 * dp_handle: disp path handle.
 * io_evnet: not defined.
 * data    :  data.
*/

int dpmgr_path_get_parameter(disp_path_handle dp_handle,int io_evnet, void * data );


/* get last config parameter of path
 * return  pointer to last config
 * dp_handle: disp path handle.
*/
disp_ddp_path_config *dpmgr_path_get_last_config(disp_path_handle dp_handle);



#endif
