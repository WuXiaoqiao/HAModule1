/*
 * Schalter.cpp
 *
 *  Created on: 26.10.2017
 *      Author: Xiaoqiao
 */
#ifndef _SCHALTER_H_
#define _SCHALTER_H_

#include "Common.h"
#include "Esp.h"
#include <vector>
#include <string>
#include <WiFi.h>
#include "html.h"

enum Status {
	UNBESTIMMT, AN, AUS, OFFEN, ZU
};

class Kanal {
public:
	uint8_t in;
	uint8_t out;
	uint8_t state;
	int64_t lastChangeTime;
	int64_t autoOff;
	int64_t autoOn;
	int retry;
	Kanal(uint8_t in, uint8_t out);
	friend class Schalter;
	void Init();
};

class Schalter {
protected:
	Status objStatus = UNBESTIMMT;
public:
	virtual void CheckIO() = 0;
	virtual void Switch() = 0;
	virtual void Init() = 0;
	static tm GetTime();
	virtual void ganz_zu() {
	}
	virtual void ganz_auf() {
	}
	virtual void an() {
	}
	;
	virtual void aus() {
	}
	;
	virtual void ProcessCommand(std::string cmd, WiFiClient& client)=0;
	virtual void PutOperations(WiFiClient& client, const String& requestHost) {
		for (auto itrStr : operationen) {
			// the content of the HTTP response follows the header:
			client.printf(
					"<button onclick=\"execOperation('http://%s/%d/%s/')\">%s</button>",
					requestHost.c_str(), (int) &(bezeichnung),
					itrStr.first.c_str(), itrStr.second.c_str());
		}
	}
	static int64_t GetNextAutoTime(uint8_t hour, uint8_t min, uint8_t sec);
	virtual bool Putinfo(char* buffer, int Size) {
		return false;
	}
	std::vector<std::pair<std::string, std::string>> operationen;
	std::string bezeichnung;
};

class LichtSchalter: public Schalter {
protected:
	std::vector<LichtSchalter*> verbunden;
public:
	Kanal SchalterKanal;
	LichtSchalter(uint8_t in, uint8_t out, std::string bezeichnung);
	virtual ~LichtSchalter();
	void CheckIO();
	void Switch();
	void Init();
	virtual void an();
	virtual void aus();
	virtual void PutOperations(WiFiClient& client, const String& requestHost);
	void ProcessCommand(std::string cmd, WiFiClient& client);
	void AddVerbunden(LichtSchalter* lichtSchalter);
};

class AutoLichtSchalter: public LichtSchalter {
public:
	AutoLichtSchalter(uint8_t in, uint8_t out, std::string bezeichnung);
	virtual ~AutoLichtSchalter();
	void CheckIO();
	void ProcessCommand(std::string cmd, WiFiClient& client);
};

class RolloSchalter: public Schalter {
public:
	RolloSchalter(uint8_t inAuf, uint8_t outAuf, uint8_t inAb, uint8_t outAb,
			std::string bezeichnung);
	virtual ~RolloSchalter();
	void CheckIO();
	void Switch();
	void Init();
	void ProcessCommand(std::string cmd, WiFiClient& client);
	virtual bool Putinfo(char* buffer, int Size);
//protected:
	Kanal SchalterAuf;
	Kanal SchalterAb;
	int64_t autoCheck;
protected:
	void ganz_auf();
	void ganz_zu();
	void auf(bool autoOff = false);
	void zu(bool autoOff = false);
};

class GlobalSchalter: public Schalter {
public:
	GlobalSchalter();
	virtual ~GlobalSchalter() {
	}
	;
	void CheckIO();
	void Switch();
	void Init();
	void ProcessCommand(std::string cmd, WiFiClient& client);
protected:
	virtual void ganz_auf();
	virtual void ganz_zu();
	virtual void an();
	virtual void aus();
};

class Raum {
public:
	Raum(std::string strBezeichnung) :
			bezeichnung(strBezeichnung) {
	}
	;
	std::string bezeichnung;
	std::vector<Schalter*> vecSchaltern;
	void Init() {
		for (auto itr : vecSchaltern) {
			itr->Init();
		}
	}
	void PutOperations(WiFiClient& client, const String& requestHost){
		client.printf("<p>");
		client.printf("<div>%s</div>", bezeichnung.c_str());
		for (auto itr : vecSchaltern) {
			itr->PutOperations(client, requestHost);
		}
		client.printf("</p>");
	}
};

#endif
