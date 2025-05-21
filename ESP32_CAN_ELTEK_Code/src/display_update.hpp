#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif


void update_display(){
counter_display_msbool = false;

    if (display_status <=ELTEK_CURRENT_LIMITED){
        hmi.setVPWord(page0_Banner_Ico,display_status+20);
    if (!in_menu){
        hmi.setPage(MAINPAGE);
    }
    }else if (display_status == ELTEK_ALARM){   // error
      hmi.setPage(ERRORPAGE);
      hmi.setText(Error1_Message,frame_array[0].alerts0);
      hmi.setText(Error2_Message,frame_array[0].alerts1);
    }   

hmi.setFloatValue(page0_DC_Volts,frame_array[0].voltage_output);
hmi.setVPWord(page0_AC_Volts,frame_array[0].voltage_input);
hmi.setFloatValue(page0_Current,frame_array[0].current_output);
hmi.setVPWord(page0_Power,frame_array[0].power_output);
hmi.setVPWord(page0_Temp_In,frame_array[0].temp_inlet);
hmi.setVPWord(page0_Temp_Out,frame_array[0].temp_outlet);

}