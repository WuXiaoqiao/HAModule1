#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "Arduino.h"
#include <WiFi.h>
#include <WiFiType.h>
#include "Esp.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"
#include "time.h"
#include <vector>
#include "Schalter.h"

#define TAG "MODULE1"

#include "Common.h"
#include "html.h"
#include "WetterDaten.h"

#define ARDUINO_RUNNING_CORE 0

#define MINUTE 	(60*1000)
#define STUNDE 	(60*MINUTE)
#define TAGE 	(24*STUNDE)
#define JAHR 	(365*TAGE)

char ssid[] = SSID_MACRO;    // your network SSID (name)

char pass[] = PASS_MACRO; // your network password (use for WPA, or use as key for WEP)

std::vector<Raum*> vecRaum;

WiFiServer server(80);
// connect to UDP returns true if successful or false if not
void ws_server(void *pvParameters);
bool inited = false;
bool TIME_OBTAINED = false;
int64_t lastCheck;
WetterDaten daten;
String requestHost;
String requestHost2;
Schalter * pGlobalSchalter;

void setup() {
	uint64_t chipid = ESP.getEfuseMac();
	ESP_LOGI(TAG, "ESP32 Chip ID = %04X", (uint16_t )(chipid >> 32)); //print High 2 bytes
	ESP_LOGI(TAG, "%08X", (uint32_t )chipid); //print Low 4bytes.
	uint32_t chipidLow = (chipid >> 32) % 65536;
	ESP_LOGI(TAG, "chipidLow %d", chipidLow);
	Raum* raum;

	if ((chipidLow == 15363) || (chipidLow == 54686) || (chipidLow == 31852)) { // module 1
		ESP_LOGI(TAG, "module 1 init");
		raum = new Raum("Zimmer Jayde");
		vecRaum.push_back(raum);
		raum->vecSchaltern.push_back(new LichtSchalter(13, 4, "Jayde"));
		raum->vecSchaltern.push_back(
				new RolloSchalter(14, 17, 12, 16, 600, 2000, "Jayde"));
		raum = new Raum("Arbeitszimmer");
		vecRaum.push_back(raum);
		raum->vecSchaltern.push_back(new LichtSchalter(27, 15, "AZ"));
		raum->vecSchaltern.push_back(
				new RolloSchalter(26, 0, 25, 2, 600, 2000, "AZ"));
		raum = new Raum("Elternzimmer");
		vecRaum.push_back(raum);
		raum->vecSchaltern.push_back(new LichtSchalter(33, 5, "EZ"));
		raum->vecSchaltern.push_back(
				new RolloSchalter(22, 19, 32, 18, 530, 2000, "EZ"));
	}					   //4610366690621131300 247728676

	if ((chipidLow == 20500)) { // module 2
		ESP_LOGI(TAG, "module2 init");
		raum = new Raum("Wohnzimmer");
		vecRaum.push_back(raum);
		raum->vecSchaltern.push_back(new LichtSchalter(13, 15, "Mitte"));
		raum->vecSchaltern.push_back(new LichtSchalter(12, 2, "Fenster"));
		raum->vecSchaltern.push_back(new LichtSchalter(14, 0, "Küche"));
		raum->vecSchaltern.push_back(
				new RolloSchalter(26, 4, 27, 16, 500, 2000, "Fenster"));
		raum->vecSchaltern.push_back(
				new RolloSchalter(25, 17, 33, 5, 500, 2000, "Tür"));
		raum = new Raum("Gang");
		vecRaum.push_back(raum);
		raum->vecSchaltern.push_back(new AutoLichtSchalter(32, 18, "Gang"));
		raum->vecSchaltern.push_back(
				new RolloSchalter(22, 19, 23, 21, 500, 2000, "Gang"));
	}
	if ((chipidLow == 21614)) { // module 3
		ESP_LOGI(TAG, "module3 init");
		raum = new Raum("Haupteingang");
		vecRaum.push_back(raum);
		raum->vecSchaltern.push_back(new TasterSchalter(13, 4, "Tür"));
		raum = new Raum("Wohnung");
		vecRaum.push_back(raum);
		raum->vecSchaltern.push_back(new LichtSchalter(13, 15, "Tür"));
	}
	raum = new Raum("Global");
	pGlobalSchalter = new GlobalSchalter();
	raum->vecSchaltern.push_back(pGlobalSchalter);
	vecRaum.push_back(raum);
	ESP_LOGI(TAG, "First Init ");
	for (auto itr : vecRaum) {
		itr->Init();
	}
	//int readin = digitalRead(23);
	//ESP_LOGI(TAG, "GPIO IN: %d, state: %d\n", 23, readin);
	ESP_LOGI(TAG, "Connecting to ");ESP_LOGI(TAG, "ESP_HWID %llu", ESP.getEfuseMac());
	//WiFi.onEvent(WiFiGotIP, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
	WiFi.begin(ssid, pass);
	/*if ((chipidLow == 60436) || (chipidLow == 20500)) { // module 1
	 connectUDP();
	 }*/
	//LogCrashDump();
	ESP_LOGI(TAG, "Start Server");
	//Create Websocket Server Task
	xTaskCreatePinnedToCore(ws_server, "ws_server", 4096, NULL, 2, NULL, 1);
	lastCheck = hmMillis();
}

void loop() {
	if (((lastCheck + MINUTE) < hmMillis()) || (lastCheck > hmMillis())) {
		if (WiFi.status() != WL_CONNECTED) {
			WiFi.reconnect();
		}
		lastCheck = hmMillis();
	}
	if (WiFi.status() == WL_CONNECTED) {
		if (!TIME_OBTAINED) {
			obtain_time();
			TIME_OBTAINED = true;
			for (auto itr : vecRaum) {
				itr->Init();
			}
		}
		//daten.Check();
	}
	for (auto itrRaum : vecRaum) {
		for (auto itr : itrRaum->vecSchaltern) {
			itr->CheckIO();
		}
	}

	for (auto itrRaum : vecRaum) {
		for (auto itr : itrRaum->vecSchaltern) {
			itr->Switch();
		}
	}

	vTaskDelay(100 / portTICK_PERIOD_MS);
}

void loopTask(void *pvParameters) {
	setup();
	for (;;) {
		micros(); //update overflow
		loop();
	}
}

void PutOperations(WiFiClient& client) {
	for (auto itrRaum : vecRaum) {
		itrRaum->PutOperations(client, requestHost);
	}
}

void PutSettings(WiFiClient& client) {
	for (auto itrRaum : vecRaum) {
		itrRaum->PutSettings(client, requestHost);
	}
}

void PutInfo(WiFiClient& client) {
	client.print(TOSTRING(HTML_INFO));
	client.print("<table>");
	char buffer[500];
	time_t now = hmMillis() / 1000;
	struct tm* ti = gmtime(&now);
	client.print("");
	snprintf(buffer, 500,
			"<tr><th>Time since restart</th><td>%d Tage %02d:%02d:%02d</td>",
			ti->tm_yday, ti->tm_hour, ti->tm_min, ti->tm_sec);
	client.print(buffer);
	client.print("<th>Local Time</th><td>");
	tm tmnow = Schalter::GetTime();
	strftime(buffer, sizeof(buffer), "%X", &tmnow);
	client.print(buffer);
	client.print("</td></tr>");

	for (auto itrRaum : vecRaum) {
		for (auto itr : itrRaum->vecSchaltern) {
			if (itr->Putinfo(buffer, 500)) {
				client.print(buffer);
			}
		}
	}
	client.print("</table>");
}

void decodeUrlComponent(String* value) {
	//ESP_LOGI(TAG, "URL Comp: %s", value->c_str()); // print a message out the serial port
	value->replace("%20", " ");
	value->replace("%C3%BC", "ü");
	/*      value>replace("%21", "!");
	 value>replace("%22", "\"");
	 value>replace("%23", "#");
	 value>replace("%24", "$");
	 value>replace("%25", "%");
	 value>replace("%26", "&");
	 value>replace("%27", "'");
	 value>replace("%28", "(");
	 value>replace("%29", ")");
	 value>replace("%2A", "*");
	 value>replace("%2B", "+");
	 value>replace("%2C", ",");
	 value>replace("%2D", "");
	 value>replace("%2E", ".");
	 value>replace("%2F", "/");
	 value>replace("%3A", ":");
	 value>replace("%3B", ";");
	 value>replace("%3C", "<");
	 value>replace("%3D", "=");
	 value>replace("%3E", ">");
	 value>replace("%3F", "?");
	 value>replace("%40", "@");*/
}

void ws_server(void *pvParameters) {
	//connection references
	server.begin();
	boolean serverStarted = true;
	boolean directIP = true;
	for (;;) {
		if ((WiFi.status() == WL_CONNECTED) && (serverStarted)) {
			requestHost = WiFi.localIP().toString();
			WiFiClient client = server.available(); // listen for incoming clients
			if (client) {
				ESP_LOGI(TAG, "New Client."); // print a message out the serial port
				Schalter* pSchalter = NULL;

				String currentLine = ""; // make a String to hold incoming data from the client
				int64_t start = hmMillis();
				while (client.connected() && ((hmMillis() - start) < 500)) { // loop while the client's connected
					if (client.available()) { // if there's bytes to read from the client,
						char c = client.read();             // read a byte, then
						if (c == '\n') {   // if the byte is a newline character

							// if the current line is blank, you got two newline characters in a row.
							// that's the end of the client HTTP request, so send a response:
							if (currentLine.length() == 0) {
								// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
								// and a content-type so the client knows what's coming, then a blank line:

								ESP_LOGI(TAG, "Write Response");
								client.print(TOSTRING(RESPONSE_HEADER));
								client.print(TOSTRING(HTML_HEAD_TOP));
								char buffer[1024];
								std::string ip1 = "192.168.0.103";
								std::string ip2 = "192.168.0.104";
								std::string ip3 = "192.168.0.105";
								if (!directIP) {
									ip1 = "localhost:103";
									ip2 = "localhost:104";
									ip3 = "localhost:105";
								}

								snprintf(buffer, 1024,
										TOSTRING(HTML_HEAD_CENTER), ip1.c_str(),
										ip2.c_str());

								client.print(buffer);
								client.print(TOSTRING(HTML_HEAD_BOTTOM));
								snprintf(buffer, 1024, TOSTRING(HTML_BODY),
										ip1.c_str(), ip2.c_str(), ip3.c_str(),
										requestHost.c_str());

								client.print(buffer);
								PutInfo(client);
								client.print(TOSTRING(HTML_FOOTER));
								// The HTTP response ends with another blank line:
								client.println();
								client.flush();
								// break out of the while loop:
								break;
							} else {

								if (strncmp("Host: ", currentLine.c_str(), 6)
										== 0) {
									requestHost = currentLine.substring(
											strlen("Host: "));
									if (strncmp(requestHost.c_str(), "192.168",
											7) == 0) {
										directIP = true;
									} else {
										directIP = false;
									}ESP_LOGI(TAG, "URL Host: %s", requestHost.c_str());
								} else {
									requestHost = WiFi.localIP().toString();
								}
								currentLine = "";
							}
						} else if ((c != '/') && (c != '\r')) { // if you got anything else but a carriage return character,
							currentLine += c; // add it to the end of the currentLine
						} else if (c == '/') {
							decodeUrlComponent(&currentLine);
							//ESP_LOGI(TAG, " Process %s", currentLine.c_str());
							if (pSchalter == NULL) {
								if (currentLine == "GET_OPERATIONS") {
									client.print(TOSTRING(RESPONSE_HEADER));
									PutOperations(client);
									client.println();
									client.flush();
									// break out of the while loop:
									break;
								} else if (currentLine == "GET_SETTINGS") {
									client.print(TOSTRING(RESPONSE_HEADER));
									PutSettings(client);
									client.println();
									client.flush();
									// break out of the while loop:
									break;
								} else if (currentLine.length() > 3) {
									//ESP_LOGI(TAG, " currentLine: \"%s\"",currentLine.c_str());
									//ESP_LOGI(TAG, " currentLine: \"%d\"",(int)currentLine.toInt());
									unsigned int currNumber =
											currentLine.toInt();
									for (auto itrRaum : vecRaum) {
										for (auto itr : itrRaum->vecSchaltern) {
											boolean found = strcmp(
													itr->bezeichnung.c_str(),
													currentLine.c_str()) == 0;
											unsigned int bezAddr =
													(unsigned int) &(itr->bezeichnung);
											if ((bezAddr == currNumber)
													|| found) {
												pSchalter = itr;
												ESP_LOGI(TAG, " Found: \"%s\"",
														currentLine.c_str());
												break;
											}
										}
										if (pSchalter != NULL) {
											break;
										}
									}
								}
							} else {
								ESP_LOGI(TAG, " ProcessCommand: \"%s\"",
										currentLine.c_str());
								pSchalter->ProcessCommand(
										std::string(currentLine.c_str()),
										client);
								pSchalter = NULL;
							}
							currentLine = ""; // add it to the end of the currentLine
						}
					}
				}
			}
			// close the connection:
			client.stop();
		} else if ((WiFi.status() == WL_CONNECTED)
				&& (serverStarted == false)) {
			ESP_LOGI(TAG, "WLan Reconnected server restarted");
			server.begin();
			serverStarted = true;
		} else if ((WiFi.status() != WL_CONNECTED) && (serverStarted)) {
			ESP_LOGI(TAG, "WLan disconnected server stoped");
			serverStarted = false;
			server.stop();
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

extern "C" void app_main() {
	initArduino();
	xTaskCreatePinnedToCore(loopTask, "loopTask", 4096, NULL, 1, NULL, 0);
}

