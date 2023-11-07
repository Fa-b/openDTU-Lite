// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Modified by Fa-b, 07-11-2023
 */

#include <Arduino.h>

#ifndef privates_h
#define privates_h

/* Change Network credentials and Hoymiles Inverter S/N here 
 * and rename this file to privates.h
 */

// Network credentials
#define     NAME            "openDTU-Lite"
#define     SSID            "ABCDEFG"
#define     WIFI_PASS       "xxxxxxxx"  /* Set password to "" for open networks. */

// Hoymiles Inverter S/N
#define     HOYMILES_INVERTER_SERIAL    0xFFFFFFFFFFFF

/* Settings below here not important */

// Hoymiles Inverter Info
#define     RADIO_DTU_SN                0x0000199978563412  /* Dummy DTU serial number */
#define     HOYMILES_INVERTER_NAME      "Hoymiles"

// NTP Server
#define     NTP_SERVER      "pool.ntp.org"
#define     NTP_TIMEZONE    "CET-1CEST,M3.5.0,M10.5.0/3"
#define     NTP_TIMEZONEDESCR "Europe/Berlin"
#define     NTP_LONGITUDE   10.4515f
#define     NTP_LATITUDE    51.1657f
#define     NTP_SUNSETTYPE  1U

// Don't touch this:
extern char name[];
extern char ssid[];
extern char pass[];
extern uint64_t dtu_serial;

#endif /* privates_h */