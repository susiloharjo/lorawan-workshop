/*
 * sl868a.cpp
 *
 * Created: 05/05/2015 22:15:38
 * Author : Seve (seve@axelelettronica.it)
 */
#include <Uart.h>
#include <Arduino.h>
 
#include "sl868a.h"
#include "sl868aModel.h"
#include "sl868aParser.h"
#include "smeErrorCode.h"


static bool cold_boot = true;

void
Sl868a::setStandby(void)
{
     // set GPS in standby
     print(SL868A_SET_STDBY_CMD);
     _ready = false;
}

 void
 Sl868a::setWarmRestart(void)
 {
      // Power On the GPS module
     print(SL868A_WARM_RST_CMD);
     _ready = false;
 }

void
Sl868a::setHotRestart(void)
{
     // Hot Power On the GPS module
     print(SL868A_HOT_RST_CMD);
     _ready = false;
}

 void
 Sl868a::setColdRestart(void)
 {
      // Cold Power On the GPS module
     print(SL868A_COLD_RST_CMD);
     _ready = false;
     cold_boot = true;
 }
 
 void
 Sl868a::processGpsRxMsg(void)
 {
     sl868a_nmea_out_msg_id nmea_msg_id;

     if (cold_boot) {
         cold_boot = false;
     }

     if (msgPtrT.messageType == STD_NMEA) {
         nmea_msg_id = sl868aIdentifyNmeaRxMsg(msgPtrT.nmea_p.std_p.talker_p,
                                               msgPtrT.nmea_p.std_p.sentenceId_p);
         switch(nmea_msg_id) {
             case NMEA_RMC:
             sme_parse_coord(msgPtrT.nmea_p.std_p.data_p, msgPtrT.nmea_p.std_p.dataLenght, SME_LAT);
             sme_parse_coord(msgPtrT.nmea_p.std_p.data_p, msgPtrT.nmea_p.std_p.dataLenght, SME_LONG);
			 sl868a_parse_rmc(msgPtrT.nmea_p.std_p.data_p, msgPtrT.nmea_p.std_p.dataLenght);
			              
             break;

             case NMEA_GGA:
             sl868a_parse_gga(msgPtrT.nmea_p.std_p.data_p, msgPtrT.nmea_p.std_p.dataLenght);
             break;

             case NMEA_UNMANAGED:
             default:
             break;
         }
     }
 }



 uint8_t
 Sl868a::handleGpsRxData(uint8_t inChar)
 {
     if (rxMsg.idx < (SL868A_MAX_MSG_LEN-2)) {

         if ((inChar < 21) && !((inChar == '\n') || (inChar == '\r')))  {
             return SME_OK;
             }  else if (inChar == '$') {
             memset(&rxMsg, 0, sizeof(rxMsg));
         }

         rxMsg.data[rxMsg.idx++] = inChar;

         if (inChar == '\n')  {
             if (rxMsg.idx > 3) {
                 rxMsg.data[rxMsg.idx] = '\0';
                 if (crcCheck(rxMsg.data, rxMsg.idx)) {
                    parseGpsRxMsg();
                    processGpsRxMsg();
	            if (!_fixing) { 
	                _fixing = true;
                    }		    
                 }
             }
         }
         } else {
         memset(&rxMsg, 0, sizeof(rxMsg));
     }

     return SME_OK;
 }


 int
 Sl868a::print(const char *msg)
 {
    gpsComm->print((const char*)msg);
    return SME_OK;
 }


 void
 Sl868a::readData(void)
 {
     while (gpsComm->available()) {
         // get the new byte:
         char inChar = (char)gpsComm->read();
         handleGpsRxData(inChar);
     }
 }

 /****************************************************************************/
 /*                               Public APIs                                */
 /****************************************************************************/

  const bool
  Sl868a::ready(void)
  {
     readData();
     return _ready;
  }

 Sl868a::Sl868a(void){
     _ready  = false;
     _fixing = false;
 }

 void Sl868a::begin (Uart *serial) {
     this->gpsComm = serial;
     gpsComm->begin(9600);
 }


int
Sl868a::getLatitudeDegrees()
{
    readData();
    return (isNorthLatitude() ? _data.lat_deg : -_data.lat_deg);
}

unsigned long
Sl868a::getLatitudeDecimals()
{
    readData();
    return (unsigned long) _data.lat_decimals;
}

double
Sl868a::getLatitude()
{
    readData();

    double lat = _data.lat_decimals;

    lat = (lat/LAT_LONG_DEC_UNIT);
    lat += _data.lat_deg;
    if (!isNorthLatitude()) {
       lat = -lat;
    }
    return lat;
}

bool
Sl868a::isNorthLatitude()
{
    readData();
    return (bool) _data.lat_direction;
}

int
Sl868a::getLongitudeDegrees()
{
    readData();
    return (isEastLongitude() ? _data.longit_deg : -_data.longit_deg);
}

unsigned long
Sl868a::getLongitudeDecimals()
{
    readData();
    return (unsigned long) _data.longit_decimals;
}

bool
Sl868a::isEastLongitude()
{
    readData();
    return (bool) _data.longit_direction;
}

double
Sl868a::getLongitude()
{
    readData();

    double longit = _data.longit_decimals;

    longit = (longit/LAT_LONG_DEC_UNIT);
    longit += _data.longit_deg;

    if (!isEastLongitude()) {
        longit = -longit;
    }
    return longit;
}

unsigned int
Sl868a::getAltitude()
{
    readData();
    return _data.altitude;
}

unsigned char
Sl868a::getLockedSatellites()
{
    readData();
    return _data.n_satellites;
}

// additional getters UTC speed here
unsigned int Sl868a::getUtcHour(){
	readData();
	return _data.utc_hour;
}
unsigned int Sl868a::getUtcMinute(){
	readData();
	return _data.utc_min;
}
unsigned int Sl868a::getUtcSecond(){
	readData();
	return _data.utc_sec;
}

unsigned int Sl868a::getUtcSecondDecimals(){
	readData();
	return _data.utc_sec_decimals;
}

unsigned int Sl868a::getUtcYear(){
	readData();
	return _data.utc_year;
}
unsigned char Sl868a::getUtcMonth(){
	readData();
	return _data.utc_month;
}
unsigned char Sl868a::getUtcDayOfMonth(){
	readData();
	return _data.utc_dayOfMonth;
}
double Sl868a::getSpeedKnots(){
	readData();
	return _data.speed_knots >0 ? _data.speed_knots :0;
}
double Sl868a::getCourse(){
	readData();
	return _data.course >0 ? _data.course:0;
}
sl868aCachedDataT Sl868a::getData(){
	readData();
	return _data;
}
Sl868a  smeGps;
