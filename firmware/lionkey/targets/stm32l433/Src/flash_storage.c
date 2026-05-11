#include "ctap_storage.h"
#include "utils.h"
#include "main.h"
#include <string.h>

#define FLASH_STORAGE_PAGE_SIZE 2048
#define FLASH_STORAGE_ADDR      0x0801F000 /* Page 62 (2KB) */
#define FLASH_STORAGE_ADDR_END  0x08020000 /* End of 128KB Flash */

typedef struct LION_ATTR_PACKED flash_storage_item_header {
	uint32_t key;
	uint32_t size;
	uint32_t is_deleted;
	uint32_t padding;
} flash_storage_item_header_t;

static const size_t item_header_size = sizeof(flash_storage_item_header_t);

static uint32_t flash_write_index = 0;

static inline uint32_t ceil_size_to_boundary(const size_t size) {
	return ((size + 7) & (~0x7u)); /* 8-byte aligned for DOUBLEWORD programming on L4 */
}

static inline size_t compute_total_item_size(const size_t size) {
	return item_header_size + ceil_size_to_boundary(size);
}

static void flash_write_doublewords(uint32_t address, const uint64_t *data, size_t length_in_bytes) {
	HAL_FLASH_Unlock();
	size_t num_doublewords = (length_in_bytes + 7) / 8;
	for (size_t i = 0; i < num_doublewords; i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address + (i * 8), data[i]);
	}
	HAL_FLASH_Lock();
}

static ctap_storage_status_t flash_storage_find_item(const ctap_storage_t *const storage, ctap_storage_item_t *const item) {
	size_t index = 0u;
	if (item->handle != 0u) {
		index = (item->handle - 1) + compute_total_item_size(item->size);
	}
	while (index < flash_write_index) {
		const flash_storage_item_header_t *const header = (flash_storage_item_header_t *)(FLASH_STORAGE_ADDR + index);
		if (header->key == 0xFFFFFFFF) {
			break;
		}
		if (header->is_deleted == 0xFFFFFFFF && (item->key == 0u || item->key == header->key)) {
			item->handle = index + 1;
			item->key = header->key;
			item->size = header->size;
			item->data = (const uint8_t *)(FLASH_STORAGE_ADDR + index + item_header_size);
			return CTAP_STORAGE_OK;
		}
		index += compute_total_item_size(header->size);
	}
	return CTAP_STORAGE_ITEM_NOT_FOUND;
}

static ctap_storage_status_t flash_storage_init(const ctap_storage_t *const storage) {
	flash_write_index = 0;
	while (flash_write_index < FLASH_STORAGE_PAGE_SIZE) {
		const flash_storage_item_header_t *header = (flash_storage_item_header_t *)(FLASH_STORAGE_ADDR + flash_write_index);
		if (header->key == 0xFFFFFFFF) {
			break;
		}
		flash_write_index += compute_total_item_size(header->size);
	}
	return CTAP_STORAGE_OK;
}

static ctap_storage_status_t flash_storage_create_item(const ctap_storage_t *const storage, ctap_storage_item_t *const item) {
	const size_t total_item_size = compute_total_item_size(item->size);
	if (flash_write_index + total_item_size > FLASH_STORAGE_PAGE_SIZE) {
		/* Not enough space, would need compaction. Since we only have a tiny amount of items, just return Error for now. */
		return CTAP_STORAGE_OUT_OF_MEMORY_ERROR;
	}

	uint8_t write_buffer[256] = {0xFF};
	memset(write_buffer, 0xFF, sizeof(write_buffer));
	flash_storage_item_header_t *header = (flash_storage_item_header_t *)write_buffer;
	header->key = item->key;
	header->size = item->size;
	header->is_deleted = 0xFFFFFFFF;
	header->padding = 0xFFFFFFFF;

	if (item->size > 0) {
		memcpy(write_buffer + item_header_size, item->data, item->size);
	}

	flash_write_doublewords(FLASH_STORAGE_ADDR + flash_write_index, (const uint64_t *)write_buffer, total_item_size);

	item->handle = flash_write_index + 1;
	item->data = (const uint8_t *)(FLASH_STORAGE_ADDR + flash_write_index + item_header_size);

	flash_write_index += total_item_size;
	return CTAP_STORAGE_OK;
}

static ctap_storage_status_t flash_storage_delete_item(const ctap_storage_t *const storage, const uint32_t item_handle) {
	uint32_t index = item_handle - 1;
	uint32_t addr = FLASH_STORAGE_ADDR + index + offsetof(flash_storage_item_header_t, is_deleted);
	uint64_t deleted_marker = 0x0000000000000000;
	/* Write 0 to is_deleted to mark it as deleted */
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr & ~0x7, 0x0000000000000000); /* Overwrite doubleword to 0 */
	HAL_FLASH_Lock();
	return CTAP_STORAGE_OK;
}

static ctap_storage_status_t flash_storage_create_or_update_item(const ctap_storage_t *const storage, ctap_storage_item_t *const item) {
	if (item->handle != 0u) {
		ctap_storage_item_t new_item = *item;
		if (flash_storage_create_item(storage, &new_item) != CTAP_STORAGE_OK) return CTAP_STORAGE_ERROR;
		if (flash_storage_delete_item(storage, item->handle) != CTAP_STORAGE_OK) return CTAP_STORAGE_ERROR;
		*item = new_item;
		return CTAP_STORAGE_OK;
	}
	return flash_storage_create_item(storage, item);
}

static ctap_storage_status_t flash_storage_increment_counter(const ctap_storage_t *const storage, const uint32_t increment, uint32_t *const counter_new_value) {
	ctap_storage_item_t item = { .handle = 0u, .key = CTAP_STORAGE_KEY_GLOBAL_SIGNATURE_COUNTER };
	uint32_t tmp_counter_value = 0u;
	if (flash_storage_find_item(storage, &item) == CTAP_STORAGE_OK) {
		tmp_counter_value = *((uint32_t *) item.data);
	}
	tmp_counter_value += increment;
	item.size = sizeof(uint32_t);
	item.data = (const uint8_t *)&tmp_counter_value;
	if (flash_storage_create_or_update_item(storage, &item) == CTAP_STORAGE_OK) {
		*counter_new_value = *((uint32_t *) item.data);
		return CTAP_STORAGE_OK;
	}
	return CTAP_STORAGE_ERROR;
}

static size_t flash_storage_estimate_num_remaining_items(const ctap_storage_t *const storage, const ctap_storage_item_t *const item) {
	return (FLASH_STORAGE_PAGE_SIZE - flash_write_index) / compute_total_item_size(item->size);
}

static ctap_storage_status_t flash_storage_erase(const ctap_storage_t *const storage) {
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks = FLASH_BANK_1;
	EraseInitStruct.Page = 62; /* 0x0801F800 is Page 63, 0x0801F000 is 62 */
	EraseInitStruct.NbPages = 1;
	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	HAL_FLASH_Lock();
	flash_write_index = 0;
	return CTAP_STORAGE_OK;
}

const ctap_storage_t app_storage = {
	.init = flash_storage_init,
	.find_item = flash_storage_find_item,
	.create_or_update_item = flash_storage_create_or_update_item,
	.delete_item = flash_storage_delete_item,
	.increment_counter = flash_storage_increment_counter,
	.estimate_num_remaining_items = flash_storage_estimate_num_remaining_items,
	.erase = flash_storage_erase,
	.context = NULL
};
