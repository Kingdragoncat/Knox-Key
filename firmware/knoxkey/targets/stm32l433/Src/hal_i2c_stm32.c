#include "hal/atca_hal.h"
#include "atca_device.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

#define HAL_I2C_BUS_RETRY_COUNT 2

ATCA_STATUS hal_i2c_init(ATCAIface iface, ATCAIfaceCfg *cfg) {
    (void)iface;
    (void)cfg;
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_post_init(ATCAIface iface) {
    (void)iface;
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t word_address, uint8_t *txdata, int txlength) {
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint16_t address = ATCA_IFACECFG_I2C_ADDRESS(cfg) >> 1;

    txdata[0] = word_address;
    if (HAL_I2C_Master_Transmit(&hi2c1, address << 1, txdata, txlength + 1, 1000) != HAL_OK) {
        return ATCA_COMM_FAIL;
    }
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength) {
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint16_t address = ATCA_IFACECFG_I2C_ADDRESS(cfg) >> 1;
    (void)word_address; /* Not used for standard recv */

    int retries = HAL_I2C_BUS_RETRY_COUNT;
    HAL_StatusTypeDef status;
    do {
        status = HAL_I2C_Master_Receive(&hi2c1, address << 1, rxdata, *rxlength, 1000);
        if (status == HAL_OK) {
            return ATCA_SUCCESS;
        }
        retries--;
    } while (retries > 0);
    return ATCA_COMM_FAIL;
}

ATCA_STATUS hal_i2c_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen) {
    (void)iface;
    if (option == ATCA_HAL_CONTROL_WAKE) {
        ATCAIfaceCfg *cfg = atgetifacecfg(iface);
        (void)cfg; /* Not used for wake since we address 0x00 */
        
        /* Send a dummy byte with address 0x00 at 100kHz.
           Since it's 0x00, ATECC608 will ignore the data but the SDA low time will wake it up */
        uint8_t dummy = 0;
        HAL_I2C_Master_Transmit(&hi2c1, 0x00, &dummy, 1, 100);
        
        /* Wait tWHI + tWLO (wake delay) which is around 1.5ms */
        hal_delay_ms(2);
        return ATCA_SUCCESS;
    }
    else if (option == ATCA_HAL_CONTROL_IDLE) {
        ATCAIfaceCfg *cfg = atgetifacecfg(iface);
        uint16_t address = ATCA_IFACECFG_I2C_ADDRESS(cfg) >> 1;
        uint8_t idle_cmd = 0x02;
        HAL_I2C_Master_Transmit(&hi2c1, address << 1, &idle_cmd, 1, 100);
        return ATCA_SUCCESS;
    }
    else if (option == ATCA_HAL_CONTROL_SLEEP) {
        ATCAIfaceCfg *cfg = atgetifacecfg(iface);
        uint16_t address = ATCA_IFACECFG_I2C_ADDRESS(cfg) >> 1;
        uint8_t sleep_cmd = 0x01;
        HAL_I2C_Master_Transmit(&hi2c1, address << 1, &sleep_cmd, 1, 100);
        return ATCA_SUCCESS;
    }
    return ATCA_UNIMPLEMENTED;
}

ATCA_STATUS hal_i2c_release(void *hal_data) {
    (void)hal_data;
    return ATCA_SUCCESS;
}

void hal_delay_ms(uint32_t delay) {
    HAL_Delay(delay);
}

void hal_delay_us(uint32_t delay) {
    /* Crude busy wait for microsecond delay */
    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) * 1000 < delay) {
    }
}
