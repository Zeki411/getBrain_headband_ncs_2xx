#ifndef __ADS1299_CFG_H__
#define __ADS1299_CFG_H__

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr.h>
#include <soc.h>
#include <logging/log.h>

// LOG_MODULE_REGISTER(ADS1299_Driver,LOG_LEVEL_DBG);

#ifdef __cplusplus
extern "C" {
#endif

#define ADS1299_GETDATA_ASYNC  1
#define ADS1299_USING_BIAS_DRIVE_CIRCUIT 1

/* */
typedef struct {
    void (*Spi_TransmitReceive_Cfg)(uint8_t *, uint16_t, uint8_t *, uint16_t);
    void (*Spi_TransmitReceive_GetData)(uint8_t *, uint16_t, uint8_t *, uint16_t);
    void (*IoCs_Write)(uint8_t board, uint8_t level);
    void (*IoStart_Write)(uint8_t level); 
    void (*IoReset_Write)(uint8_t level);
    void (*IoPwdn_Write)(uint8_t level);
    uint8_t (*IoDrdy_Read)();
    void (*Delay_Ms)(uint16_t ms);
} ADS1299_HwConfigType;

typedef void (*ads1299_data_done_cb_t)(void);

/* === Define HW Option for ADS1299 === */
#define ADS1299_USING_HW_RESET
#define ADS1299_USING_HW_START_STOP
#define ADS1299_NUMBER_OF_USED_BOARD      (1U)

#define ADS1299_IO_HIGH 1
#define ADS1299_IO_LOW  0

/* === Define HW Interface for ADS1299 === */
#define ADS1299_LOG_DBG(STR) \
                // LOG_DBG(STR)

#define ADS1299_LOG_ERR(STR) \
                // LOG_ERR(STR)

    
/* HW Specific Function */
extern void ADS1299_Platform_Init(void);
#if(ADS1299_GETDATA_ASYNC == 1)
void ADS1299_Platform_DataDoneCB_Init(ads1299_data_done_cb_t cb);
#endif
extern ADS1299_HwConfigType ADS1299_Hw;

#ifdef __cplusplus
}
#endif  /* __ADS1299_CFG_H__ */

#endif
