/**
 * @file    usb_descriptors.c
 * @brief   USB descriptors for Knox Key — STM32F072
 */

#include "tusb.h"
#include "stm32f0xx.h"
#include <assert.h>
#include <string.h>

/*--------------------------------------------------------------------+
 * Device Descriptor
 *--------------------------------------------------------------------*/

static const tusb_desc_device_t device_descriptor = {
	.bLength            = sizeof(tusb_desc_device_t),
	.bDescriptorType    = TUSB_DESC_DEVICE,
	.bcdUSB             = 0x0200,
	.bDeviceClass       = 0x00,
	.bDeviceSubClass    = 0x00,
	.bDeviceProtocol    = 0x00,
	.bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

	.idVendor  = 0x1209, /* pid.codes test VID */
	.idProduct = 0x0001, /* TODO: get a real PID from pid.codes */
	.bcdDevice = 0x0100,

	.iManufacturer = 0x01,
	.iProduct      = 0x02,
	.iSerialNumber = 0x03,

	.bNumConfigurations = 0x01
};

const uint8_t *tud_descriptor_device_cb(void) {
	return (const uint8_t *) &device_descriptor;
}

/*--------------------------------------------------------------------+
 * HID Report Descriptor
 *--------------------------------------------------------------------*/

static const uint8_t hid_report_descriptor[] = {
	TUD_HID_REPORT_DESC_FIDO_U2F(CFG_TUD_HID_EP_BUFSIZE)
};

const uint8_t *tud_hid_descriptor_report_cb(uint8_t itf) {
	(void) itf;
	return hid_report_descriptor;
}

/*--------------------------------------------------------------------+
 * Configuration Descriptor
 *--------------------------------------------------------------------*/

enum endpoint_number {
	HID_INTERFACE_NUMBER,
	ITF_NUM_TOTAL
};
#define CONFIGURATION_DESCRIPTOR_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN)
#define ENDPOINT_NUM_HID   0x01

static const uint8_t configuration_descriptor[] = {
	TUD_CONFIG_DESCRIPTOR(
		1,
		ITF_NUM_TOTAL,
		0,
		CONFIGURATION_DESCRIPTOR_TOTAL_LEN,
		0x00,
		100
	),
	TUD_HID_INOUT_DESCRIPTOR(
		HID_INTERFACE_NUMBER,
		0,
		HID_ITF_PROTOCOL_NONE,
		sizeof(hid_report_descriptor),
		ENDPOINT_NUM_HID,
		0x80 | ENDPOINT_NUM_HID,
		CFG_TUD_HID_EP_BUFSIZE,
		5
	)
};

TU_VERIFY_STATIC(
	sizeof(configuration_descriptor) == CONFIGURATION_DESCRIPTOR_TOTAL_LEN,
	"invalid CONFIGURATION_DESCRIPTOR_TOTAL_LEN"
);

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
	(void) index;
	return configuration_descriptor;
}

/*--------------------------------------------------------------------+
 * String Descriptors
 *--------------------------------------------------------------------*/

enum string_descriptor_index {
	STRID_LANGID = 0,
	STRID_MANUFACTURER,
	STRID_PRODUCT,
	STRID_SERIAL,
	STRID_NUM_TOTAL,
};

static const char *string_desc_arr[] = {
	(const char[]) {0x09, 0x04}, /* 0: English (0x0409) */
	"Knox Key",                  /* 1: Manufacturer */
	"Knox Key FIDO2",            /* 2: Product */
	NULL,                        /* 3: Serial — uses unique ID */
};

static_assert(
	sizeof(string_desc_arr) / sizeof(string_desc_arr[0]) == STRID_NUM_TOTAL,
	"invalid STRID_NUM_TOTAL, fix string_desc_arr"
);

#define STRING_DESCRIPTOR_MAX_NUM_CHARS 32
static uint16_t string_descriptor_buffer[1 + STRING_DESCRIPTOR_MAX_NUM_CHARS];

static size_t board_get_unique_id(uint8_t id[], size_t max_len) {
	(void) max_len;
	const size_t len = 12;
	assert(max_len >= len);

	uint32_t *id32 = (uint32_t *) id;
	volatile uint32_t *stm32_uuid = (volatile uint32_t *) UID_BASE;
	id32[0] = stm32_uuid[0];
	id32[1] = stm32_uuid[1];
	id32[2] = stm32_uuid[2];

	return len;
}

static size_t board_usb_get_serial(uint16_t desc_str1[], size_t max_chars) {

	uint8_t uid[12] TU_ATTR_ALIGNED(4);
	size_t uid_len = board_get_unique_id(uid, sizeof(uid));

	if (uid_len > max_chars / 2) {
		uid_len = max_chars / 2;
	}

	for (size_t i = 0; i < uid_len; i++) {
		for (size_t j = 0; j < 2; j++) {
			const char nibble_to_hex[16] = {
				'0', '1', '2', '3', '4', '5', '6', '7',
				'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
			};
			const uint8_t nibble = (uid[i] >> (j * 4)) & 0xf;
			desc_str1[i * 2 + (1 - j)] = nibble_to_hex[nibble];
		}
	}

	return 2 * uid_len;
}

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
	(void) langid;

	uint16_t *const string_descriptor_buffer_chars = &string_descriptor_buffer[1];
	size_t num_chars;

	switch (index) {
		case STRID_LANGID:
			memcpy(string_descriptor_buffer_chars, string_desc_arr[0], 2);
			num_chars = 1;
			break;

		case STRID_SERIAL:
			num_chars = board_usb_get_serial(
				string_descriptor_buffer_chars,
				STRING_DESCRIPTOR_MAX_NUM_CHARS
			);
			break;

		default:
			if (index >= STRID_NUM_TOTAL) {
				return NULL;
			}

			const char *str = string_desc_arr[index];
			num_chars = strlen(str);
			if (num_chars > STRING_DESCRIPTOR_MAX_NUM_CHARS) {
				num_chars = STRING_DESCRIPTOR_MAX_NUM_CHARS;
			}

			for (size_t i = 0; i < num_chars; i++) {
				string_descriptor_buffer[1 + i] = str[i];
			}
			break;
	}

	assert(num_chars > 0);

	string_descriptor_buffer[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * num_chars + 2));

	return string_descriptor_buffer;
}
