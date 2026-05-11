#ifndef KNOXKEY_stm32l433_APP_H
#define KNOXKEY_stm32l433_APP_H

#include "ctaphid.h"
#include "ctap.h"

extern const ctap_storage_t app_storage;
extern const ctap_crypto_t app_sw_crypto;

extern ctaphid_state_t app_ctaphid;
extern ctap_state_t app_ctap;

void app_run(void);

void app_hid_task(void);

void app_handle_incoming_hid_packet(const ctaphid_packet_t *packet);

void app_hid_report_send_queue_add(const ctaphid_packet_t *packet, _Bool fail_if_full);

void app_hid_report_send_queue_send_one_if_possible(void);

_Bool app_hid_report_send_queue_is_empty(void);

void app_debug_task(void);

#endif // KNOXKEY_stm32l433_APP_H
