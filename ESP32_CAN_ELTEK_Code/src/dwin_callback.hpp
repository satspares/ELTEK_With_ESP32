
#include <Arduino.h>
#ifndef INCLUDES_H
    #include <includes.h>
#endif

// Event Occurs when response comes from HMI
void onHMIEvent(String address, int lastByte, String message, String response) {  // start tag onHMIEvent
#ifdef displayDebug
    Serial.println("OnEvent : [ A : " + address + " | D : " + String(lastByte, HEX) + " | M : " + message + " | R : " + response + " ]");
#endif


    char* endPtr;
    uint16_t displayVP = (uint16_t)strtol(address.c_str(), &endPtr, 16);
    if (*endPtr != '\0') {
    Serial.println("Bad VP address: " + address);
    return;
    }

    float voltSet;
    float currentSet;
     
    //page 0 start page
    //page 1 set volts and current
    //page 2 set default volts
    //page 3 set power supply id PSUID
    //page 8 restart page
    //page 9 please unset session
    //page 10 alarms
    //page 11 default volts not avaiable for old elteks 

switch(displayVP) {
    // Page 0 Controls
    case page0_right_arrow:{

     voltSet =  (sessionArray[SESSION_VOLT_LOCATION]);
        currentSet = (sessionArray[SESSION_CURRENT_LOCATION]);
        hmi.setFloatValue(page1_Volt_DC_Display, (voltSet / 100.f));
        hmi.setFloatValue(page1_Current_Display, (currentSet / 100.f));
        hmi.setVPWord(page1_Volt_Up_Down, (sessionArray[SESSION_VOLT_LOCATION]));
        hmi.setVPWord(page1_Current_Up_Down, (sessionArray[SESSION_CURRENT_LOCATION]));
        if ((display_status >= ELTEK_NORMAL) && (display_status <= ELTEK_CURRENT_LIMITED)) {
            hmi.setPage(SESSIONPAGE);
            if (use_session_info) { // change test color if using session values
                hmi.setTextColor(page1_Volt_DC_Display_SP, 3, 0x07F0);
                hmi.setTextColor(page1_Current_Display_SP, 3, 0x07F0);
            } else {
                hmi.setTextColor(page1_Volt_DC_Display_SP, 3, 0xFFFF);
                hmi.setTextColor(page1_Current_Display_SP, 3, 0xFFFF);
            }
            in_menu = true;
            hmi.beepHMI(BEEP_YES);
        } else {
            hmi.beepHMI(BEEP_ERROR);
        }
       
    }
    break;

    case page0_left_arrow:{
     in_menu = true;
        if ((use_session_info) && (!OLD_ELTEK)) {
            hmi.setPage(CANCELSESSIONPAGE);
            hmi.beepHMI(BEEP_CANCEL);
        } else {
            if ((display_status >= ELTEK_NORMAL) && (display_status <= ELTEK_CURRENT_LIMITED)) {
                hmi.setPage(DEFAULTVOLTPAGE);
                hmi.setVPWord(page2_Up_Down_control, sessionArray[DEFAULT_VOLTAGE_LOCATION]);
                voltSet = (float)sessionArray[DEFAULT_VOLTAGE_LOCATION];
                hmi.setFloatValue(page2_Voltage_Display, (voltSet / 100.f));
                in_menu = true;
                hmi.beepHMI(BEEP_YES);
            } else {
                hmi.beepHMI(BEEP_ERROR);
            }
        }      
    }
    break;
    // Page 1 Controls
    case page1_Volt_Up_Down:{
        voltSet = hmi.readVP(page1_Volt_Up_Down);
        hmi.setFloatValue(page1_Volt_DC_Display, (voltSet / 100.f));
        hmi.beepHMI(BEEP_TINY); // slows down key press   
    }
    break;

    case page1_Current_Up_Down:{
        currentSet = hmi.readVP(page1_Current_Up_Down);
        hmi.setFloatValue(page1_Current_Display, (currentSet / 100.f));
        hmi.beepHMI(BEEP_TINY);
    }
    break;

    case page1_Return:{
        in_menu = false;
        hmi.beepHMI(BEEP_YES);
        hmi.setPage(MAINPAGE);
    }
    break;

    case page1_Unset:{
        in_menu = false;
        hmi.beepHMI(BEEP_YES);
        use_session_info = false;
        counter_login_secsbool = true;
    }
    break;

    case page1_Set:{
         use_session_info = true;
        sessionArray[SESSION_VOLT_LOCATION] = hmi.readVP(page1_Volt_Up_Down);
        sessionArray[SESSION_CURRENT_LOCATION] = hmi.readVP(page1_Current_Up_Down);
        sessionArray[SESSION_VOLT_LOCATION] = constrain(sessionArray[SESSION_VOLT_LOCATION], BASE_VOLTAGE, MAX_VOLTAGE);
        sessionArray[SESSION_CURRENT_LOCATION] = constrain(sessionArray[SESSION_CURRENT_LOCATION], 10, MAX_CURRENT);

        eeprom_write_session_values();
        in_menu = false;
        counter_login_secsbool = true;   // call for set
        hmi.beepHMI(BEEP_YES);    
    }
    break;

    case page1_psid:{
        hmi.beepHMI(BEEP_YES);
        in_menu = true;
        hmi.setPage(PSIDPAGE);
        hmi.setText(page3_Serial_Display, "Serial: " + frame_array[0].psuid_string);
        hmi.setVPWord(page3_Up_Down, PSUID);
        hmi.setVPWord(page3_PSID_Display, PSUID);    
    }
    break;
    //  Page 9 Control
    case page9_Return:{
        hmi.setPage(MAINPAGE);
        hmi.beepHMI(BEEP_YES);
        in_menu = false;
    }
    break;
    // Page 10 Control
    case page10_Reboot:{
        clearFrameArrayDigits();
        hmi.setPage(MAINPAGE);
        display_status = ELTEK_STARTING;
        hmi.beepHMI(BEEP_YES);
    }    
    break;
       //  Page 2 Controls
    case page2_Return:{
        in_menu = false;
        hmi.setPage(MAINPAGE);
        hmi.beepHMI(BEEP_YES);
    }
    break;

    case page2_Up_Down_control:{
        voltSet = hmi.readVP(page2_Up_Down_control);
        hmi.setFloatValue(page2_Voltage_Display, (voltSet / 100.f));
        hmi.beepHMI(BEEP_TINY);
    }
    break;
  
    case page2_Set_Default_Voltage:{
        in_menu = true;
        default_voltage = hmi.readVP(page2_Up_Down_control);
        sessionArray[DEFAULT_VOLTAGE_LOCATION] = default_voltage;
        defaultVoltSent = true;
        eeprom_write_session_values();
        hmi.beepHMI(BEEP_YES);
        counter_login_secsbool = true;  // call for set now
    }
    break;

    // Page 8 Control 
    case page8_Return:{
        in_menu = false;
        hmi.setPage(MAINPAGE);
        hmi.beepHMI(BEEP_YES); 
    }
    break;
    // Page 3 Controls
    case page3_Return:{
        in_menu = false;
        sessionArray[DEFAULT_PSUID_LOCATION] = hmi.readVP(page3_Up_Down);
        PSUID = sessionArray[DEFAULT_PSUID_LOCATION];
        hmi.beepHMI(BEEP_YES);
        eeprom_write_session_values();
    }
    break;

    case page3_Up_Down:{
        hmi.setVPWord(page3_PSID_Display, hmi.readVP(page3_Up_Down));
        hmi.beepHMI(BEEP_YES);
    }
    break;

}  //switch end




} // end tag onHMIEvent
