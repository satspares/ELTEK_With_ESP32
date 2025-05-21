
#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif

// Event Occurs when response comes from HMI
void onHMIEvent(String address, int lastByte, String message, String response)
{  // start tag onHMIEvent
  #ifdef displayDebug
  Serial.println("OnEvent : [ A : " + address + " | D : " + String(lastByte, HEX) + " | M : " + message + " | R : " + response + " ]");
  #endif
 
  uint16_t displayVP = strtol(address.c_str(),0,16);
  float voltSet;
  float currentSet;
  // Page 0 Controls
  if(page0_right_arrow == displayVP){
    voltSet =  (sessionArray[SESSION_VOLT_LOCATION]); 
    currentSet = (sessionArray[SESSION_CURRENT_LOCATION]);
    hmi.setFloatValue(Page1_Volt_DC_Display,(voltSet/100));
    hmi.setFloatValue(Page1_Current_Display,(currentSet/100));
    hmi.setVPWord(page1_Volt_Up_Down,(sessionArray[SESSION_VOLT_LOCATION]));
    hmi.setVPWord(page1_Current_Up_Down,(sessionArray[SESSION_CURRENT_LOCATION]));
    if ((display_status >= ELTEK_NORMAL) && (display_status <= ELTEK_CURRENT_LIMITED)){
      hmi.setPage(SESSIONPAGE);
        if(use_session_info){ // change test color if using session values
          hmi.setTextColor(Page1_Volt_DC_Display_SP,3,0x07F0);
          hmi.setTextColor(Page1_Current_Display_SP,3,0x07F0);
        }else{
          hmi.setTextColor(Page1_Volt_DC_Display_SP,3,0xFFFF);
          hmi.setTextColor(Page1_Current_Display_SP,3,0xFFFF);
        }
      in_menu = true;
      hmi.beepHMI(BEEP_YES);
    }else{
      hmi.beepHMI(BEEP_ERROR);
    }
  }
  else if(page0_left_arrow == displayVP){   // set default voltage
    in_menu = true;
    if ((use_session_info) && (!OLD_ELTEK)) {
      hmi.setPage(CANCELSESSIONPAGE);
      hmi.beepHMI(BEEP_CANCEL);
    }else{
       if ((display_status >= ELTEK_NORMAL) && (display_status <= ELTEK_CURRENT_LIMITED)){
        hmi.setPage(DEFAULTVOLTPAGE);
        hmi.setVPWord(Page2_Up_Down_control,sessionArray[DEFAULT_VOLTAGE_LOCATION]);
        voltSet = sessionArray[DEFAULT_VOLTAGE_LOCATION];
        hmi.setFloatValue(Page2_Voltage_Display,(voltSet/100));
        in_menu = true;
        hmi.beepHMI(BEEP_YES);
      }else{
        hmi.beepHMI(BEEP_ERROR);
      }    
    }
  }
  // Page 1 Controls
  else if(page1_Volt_Up_Down == displayVP){
   voltSet = hmi.readVP(page1_Volt_Up_Down) ;
   hmi.setFloatValue(Page1_Volt_DC_Display,(voltSet/100));
  }
  else if(page1_Current_Up_Down == displayVP){
   currentSet = hmi.readVP(page1_Current_Up_Down);
   hmi.setFloatValue(Page1_Current_Display,(currentSet/100));
  }
  else if(page1_Return == displayVP){
    in_menu = false;
    hmi.beepHMI(BEEP_YES);
    hmi.setPage(MAINPAGE);
  }
  else if(page1_Unset == displayVP){
      in_menu = false;
      hmi.beepHMI(BEEP_YES); 
      use_session_info = false;  
      counter_login_secsbool = true;
  }
  else if(page1_Set == displayVP){
    use_session_info = true;
    sessionArray[SESSION_VOLT_LOCATION] = hmi.readVP(page1_Volt_Up_Down);
    sessionArray[SESSION_CURRENT_LOCATION] = hmi.readVP(page1_Current_Up_Down);
    eeprom_write_session_values();
    in_menu = false;
    counter_login_secsbool = true;   // call for set
    hmi.beepHMI(BEEP_YES);
  }
  // Page 9 Control  
  else if (Page9_Return == displayVP){
    hmi.setPage(MAINPAGE);
    hmi.beepHMI(BEEP_YES);
    in_menu = false;
  }
  // Page 10 Control;
  else if(page10_Reboot == displayVP){
     clearFrameArrayDigits(); 
     hmi.setPage(MAINPAGE);
     display_status = ELTEK_STARTING;
     hmi.beepHMI(BEEP_YES);
  } 
   // Page 2 Controls   
  else if(Page2_Return == displayVP){
      in_menu = false;
      hmi.setPage(MAINPAGE);
      hmi.beepHMI(BEEP_YES);
  }
  else if(Page2_Up_Down_control == displayVP){
      voltSet = hmi.readVP(Page2_Up_Down_control);
      hmi.setFloatValue(Page2_Voltage_Display,(voltSet/100));
  } 
  else if(Page2_Set_Default_Voltage == displayVP){
      in_menu = true;
      default_voltage = hmi.readVP(Page2_Up_Down_control);
      sessionArray[DEFAULT_VOLTAGE_LOCATION] = default_voltage;
      defaultVoltSent = true;
      eeprom_write_session_values();
      hmi.beepHMI(BEEP_YES);
    }
  else if(Page8_Return == displayVP){
      in_menu = false;
      hmi.setPage(MAINPAGE);
      hmi.beepHMI(BEEP_YES);
   }


} // end tag onHMIEvent  