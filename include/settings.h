// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Modified by Fa-b, 07-11-2023
 */

// NRF24L01+ Settings
#define NRF_PA_LEVEL        RF24_PA_LOW         // options: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX

// CMT2300A Settings
#define CMT_FREQUENCY       865000              // in kHz
#define CMT_PA_LEVEL        10                  // -10 .. 20 dBm

// Data update interval
#define UPDATE_INTERVAL     5000                // in milliseconds