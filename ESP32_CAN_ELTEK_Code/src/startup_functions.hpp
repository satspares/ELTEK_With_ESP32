#include <Arduino.h>
#ifndef INCLUDES_H
    #include <includes.h>
#endif

void setupDisplay() {
    // on DWIN Display board RX2/TX2
    // we are Serial0
    hmi.initSerial(DGUS_SERIAL, DGUS_BAUD);
    hmi.hmiCallBack(onHMIEvent);  //set callback
    hmi.echoEnabled(false);       //dont want to see all the display transactions
    hmi.ackDisabled(true);        //for our no ack kernel
#ifdef SCREENROTATE
    /* ======= Screen rotate should be set in .cfg config file ======== */
    const byte rotate0 = 0; const byte rotate90 = 1;
    const byte rotate180 = 2; const byte rotate270 = 3;
    setScreenRotate(rotate90);  // some display set to portrait when new!
    delay(50);
#endif
    hmi.setPage(MAINPAGE);
    hmi.setBrightness(DISPLAY_BRIGHTNESS);
    hmi.setTPBeep(false); // turn off dwin touch beep use ours
    hmi.returnWord(false); // return byte normal behavior returned in onHMIEvent (int)lastByte 

    #ifdef displayDebug
    Serial.print("DWIN GUI SoftVersion v" );
    Serial.println(hmi.getGUISoftVersion(),HEX);
    Serial.print("DWIN OS SoftVersion Version v" );
    Serial.println(hmi.getOSSoftVersion(),HEX);
    #endif
}



#ifdef SCREENROTATE
//rotate the screen  eg. setScreenRotate(rotate270); // flip the screen
void setScreenRotate(byte angle) {
    hmi.setVPWord(0x0081, ((hmi.readVP(0x0081) & 0xFFFC) + angle));
    hmi.setVPWord(0x0080, 0x5A00);  // set it.
}
#endif
