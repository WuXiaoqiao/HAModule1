/*
 * WetterDaten.h
 *
 *  Created on: 09.11.2017
 *      Author: Xiaoqiao
 */

#ifndef MAIN_INCLUDE_WETTERDATEN_H_
#define MAIN_INCLUDE_WETTERDATEN_H_
#include "WiFiClient.h"

class WetterDaten: public WiFiClient {
private:
	int64_t nextCheck;
	boolean setWindGeschwindigkeit(String& line);
	void setWindGeschwindigkeitError();
	void setSonnenDaten(String& line);
	void setSonnenDatentError();
public:
	int WindGeschwindigkeitMax;
	int WindGeschwindigkeit;
	int SonnenAufGangSek;
	int SonnenUnterGangSek;
	tm lastSonnenUpdate;
	WetterDaten() :
		nextCheck(0), WindGeschwindigkeitMax(0), WindGeschwindigkeit(0), SonnenAufGangSek(
					0), SonnenUnterGangSek(0){
		memset(&lastSonnenUpdate, 0, sizeof(lastSonnenUpdate));
		setSonnenDatentError();
	};
	~WetterDaten() {
	};
	void GetDataSonne();
	void GetDataWind();
	void Check();
};

#endif /* MAIN_INCLUDE_WETTERDATEN_H_ */
