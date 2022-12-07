#include <logging/log.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>

#include "ads1299_cfg.h"
#include "ads1299.h"

#define ADS1299_SPI DT_NODELABEL(ads1299spi)

static void ADS1299_Platform_Spi_TxRx_Cfg(uint8_t *pTxData, uint16_t tx_len, uint8_t *pRxData, uint16_t rx_len);
static void ADS1299_Platform_Spi_TxRx_GetData(uint8_t *pTxData, uint16_t tx_len, uint8_t *pRxData, uint16_t rx_len);
static void ADS1299_Platform_IoCs_Write(uint8_t board, uint8_t level);
static void ADS1299_Platform_IoStart_Write(uint8_t level);
static void ADS1299_Platform_IoReset_Write(uint8_t level);
static void ADS1299_Platform_IoPwdn_Write(uint8_t level);
static uint8_t ADS1299_Platform_IoDrdy_Read(void);
static void ADS1299_Platform_Delay(uint16_t ms);

/* === Define HW Interface for ADS1299 === */
ADS1299_HwConfigType ADS1299_Hw = {
    .Spi_TransmitReceive_Cfg     = &ADS1299_Platform_Spi_TxRx_Cfg,
    .Spi_TransmitReceive_GetData = &ADS1299_Platform_Spi_TxRx_GetData,
    .IoCs_Write                  = &ADS1299_Platform_IoCs_Write,
    .IoStart_Write               = &ADS1299_Platform_IoStart_Write,
    .IoReset_Write               = &ADS1299_Platform_IoReset_Write,
    .IoPwdn_Write                = &ADS1299_Platform_IoPwdn_Write,
    .IoDrdy_Read                 = &ADS1299_Platform_IoDrdy_Read,
    .Delay_Ms                    = &ADS1299_Platform_Delay,
};

struct gpio_dt_spec ADS1299O_RST = GPIO_DT_SPEC_GET(DT_NODELABEL(ads1299rst), gpios);
struct gpio_dt_spec ADS1299O_START = GPIO_DT_SPEC_GET(DT_NODELABEL(ads1299start), gpios);
struct gpio_dt_spec ADS1299O_PWDN = GPIO_DT_SPEC_GET(DT_NODELABEL(ads1299pwdn), gpios);
struct gpio_dt_spec ADS1299I_DRDY = GPIO_DT_SPEC_GET(DT_NODELABEL(ads1299drdy), gpios);

#if(ADS1299_GETDATA_ASYNC == 1)
struct gpio_callback drdy_cb_data;
ads1299_data_done_cb_t data_done_cb = NULL;
#endif

const struct device *spi_dev;
struct k_poll_signal spi_done_sig = K_POLL_SIGNAL_INITIALIZER(spi_done_sig);

struct spi_cs_control spim_cs = {
	.gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(ads1299cs_0)),
	.delay = 0,
};

static const struct spi_config spi_cfg = {
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB |
				 SPI_MODE_CPOL,
	.frequency = 4000000,
	.slave = 0,
	.cs = &spim_cs,
};

static void ADS1299_Platform_Spi_TxRx_Cfg(uint8_t *pTxData, uint16_t tx_len, uint8_t *pRxData, uint16_t rx_len)
{
    struct spi_buf tx_buf = {
		.buf = pTxData,
		.len = tx_len
	};

    struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1
	};

    struct spi_buf rx_buf = {
		.buf = pRxData,
		.len = rx_len
	};

    struct spi_buf_set rx = {
		.buffers = &rx_buf,
		.count = 1
	};

    // Start transaction
	int error = spi_transceive(spi_dev, &spi_cfg, &tx, &rx);

}

static void ADS1299_Platform_Spi_TxRx_GetData(uint8_t *pTxData, uint16_t tx_len, uint8_t *pRxData, uint16_t rx_len)
{
    struct spi_buf tx_buf = {
		.buf = pTxData,
		.len = tx_len
	};

    struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1
	};

    struct spi_buf rx_buf = {
		.buf = pRxData,
		.len = rx_len
	};

    struct spi_buf_set rx = {
		.buffers = &rx_buf,
		.count = 1
	};

    // Start transaction
	int error = spi_transceive(spi_dev, &spi_cfg, &tx, &rx);
	
	// k_poll_signal_reset(&spi_slave_done_sig);
	// int error = spi_transceive_async(spi_dev, &spi_cfg, &tx, &rx, &spi_done_sig);
}

static void ADS1299_Platform_IoCs_Write(uint8_t board, uint8_t level)
{
    if(board == 0)
    {
		// Empty
    }
}

static void ADS1299_Platform_IoStart_Write(uint8_t level)
{
	if(level == ADS1299_IO_LOW)
	{
        gpio_pin_set_dt(&ADS1299O_START, GPIO_ACTIVE_LOW);
	}
	else
	{
        gpio_pin_set_dt(&ADS1299O_START, GPIO_ACTIVE_HIGH);
	}
}

static void ADS1299_Platform_IoReset_Write(uint8_t level)
{
	if(level == ADS1299_IO_LOW)
	{
		gpio_pin_set_dt(&ADS1299O_RST, GPIO_ACTIVE_LOW);
	}
	else
	{
		gpio_pin_set_dt(&ADS1299O_RST, GPIO_ACTIVE_HIGH);
	}
}

static void ADS1299_Platform_IoPwdn_Write(uint8_t level)
{
    if(level == ADS1299_IO_LOW)
	{
		gpio_pin_set_dt(&ADS1299O_PWDN, GPIO_ACTIVE_LOW);
	}
	else
	{
		gpio_pin_set_dt(&ADS1299O_PWDN, GPIO_ACTIVE_HIGH);
	}
}

static uint8_t ADS1299_Platform_IoDrdy_Read()
{
	if(gpio_pin_get_dt(&ADS1299I_DRDY) == GPIO_ACTIVE_LOW)
	{
		return ADS1299_IO_LOW;
	}
	else
	{
		return ADS1299_IO_HIGH;
	}
	
}

static void ADS1299_Platform_Delay(uint16_t ms) 
{
    k_msleep(ms);
}

#if(ADS1299_GETDATA_ASYNC == 1)
void ADS1299_Platform_DataDoneCB_Init(ads1299_data_done_cb_t cb)
{
	data_done_cb = cb;
}

void ADS1299_Platform_DrdyCallback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	ADS1299_GetSensorData(0);

	// while (k_msgq_put(&gb_ads1299_msgq, &ADS1299_EEGRawDataBuffer[0][3], K_NO_WAIT) != 0) {
    //     /* message queue is full: purge old data & try again */
    //     k_msgq_purge(&gb_ads1299_msgq);
    // }
	if(data_done_cb)
	{
		data_done_cb();
	}


}
#endif

void ADS1299_Platform_Init(void)
{
    int ret;

	if (!device_is_ready(ADS1299O_START.port)) {
		return;
	}
	if (!device_is_ready(ADS1299O_PWDN.port)) {
		return;
	}

    ret = gpio_pin_configure_dt(&ADS1299O_START, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
    ret = gpio_pin_configure_dt(&ADS1299O_RST, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}
    ret = gpio_pin_configure_dt(&ADS1299O_PWDN, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

#if(ADS1299_GETDATA_ASYNC == 0)
    ret = gpio_pin_configure_dt(&ADS1299I_DRDY, GPIO_INPUT);
	if (ret < 0) {
		return;
	}
#else
    ret = gpio_pin_interrupt_configure_dt(&ADS1299I_DRDY, GPIO_INT_EDGE_TO_INACTIVE);
	if (ret < 0) {
		return;
	}
	gpio_init_callback(&drdy_cb_data, ADS1299_Platform_DrdyCallback, BIT(ADS1299I_DRDY.pin));
	gpio_add_callback(ADS1299I_DRDY.port, &drdy_cb_data);
#endif

	// gpio_pin_set_dt(&ADS1299O_PWDN, 1);
	// gpio_pin_set_dt(&ADS1299O_START, GPIO_ACTIVE_HIGH);
	// gpio_pin_set_dt(&ADS1299O_RST, GPIO_ACTIVE_HIGH);
	
    spi_dev = DEVICE_DT_GET(ADS1299_SPI);
	if(!device_is_ready(spi_dev)) {
		printk("SPI master device not ready!\n");
	}
	if(!device_is_ready(spim_cs.gpio.port)){
		printk("SPI master chip select device not ready!\n");
	}
}