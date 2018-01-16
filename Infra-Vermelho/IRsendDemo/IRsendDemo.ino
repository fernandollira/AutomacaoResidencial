/* IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend.
 *
 * Version 1.0 April, 2017
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009,
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 *
 * An IR LED circuit *MUST* be connected to ESP8266 pin 4 (D2).
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/markszabo/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>

IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)

// Example of data captured by IRrecvDumpV2.ino
uint16_t rawData[93] = {1318, 384,  1302, 382,  460, 1224,  
1296, 388,  1302, 382,  456, 1228,  462, 1222,  458, 1248,  
430, 1232,  454, 1230,  460, 1224,  1304, 7102,  1314, 388,  
1304, 382,  460, 1224,  1298, 388,  1296, 388,  456, 1228,  462, 
1222,  460, 1248,  430, 1232,  454, 1228,  456, 1230,  1302, 7104,  
1316, 386,  1294, 392,  450, 1234,  1298, 384,  1302, 382,  456, 1228,  
460, 1224,  452, 1232,  456, 1228,  454, 1252,  434, 1228,  1296, 7106,  
1320, 384,  1300, 384,  462, 1244,  1280, 382,  1302, 382,  458, 1228,  
456, 1228,  452, 1232,  458, 1226,  452, 1256,  434};

void setup() {
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
}

void loop() {
  //Serial.println("NEC");
  //irsend.sendNEC(0x00FFE01FUL, 32);
  //delay(2000);
 // Serial.println("Sony");
 // irsend.sendSony(0xa90, 12, 2);
 // delay(2000);
  Serial.println("a rawData capture from IRrecvDumpV2");
  irsend.sendRaw(rawData, 67, 38);  // Send a raw data capture at 38kHz.
  delay(2000);
}
