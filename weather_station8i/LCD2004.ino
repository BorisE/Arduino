/*
    ** Example Arduino sketch for SainSmart I2C LCD2004 adapter for HD44780 LCD screens
 ** Readily found on eBay or http://www.sainsmart.com/
 ** The LCD2004 module appears to be identical to one marketed by YwRobot
 **
 ** Address pins 0,1 & 2 are all permanently tied high so the address is fixed at 0x27
 **
 ** Written for and tested with Arduino 1.0
 ** This example uses F Malpartida's NewLiquidCrystal library. Obtain from:
 ** https://bitbucket.org/fmalpartida/new-liquidcrystal
 **
 ** Edward Comer
 ** LICENSE: GNU General Public License, version 3 (GPL-3.0)
 */

 //LCDOFF 
 /*
 void wipeLines() {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 20; x++) {
      lcd.setCursor (x,y);
      lcd.print(" ");
      delay(10);
    }
  }
}
*/
