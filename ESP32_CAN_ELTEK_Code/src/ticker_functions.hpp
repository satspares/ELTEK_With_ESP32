#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif

// ticker login timeout
void counterLoginsecs(){
    counter_login_secsbool = true;
}

// ticker every 1 minute 
void counterBootMins(){
 counter_reboot_mins += 1;
    if (counter_reboot_mins >= REBOOTMINS){
        Serial.println("No Activity Restarting ") ;
        delay(500);
        hmi.restartHMI();
        delay(1000);
        esp_restart();
    }
}

void counterDisplayRefresh(){
    counter_display_msbool = true;
}

void checkCurrentWarning(){
    if (display_status == ELTEK_ALARM) {
        hmi.beepHMI(BEEP_ERROR);
    }else if (frame_array[0].current_limited){
        if (!in_menu) hmi.beepHMI(BEEP_CURRENT_ALARM);
        digitalWrite(PINALARM,true);
    }else{
     digitalWrite(PINALARM,false);
    }
}
