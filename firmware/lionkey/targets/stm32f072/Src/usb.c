/**
 * @file    usb.c
 * @brief   USB initialization and HID task — Knox Key STM32F072
 *
 * Uses TinyUSB with the stm32_fsdev driver. The STM32F072 has an integrated
 * USB 2.0 Full-Speed peripheral with internal PHY (no external components needed).
 */

#include "main.h"
#include "app.h"
#include "tusb.h"
#include "utils.h"

static_assert(sizeof(ctaphid_packet_t) == CFG_TUD_ENDPOINT0_SIZE, "invalid sizeof(ctaphid_packet_t)");
/* Reduced queue depth to save RAM on the 16KB STM32F072.
 * The full 129-packet queue (8KB!) is too large — 8 packets is enough
 * for pipelining since we drain the queue in app_hid_task(). */
#define KNOXKEY_HID_SEND_QUEUE_DEPTH 8
TU_FIFO_DEF(app_hid_report_send_queue, KNOXKEY_HID_SEND_QUEUE_DEPTH, ctaphid_packet_t, false);

void app_hid_task(void) {
	tud_task(); /* tinyusb device task */
	app_hid_report_send_queue_send_one_if_possible();
}

void app_hid_report_send_queue_add(const ctaphid_packet_t *packet, _Bool fail_if_full) {
	if (!tu_fifo_write(&app_hid_report_send_queue, packet)) {
		if (fail_if_full) {
			Error_Handler();
		}
	}
}

void app_hid_report_send_queue_send_one_if_possible(void) {
	if (tu_fifo_empty(&app_hid_report_send_queue)) {
		return;
	}

	ctaphid_packet_t packet;

	if (!tu_fifo_peek(&app_hid_report_send_queue, &packet)) {
		return;
	}

	if (tud_hid_report(0, &packet, sizeof(ctaphid_packet_t))) {
		tu_fifo_advance_read_pointer(&app_hid_report_send_queue, 1);
	}
}

_Bool app_hid_report_send_queue_is_empty(void) {
	return tu_fifo_empty(&app_hid_report_send_queue);
}

void usb_init(void) {

	/* Enable USB peripheral clock */
	__HAL_RCC_USB_CLK_ENABLE();

	/* Enable CRS (Clock Recovery System) for HSI48 calibration from USB SOF */
	__HAL_RCC_CRS_CLK_ENABLE();
	RCC_CRSInitTypeDef crs_init = {0};
	crs_init.Prescaler = RCC_CRS_SYNC_DIV1;
	crs_init.Source = RCC_CRS_SYNC_SOURCE_USB;
	crs_init.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
	crs_init.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000);
	crs_init.ErrorLimitValue = 34;
	crs_init.HSI48CalibrationValue = 32;
	HAL_RCCEx_CRSConfig(&crs_init);

	/* Enable USB interrupt */
	HAL_NVIC_SetPriority(USB_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USB_IRQn);

	/* Init TinyUSB device stack */
	tusb_rhport_init_t dev_init = {
		.role = TUSB_ROLE_DEVICE,
		.speed = TUSB_SPEED_AUTO
	};
	tusb_rhport_init(0, &dev_init);
}

/*--------------------------------------------------------------------+
 * Device callbacks
 *--------------------------------------------------------------------*/

void tud_mount_cb(void) {
	Status_LED_Set_Mode(STATUS_LED_MODE_ON);
#if LIONKEY_DEVELOPMENT_OVERRIDE == 1
	app_ctap.init_time = ctap_get_current_time();
	app_ctap.pin_boot_remaining_attempts = CTAP_PIN_PER_BOOT_ATTEMPTS;
#endif
}

void tud_umount_cb(void) {
}

void tud_suspend_cb(_Bool remote_wakeup_en) {
	(void) remote_wakeup_en;
	Status_LED_Set_Mode(STATUS_LED_MODE_OFF);
}

void tud_resume_cb(void) {
}

/*--------------------------------------------------------------------+
 * USB HID
 *--------------------------------------------------------------------*/

uint16_t tud_hid_get_report_cb(
	uint8_t itf,
	uint8_t report_id,
	hid_report_type_t report_type,
	uint8_t *buffer,
	uint16_t reqlen
) {
	(void)itf;
	(void)report_id;
	(void)report_type;
	(void)buffer;
	(void)reqlen;
	return 0;
}

void tud_hid_set_report_cb(
	uint8_t itf,
	uint8_t report_id,
	hid_report_type_t report_type,
	const uint8_t *buffer,
	uint16_t bufsize
) {
	(void)itf;
	(void)report_id;
	(void)report_type;

	static_assert(sizeof(ctaphid_packet_t) == CFG_TUD_ENDPOINT0_SIZE, "invalid sizeof(ctaphid_packet_t)");
	if (bufsize != sizeof(ctaphid_packet_t)) {
		return;
	}

	const ctaphid_packet_t *packet = (const ctaphid_packet_t *) buffer;
	app_handle_incoming_hid_packet(packet);
}

void tud_hid_report_complete_cb(uint8_t instance, const uint8_t *report, uint16_t len) {
	(void)instance;
	(void)report;
	(void)len;
}

void tud_hid_report_failed_cb(
	uint8_t instance,
	hid_report_type_t report_type,
	const uint8_t *report,
	uint16_t xferred_bytes
) {
	(void)instance;
	(void)report_type;
	(void)report;
	(void)xferred_bytes;

	if (report_type == HID_REPORT_TYPE_OUTPUT) {
		Error_Handler();
	}
}
