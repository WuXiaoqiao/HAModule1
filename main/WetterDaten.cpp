/*
 * WetterDaten.cpp
 *
 *  Created on: 09.11.2017
 *      Author: Xiaoqiao
 */

#include "WetterDaten.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <Regexp.h>
#include "Schalter.h" //Schalter::GetTime
#include <WiFiClientSecure.h>

#ifndef TAG
#define TAG "WETTERDATEN"
#endif

#define VOR_HALB_SIEBEN 23400
/* use
 openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null
 to get this certificate */
const char* ca_cert = "-----BEGIN CERTIFICATE-----\n"
		"MIIE4DCCA8igAwIBAgIQC1w0NWdbJGfA1zI3+Q1flDANBgkqhkiG9w0BAQsFADBs\n"
		"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
		"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
		"ZSBFViBSb290IENBMB4XDTE0MTExODEyMDAwMFoXDTI0MTExODEyMDAwMFowczEL\n"
		"MAkGA1UEBhMCTkwxFjAUBgNVBAgTDU5vb3JkLUhvbGxhbmQxEjAQBgNVBAcTCUFt\n"
		"c3RlcmRhbTEPMA0GA1UEChMGVEVSRU5BMScwJQYDVQQDEx5URVJFTkEgU1NMIEhp\n"
		"Z2ggQXNzdXJhbmNlIENBIDMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB\n"
		"AQChNsmK4gfxr6c9j2OMBRo3gOA7z5keoaPHiX4rUX+1fF1Brmvf7Uo83sRiXRYQ\n"
		"RJrD79hzJrulDtdihxgS5HgvIQHqGrp3NRRDUlq/4bItLTp9QCHzLhRQSrSYaFkI\n"
		"zztYezwb3ABzNiVciqQFk7WR9ebh9ZaCxaXfebcg7LodgQQ4XDvkW2Aknkb1J8NV\n"
		"nlbKen6PLlNSL4+MLV+uF1e87aTgOxbM9sxZ1/1LRqrOu28z9WA8qUZn2Av+hcP2\n"
		"TQIBoMPMQ8dT+6Yx/0Y+2J702OU//dS0pi8gMe7FtYVcZrlcSy/C40I7EFYHEjTm\n"
		"zH4EGvG6t9wZua2atFKvP/7HAgMBAAGjggF1MIIBcTASBgNVHRMBAf8ECDAGAQH/\n"
		"AgEAMA4GA1UdDwEB/wQEAwIBhjB/BggrBgEFBQcBAQRzMHEwJAYIKwYBBQUHMAGG\n"
		"GGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBJBggrBgEFBQcwAoY9aHR0cDovL2Nh\n"
		"Y2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0SGlnaEFzc3VyYW5jZUVWUm9vdENB\n"
		"LmNydDBLBgNVHR8ERDBCMECgPqA8hjpodHRwOi8vY3JsNC5kaWdpY2VydC5jb20v\n"
		"RGlnaUNlcnRIaWdoQXNzdXJhbmNlRVZSb290Q0EuY3JsMD0GA1UdIAQ2MDQwMgYE\n"
		"VR0gADAqMCgGCCsGAQUFBwIBFhxodHRwczovL3d3dy5kaWdpY2VydC5jb20vQ1BT\n"
		"MB0GA1UdDgQWBBTCuIXX4bkTvdFIvP1e3H2QQnqKqTAfBgNVHSMEGDAWgBSxPsNp\n"
		"A/i/RwHUmCYaCALvY2QrwzANBgkqhkiG9w0BAQsFAAOCAQEAsCq7NTey6NjZHqT4\n"
		"kjZBNU3sItnD+RYAMWx4ZyaELcy7XhndQzX88TYSCYxl/YWB6lCjxx0dL3wTZUbX\n"
		"r+WRDzz5xX+98kdYrwNCT7fmT4eenv6cCS1sC9hc4sIl5dkb1pguY3ViV5D8/yEB\n"
		"hadOpw3TwI8xkqe2j/H5fp4Oaf9cFdpf9C85mQgZJwsvtvmmDTQTPcGPRFTgdGtY\n"
		"2xbWxDah6HjKpX6iI4BTBQhhpX6TJl6/GEaYK07s2Kr8BFPhrgmep9vrepWv61x7\n"
		"dnnqz5SeAs6cbSm551qG7Dj8+6f/8e33oqLC5Ldnbt0Ou6PjtZ4O02dN9cnicemR\n"
		"1B0/YQ==\n"
		"-----END CERTIFICATE-----\n";

/* create an instance of WiFiClientSecure */
WiFiClientSecure client;

void WetterDaten::Check() {
	if (nextCheck < hmMillis()) {
		GetDataWind();
		GetDataSonne();
		nextCheck = hmMillis() + 3600000;
	}
}

// called for each match
boolean WetterDaten::setWindGeschwindigkeit(String& line) // MatchState in use (to get captures)
		{
	unsigned int pos = 0;
	boolean retval = false;
	const char* pattern = "<td class=\"wert \">[A-Za-z, ]+([0-9]+) km/h</td>";
	char cap[100];
	MatchState ms((char*) (line.c_str()));
	if (REGEXP_MATCHED == ms.Match(pattern, pos)) {
		for (byte i = 0; i < ms.level; i++) {
			ms.GetCapture(cap, i);
			ESP_LOGI(TAG, "WindGeschwindigkeit %d = %s", i, cap);
			WindGeschwindigkeit = atoi(cap);
			retval = true;
		}
	}
	pos = ms.MatchStart + ms.MatchLength;
	const char* maxGschw = "<td class=\"wert  text_right\"> ([0-9]+) km/h</td>";

	if (REGEXP_MATCHED == ms.Match(maxGschw, pos)) {
		for (byte i = 0; i < ms.level; i++) {
			ms.GetCapture(cap, i);
			ESP_LOGI(TAG, "WindGeschwindigkeitMax %d = %s", i, cap);
			WindGeschwindigkeitMax = atoi(cap);
			retval = true;
		}
	}
	return retval;
}  // end of match_callback

void WetterDaten::setWindGeschwindigkeitError() {
	WindGeschwindigkeit = 0;
	WindGeschwindigkeitMax = 0;
	ESP_LOGE(TAG, "setWindGeschwindigkeitError");
}

void WetterDaten::setSonnenDatentError() {
	SonnenAufGangSek = 7 * 60 * 60;
	SonnenUnterGangSek = ((18 * 60) + 30) * 60;
	ESP_LOGE(TAG, "setSonnenDatentError");
}

void WetterDaten::setSonnenDaten(String& line) // MatchState in use (to get captures)
		{
	unsigned int pos = 0;

	const char* pattern = "<td class=\"time\">([0-9]+):([0-9]+)</td>";
	char cap[100];
	MatchState ms((char*) (line.c_str()));
	if (REGEXP_MATCHED == ms.Match(pattern, pos)) {
		SonnenAufGangSek = 0;
		for (byte i = 0; i < ms.level; i++) {
			ms.GetCapture(cap, i);
			SonnenAufGangSek = (SonnenAufGangSek + atoi(cap)) * 60;
		}
		if(SonnenAufGangSek < VOR_HALB_SIEBEN)
		{
			SonnenAufGangSek = VOR_HALB_SIEBEN;
		}
	} else {
		setSonnenDatentError();
	}
	ESP_LOGI(TAG, "SonnenAufGangSek: %d", SonnenAufGangSek);
	pos = ms.MatchStart + 10;
	if (REGEXP_MATCHED == ms.Match(pattern, pos)) {
		SonnenUnterGangSek = 0;
		for (byte i = 0; i < ms.level; i++) {
			ms.GetCapture(cap, i);
			SonnenUnterGangSek = (SonnenUnterGangSek + atoi(cap)) * 60;
		}
		lastSonnenUpdate = Schalter::GetTime();
	} else {
		setSonnenDatentError();
	}
	ESP_LOGI(TAG, "SonnenUnterGangSek: %d", SonnenUnterGangSek);
	return;
}  // end of match_callback

void WetterDaten::GetDataWind() {
	/* set SSL/TLS certificate */
	client.setCACert(ca_cert);

	const char* server = "www.zamg.ac.at";

	if (!client.connect(server, 443)) {
		ESP_LOGI(TAG, "connection failed");
		setWindGeschwindigkeitError();
	} else {
		ESP_LOGI(TAG, "Connected to server: %s!", server);
		/* create HTTP request */
		client.println(
				"GET https://www.zamg.ac.at/cms/de/wetter/wetterwerte-analysen/wien HTTP/1.0");
		client.println("Host: www.zamg.ac.at");
		client.println("Connection: close");
		client.println();

		ESP_LOGI(TAG, "Waiting for response ");
		int end = 25;
		while (!client.available() && (end--)) {
			vTaskDelay(50 / portTICK_PERIOD_MS);
			ESP_LOGI(TAG, "feed watchdog");
		}

		String strStammersDorf = "Wien Stammersdorf";
		ESP_LOGI(TAG, "reply was:");
		ESP_LOGI(TAG, "==========");
		int zeile = 1000;
		while (client.available()&&(zeile--)) {
			client.readStringUntil('\n');
		}
		ESP_LOGI(TAG, "%s", "1000 zeilen gelesen");
		while (client.available()) {
			String line = client.readStringUntil('\n');
			//ESP_LOGI(TAG, "%s", line.c_str());
			if (line.indexOf(strStammersDorf) > -1) {
				if (setWindGeschwindigkeit(line)) {
					ESP_LOGI(TAG, "%s: ", line.c_str());
					break;
				}
			}
		}
		while (client.available()) {
			client.read();
		}
		client.stop();
	}
}

void WetterDaten::GetDataSonne() {
	tm now = Schalter::GetTime();
	if(lastSonnenUpdate.tm_year == now.tm_year && lastSonnenUpdate.tm_yday == now.tm_yday){
		ESP_LOGI(TAG, "Sonnendaten bereits bekannt:return.");
		return;
	}
	const char* host = "web-calendar.org";
	ESP_LOGI(TAG, "connecting to %s", host);
	if (!connect(host, 80)) {
		ESP_LOGI(TAG, "connection failed");
		setSonnenDatentError();
		return;
	}

	ESP_LOGI(TAG,
			"requesting URL: /de/world/europe/austria/wien--09?menu=sun HTTP/1.1");

	print("GET /de/world/europe/austria/wien--09?menu=sun HTTP/1.1\r\n");
	print("Host: www.web-calendar.org\r\n");
	print("User-Agent:nix\r\n");
	print("Connection: close\r\n\r\n");

	ESP_LOGI(TAG, "request sent");
	while (connected()) {
		String line = readStringUntil('\n');
		if (line == "\r") {
			ESP_LOGI(TAG, "headers received");
			break;
		}
		vTaskDelay(50 / portTICK_PERIOD_MS);
		ESP_LOGI(TAG, "feed watchdog");
	}
	char bufSearch[100];
	struct tm timeinfo = Schalter::GetTime();
	snprintf(bufSearch,100, "<td class=\"date\">%02d/%02d/%4d</td>",
			timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
	ESP_LOGI(TAG, "search tag:%s", bufSearch);
	ESP_LOGI(TAG, "reply was:");
	while (available()) {
		String line = readStringUntil('\n');
		int srBeginn = line.indexOf(bufSearch);
		if (srBeginn > -1) {
			int srEnd = line.indexOf("</tr>", srBeginn);
			String today = line.substring(srBeginn, srEnd);
			setSonnenDaten(today);
			ESP_LOGI(TAG, "Today: %s", today.c_str());
		}
	}
	stop();
}
