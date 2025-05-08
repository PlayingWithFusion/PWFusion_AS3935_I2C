/***************************************************************************
* File Name: as3935_lightning_i2c_nocal.ino
* Processor/Platform: Arduino Uno R3 (tested), R3aktor M0 Logger (tested)
* Development Environment: Arduino 1.6.1
*
* Designed for use with with Playing With Fusion AS3935 Lightning Sensor
* Breakout: SEN-39001-R01. Demo shows how this lightning sensor can be brought 
* into an Arduino project without a bunch of calibration needed. This is
* because each board is tested calibrated prior to being shipped, and the 
* cal value is written on the packaging.
*
*   SEN-39001-R01 (universal applications)
*   ---> https://www.playingwithfusion.com/productview.php?pdid=135
*
* Copyright © 2015 Playing With Fusion, Inc.
* SOFTWARE LICENSE AGREEMENT: This code is released under the MIT License.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* **************************************************************************
* REVISION HISTORY:
* Author		      Date		        Comments
* N. Johnson      2025May08       Updated i2c example to work with qwiic connection
* 
* Playing With Fusion, Inc. invests time and resources developing open-source
* code. Please support Playing With Fusion and continued open-source 
* development by buying products from Playing With Fusion!
*
* **************************************************************************
* APPLICATION SPECIFIC NOTES (READ THIS!!!):
* - This file configures then runs a program on an Arduino to interface with
*   an AS3935 Franklin Lightning Sensor manufactured by AMS.
*    - Configure Arduino
*    - Perform setup for AS3935 chip
*      --- capacitance registers for tuning (based on cal value provided)
*      --- configurations for your application specifics (indoor/outdoor, etc)
*    - read status/info from sensor
*    - Write formatted information to serial port
* - Set configs for your specific needs using the #defines for wiring, and
*   review the setup() function for other settings (indoor/outdoor, for example)
* 
* Circuit:
*    R3aktor  -->  SEN-39001: AS3935 Breakout
*    Qwiic         Qwiic
**************************************************************************/
// The AS3935 communicates I2C when connected via qwiic cable
// This example uses the I2C interface via the Wire lib
#include "Wire.h"
// include Playing With Fusion AXS3935 libraries
#include "PWFusion_AS3935_I2C.h"

// interrupt trigger global var        
volatile int8_t AS3935_ISR_Trig = 0;

// defines for hardware config
#define SI_PIN               9
#define IRQ_PIN              2        // digital pins 2 and 3 are available for interrupt capability
#define AS3935_ADD           0x03     // x03 - standard PWF SEN-39001-R01 config
#define AS3935_CAPACITANCE   80       // <-- SET THIS VALUE TO THE NUMBER LISTED ON YOUR BOARD 
#define BOARD_IRQ            2        // For Uno pin 2 --> 0, For Uno pin 3 --> 1, For R3aktor pin 2 --> 2

// defines for general chip settings
#define AS3935_INDOORS       0
#define AS3935_OUTDOORS      1
#define AS3935_DIST_DIS      0
#define AS3935_DIST_EN       1

#define AS3935_LOCATION      AS3935_OUTDOORS   // Change depending on the sensor location
#define AS3935_DIST_UNIT     AS3935_DIST_EN   // Change depending on the prefered units for distance

// prototypes
void AS3935_ISR();

PWF_AS3935_I2C  lightning0((uint8_t)IRQ_PIN, (uint8_t)SI_PIN, (uint8_t)AS3935_ADD);

void setup()
{
  
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Playing With Fusion: AS3935 Lightning Sensor, SEN-39001-R01");
  Serial.println("beginning boot procedure....");
  
  // setup for the the I2C library: (enable pullups, set speed to 400kHz)
  Wire.begin();
  //Wire.pullup(true);
  //Wire.setSpeed(1); 
  delay(2);
  
  lightning0.AS3935_DefInit();   // set registers to default  
  // now update sensor cal for your application and power up chip
  lightning0.AS3935_ManualCal(AS3935_CAPACITANCE, AS3935_LOCATION, AS3935_DIST_UNIT);
                                 // AS3935_ManualCal Parameters:
                                 //   --> capacitance, in pF (marked on package)
                                 //   --> indoors/outdoors (AS3935_INDOORS:0 / AS3935_OUTDOORS:1)
                                 //   --> disturbers (AS3935_DIST_EN:1 / AS3935_DIST_DIS:2)
                                 // function also powers up the chip
                  

}

void loop()
{

  // retrieve statistics from AS3935
  uint8_t lightning_dist_km = lightning0.AS3935_GetLightningDistKm();
  uint32_t lightning_energy = lightning0.AS3935_GetStrikeEnergyRaw();

  Serial.print("The storm is ");
  Serial.print(lightning_dist_km);
  Serial.print("km away with the previous recorded strike energy value of ");
  Serial.println(lightning_energy);

  // Delay readings
  delay(1000);

}