/*
 * task.c
 *
 *  Created on: 2018-02-16 18:00
 *      Author: Jack Chen <redchenjs@live.com>
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tasks/gui_daemon.h"
#include "tasks/mp3_player.h"
#include "tasks/sntp_client.h"
#include "tasks/wifi_daemon.h"
#include "tasks/blufi_daemon.h"
#include "tasks/led_indicator.h"
#include "tasks/nfc_initiator.h"
#include "tasks/token_verifier.h"

void task_init(void)
{
#if defined(CONFIG_ENABLE_GUI)
    xTaskCreate(gui_daemon_task, "gui_daemon_task", 1920, NULL, 5, NULL);
#endif
#if defined(CONFIG_ENABLE_VOICE_PROMPT)
    xTaskCreate(mp3_player_task, "mp3_player_task", 8192, NULL, 5, NULL);
#endif
    xTaskCreate(sntp_client_task, "sntp_client_task", 1920, NULL, 5, NULL);
    xTaskCreate(wifi_daemon_task, "wifi_daemon_task", 1920, NULL, 5, NULL);
#if defined(CONFIG_ENABLE_BLUFI)
    xTaskCreate(blufi_daemon_task, "blufi_daemon_task", 1024, NULL, 5, NULL);
#endif
    xTaskCreate(nfc_initiator_task, "nfc_initiator_task", 5120, NULL, 5, NULL);
#if defined(CONFIG_ENABLE_LED)
    xTaskCreate(led_indicator_task, "led_indicator_task", 1024, NULL, 5, NULL);
#endif
    xTaskCreate(token_verifier_task, "token_verifier_task", 19200, NULL, 5, NULL);
}
