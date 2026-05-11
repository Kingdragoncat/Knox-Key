/**
 * @file    app.c
 * @brief   Knox Key application logic — stm32l433 target
 *
 * Adapted from LionKey's stm32h533 app.c.
 * Uses software crypto (micro-ecc, tiny-AES-c, sha256) since the stm32l433
 * has no hardware crypto accelerators.
 * Uses memory-based storage (RAM) as the initial implementation;
 * flash-based storage for the F072 will be added in a future iteration.
 */

#include "app.h"
#include "usb.h"
#include "main.h"
#include "utils.h"
#include <stdbool.h>
#include "ctap_crypto_atecc.h"
extern const ctap_storage_t app_storage;

/* --- Hardware Crypto --- */
static ctap_atecc_crypto_context_t app_crypto_ctx;
const ctap_crypto_t app_sw_crypto = CTAP_CRYPTO_ATECC_CONST_INIT(&app_crypto_ctx);

/* --- CTAPHID state --- */
ctaphid_state_t app_ctaphid;
static const uint8_t app_ctaphid_capabilities =
	CTAPHID_CAPABILITY_WINK | CTAPHID_CAPABILITY_CBOR | CTAPHID_CAPABILITY_NMSG;
static const uint8_t app_ctaphid_version_major = 1;
static const uint8_t app_ctaphid_version_minor = 0;
static const uint8_t app_ctaphid_version_build = 0;

static uint8_t app_ctaphid_cbor_response_buffer[CTAPHID_MAX_PAYLOAD_LENGTH];
ctap_response_t app_ctap_response = {
	.data_max_size = sizeof(app_ctaphid_cbor_response_buffer) - 1,
	.data = &app_ctaphid_cbor_response_buffer[1]
};
ctap_state_t app_ctap = CTAP_STATE_CONST_INIT(&app_sw_crypto, &app_storage);

static ctap_keepalive_status_t app_ctap_last_status = CTAP_STATUS_PROCESSING;
static uint32_t app_ctap_last_status_message_timestamp = 0;

static inline void app_ctap_reset_keepalive(void) {
	app_ctap_last_status = CTAP_STATUS_PROCESSING;
	app_ctap_last_status_message_timestamp = 0;
}

static void app_ctaphid_send_keepalive(ctap_keepalive_status_t status) {
	assert(ctaphid_has_complete_message_ready(&app_ctaphid));
	const ctaphid_channel_buffer_t *message = &app_ctaphid.buffer;
	ctaphid_packet_t res;
	ctaphid_create_init_packet(&res, message->cid, CTAPHID_KEEPALIVE, 1);
	res.pkt.init.payload[0] = status;
	app_hid_report_send_queue_add(&res, false);
	app_ctap_last_status_message_timestamp = HAL_GetTick();
}

static void app_ctap_send_keepalive_if_needed(ctap_keepalive_status_t current_status) {
	if (current_status != app_ctap_last_status) {
		app_ctap_last_status = current_status;
		app_ctaphid_send_keepalive(current_status);
		return;
	}
	uint32_t elapsed_since_last_keepalive = HAL_GetTick() - app_ctap_last_status_message_timestamp;
	if (elapsed_since_last_keepalive > 80) {
		app_ctaphid_send_keepalive(current_status);
	}
}

void app_debug_task(void) {
	/* No debug UART on Knox Key minimal board — stub */
}

/* This function might be invoked anytime by the CTAP layer while in ctap_request(). */
void ctap_send_keepalive_if_needed(ctap_keepalive_status_t current_status) {
	app_ctap_send_keepalive_if_needed(current_status);
	app_hid_task();
	app_debug_task();
}

/* This function might be invoked anytime by the CTAP layer while in ctap_request(). */
ctap_user_presence_result_t ctap_wait_for_user_presence(void) {

	info_log("waiting for user presence ..." nl);
	app_ctap_send_keepalive_if_needed(CTAP_STATUS_UPNEEDED);

	Status_LED_Set_Mode(STATUS_LED_MODE_BLINKING_NORMAL);

	const uint32_t timeout_ms = 30 * 1000; /* 30 seconds */
	uint32_t start_timestamp = HAL_GetTick();

	if (CurrentButtonEvent != BUTTON_EVENT_NONE) {
		CurrentButtonEvent = BUTTON_EVENT_NONE;
	}

	while (true) {

		app_hid_task();
		app_debug_task();

		if (app_ctaphid.buffer.cancel) {
			Status_LED_Set_Mode(STATUS_LED_MODE_ERROR);
			return CTAP_UP_RESULT_CANCEL;
		}

		uint32_t elapsed_ms = HAL_GetTick() - start_timestamp;
		if (elapsed_ms > timeout_ms) {
			Status_LED_Set_Mode(STATUS_LED_MODE_ERROR);
			return CTAP_UP_RESULT_TIMEOUT;
		}

#ifdef LIONKEY_DEVELOPMENT_AUTO_USER_PRESENCE
		if (elapsed_ms > LIONKEY_DEVELOPMENT_AUTO_USER_PRESENCE) {
            Status_LED_Set_Mode(STATUS_LED_MODE_SUCCESS);
			return CTAP_UP_RESULT_ALLOW;
		}
#endif

		if (CurrentButtonEvent == BUTTON_EVENT_SINGLE_CLICK || CurrentButtonEvent == BUTTON_EVENT_DOUBLE_CLICK || CurrentButtonEvent == BUTTON_EVENT_LONG_PRESS) {
			CurrentButtonEvent = BUTTON_EVENT_NONE;
			Status_LED_Set_Mode(STATUS_LED_MODE_SUCCESS);
			return CTAP_UP_RESULT_ALLOW;
		}

		app_ctap_send_keepalive_if_needed(CTAP_STATUS_UPNEEDED);
	}
}

uint32_t ctap_get_current_time(void) {
	return HAL_GetTick();
}

static void handle_packet_using_send_or_queue_ctaphid_packet(const ctaphid_packet_t *packet, void *ctx) {
	(void)ctx;
	app_hid_report_send_queue_add(packet, true);
}

static void app_init(void) {

	Status_LED_Set_Mode(STATUS_LED_MODE_ON);

	const uint32_t t1 = HAL_GetTick();
	ctaphid_init(&app_ctaphid);
	if (app_storage.init(&app_storage) != CTAP_STORAGE_OK) {
		Error_Handler();
	}
	/* Initialize software crypto with a seed derived from SysTick + UID */
	uint32_t seed = HAL_GetTick();
	/* Mix in the unique device ID for better entropy */
	volatile uint32_t *uid = (volatile uint32_t *)UID_BASE;
	seed ^= uid[0] ^ uid[1] ^ uid[2];
	if (app_sw_crypto.init(&app_sw_crypto, seed) != CTAP_CRYPTO_OK) {
		Error_Handler();
	}
	ctap_init(&app_ctap);
	const uint32_t t2 = HAL_GetTick();

	info_log("init done in %" PRId32 " ms" nl, t2 - t1);
}

static inline void app_wait_for_empty_hid_send_queue(void) {
	while (!app_hid_report_send_queue_is_empty()) {
		app_hid_task();
		app_debug_task();
	}
}

static inline void app_ctaphid_task(void) {

	if (!ctaphid_has_complete_message_ready(&app_ctaphid)) {
		return;
	}

	const ctaphid_channel_buffer_t *message = &app_ctaphid.buffer;
	const uint8_t cmd = message->cmd;

	debug_log(
		nl nl "app_run: " green("ctaphid message ready")
		" cid=0x%08" PRIx32
		" cmd=0x%02" wPRIx8
		" payload_length=%" PRIu16
		nl,
		message->cid, ctaphid_get_cmd_number_per_spec(cmd), message->payload_length
	);

	app_wait_for_empty_hid_send_queue();

	ctaphid_packet_t res;

	if (cmd == CTAPHID_PING) {
		ctaphid_message_to_packets(
			message->cid,
			CTAPHID_PING,
			message->payload_length,
			message->payload,
			handle_packet_using_send_or_queue_ctaphid_packet,
			NULL
		);
		ctaphid_reset_to_idle(&app_ctaphid);
		return;
	}

	if (cmd == CTAPHID_WINK) {
		if (message->payload_length != 0) {
			ctaphid_create_error_packet(&res, message->cid, CTAP1_ERR_INVALID_LENGTH);
			app_hid_report_send_queue_add(&res, true);
			ctaphid_reset_to_idle(&app_ctaphid);
			return;
		}
		/* Blink LED briefly as visual identification */
		Status_LED_Set_Mode(STATUS_LED_MODE_BLINKING_SPECIAL);
		ctaphid_create_init_packet(&res, message->cid, CTAPHID_WINK, 0);
		app_hid_report_send_queue_add(&res, true);
		ctaphid_reset_to_idle(&app_ctaphid);
		return;
	}

	if (cmd == CTAPHID_CBOR) {
		if (message->payload_length == 0) {
			ctaphid_create_error_packet(&res, message->cid, CTAP1_ERR_INVALID_LENGTH);
			app_hid_report_send_queue_add(&res, true);
			ctaphid_reset_to_idle(&app_ctaphid);
			return;
		}
		assert(message->payload_length >= 1);
		app_ctap_reset_keepalive();
		app_ctaphid_cbor_response_buffer[0] = ctap_request(
			&app_ctap,
			message->payload[0],
			message->payload_length - 1,
			&message->payload[1],
			&app_ctap_response
		);
		app_wait_for_empty_hid_send_queue();
		ctaphid_message_to_packets(
			message->cid,
			CTAPHID_CBOR,
			1 + app_ctap_response.length,
			app_ctaphid_cbor_response_buffer,
			handle_packet_using_send_or_queue_ctaphid_packet,
			NULL
		);
		ctaphid_reset_to_idle(&app_ctaphid);
		return;
	}

	error_log(
		red("unsupported ctaphid command 0x%02" wPRIx8) nl,
		ctaphid_get_cmd_number_per_spec(cmd)
	);
	ctaphid_create_error_packet(&res, message->cid, CTAP1_ERR_INVALID_COMMAND);
	app_hid_report_send_queue_add(&res, true);
	ctaphid_reset_to_idle(&app_ctaphid);
}

void app_run(void) {

	info_log(nl nl "Knox Key app_run" nl);

	app_init();

	usb_init();

	while (true) {

		app_hid_task();

		app_debug_task();

		app_ctaphid_task();
	}
}

void app_handle_incoming_hid_packet(const ctaphid_packet_t *packet) {

	uint8_t error_code;
	ctaphid_packet_t res;
	uint32_t new_channel_id;

	const uint32_t current_time = HAL_GetTick();

	if (ctaphid_has_incomplete_message_timeout(&app_ctaphid, current_time)) {
		ctaphid_create_error_packet(&res, app_ctaphid.buffer.cid, CTAP1_ERR_TIMEOUT);
		ctaphid_reset_to_idle(&app_ctaphid);
	}

	ctaphid_process_packet_result_t result = ctaphid_process_packet(
		&app_ctaphid,
		packet,
		current_time,
		&error_code
	);

	switch (result) {

		case CTAPHID_RESULT_ERROR:
			ctaphid_create_error_packet(&res, packet->cid, error_code);
			app_hid_report_send_queue_add(&res, false);
			break;

		case CTAPHID_RESULT_ALLOCATE_CHANNEL:
			new_channel_id = ctaphid_allocate_channel(&app_ctaphid);
			if (new_channel_id == 0) {
				ctaphid_create_error_packet(&res, packet->cid, error_code);
				app_hid_report_send_queue_add(&res, false);
				break;
			}
			ctaphid_create_ctaphid_init_response_packet(
				&res,
				packet->pkt.init.payload,
				CTAPHID_BROADCAST_CID,
				app_ctaphid.highest_allocated_cid,
				app_ctaphid_version_major,
				app_ctaphid_version_minor,
				app_ctaphid_version_build,
				app_ctaphid_capabilities
			);
			app_hid_report_send_queue_add(&res, false);
			break;

		case CTAPHID_RESULT_DISCARD_INCOMPLETE_MESSAGE:
			ctaphid_create_ctaphid_init_response_packet(
				&res,
				packet->pkt.init.payload,
				packet->cid,
				app_ctaphid.highest_allocated_cid,
				app_ctaphid_version_major,
				app_ctaphid_version_minor,
				app_ctaphid_version_build,
				app_ctaphid_capabilities
			);
			app_hid_report_send_queue_add(&res, false);
			break;

		case CTAPHID_RESULT_IGNORED:
		case CTAPHID_RESULT_BUFFERING:
		case CTAPHID_RESULT_CANCEL:
		case CTAPHID_RESULT_MESSAGE:
			break;
	}
}
