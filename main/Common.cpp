/*
 * Common.cpp
 *
 *  Created on: 25.10.2017
 *      Author: Xiaoqiao
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <WiFi.h>
#include "Esp.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "WiFiUdp.h"
#include "Common.h"
#include "apps/sntp/sntp.h"
#include "esp_timer.h"
#include "esp_spi_flash.h"
#include "base64.h"

#include <string.h>
#include <time.h>



#ifndef TAG
#define TAG "MODULE1"
#endif

WiFiUDP myUDP;

static char buf[UDP_LOGGING_MAX_PAYLOAD_LEN];
boolean udpConnected = false;

static int udp_logging_vprintf(const char *format, va_list args) {
	if (udpConnected) {
		vsnprintf((char*) buf, UDP_LOGGING_MAX_PAYLOAD_LEN, format, args);
		myUDP.beginPacket("192.168.0.100", 51818);
		myUDP.write((uint8_t*) TAG, strlen(TAG));
		myUDP.write((uint8_t*) ":", 1);
		myUDP.write((uint8_t*) buf, strlen(buf));
		myUDP.endPacket();
	}
	return vprintf(format, args);
}

void LogCrashDump(){
	const int dumpsize = 65536;
	uint8_t* crashDump = new uint8_t[dumpsize];
	spi_flash_read(0x11000,crashDump,dumpsize);
	ESP_LOGI(TAG,"crashdump begin");
	int index = 0;
	uint8_t *ptr = crashDump;
	while(index < 64){
		String base64CrashDump = base64::encode(ptr, 1024);
		ptr += 1024;
		ESP_LOGI("DUMP","%d:%s",index,base64CrashDump.c_str());
		index++;
		vTaskDelay(10);//Feed the Watchdog
	}
	ESP_LOGI(TAG,"crashdump end");
	delete crashDump;
}

void connectUDP() {
	ESP_LOGI(TAG,"Connecting to UDP");

	if (myUDP.begin(8888) == 1) {
		ESP_LOGI(TAG,"Connection successful");
		udpConnected = true;
	} else {
		ESP_LOGI(TAG,"Connection failed");
	}
	if (udpConnected) {
		esp_log_set_vprintf(udp_logging_vprintf);
	}
}

void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, (char *)"at.pool.ntp.org");
    sntp_init();
}

void obtain_time()
{
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo;
    memset(&timeinfo, 0, sizeof(timeinfo));
    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
	char strftime_buf[64];
    // Set timezone to Eastern Standard Time and print local time

    //setenv("TZ", "UTC-1:00", 1);
	setenv("TZ", "UTC-1DST-2", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in Austria: %s", strftime_buf);
}


int64_t hmMillis(){
	return (esp_timer_get_time()/1000);
}
