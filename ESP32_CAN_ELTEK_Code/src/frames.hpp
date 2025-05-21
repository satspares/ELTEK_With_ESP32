#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif

void sendLogin(uint32_t ID, uint8_t extd, uint8_t data[]){
 CanFrame loginFrame = {0};
 loginFrame.identifier = ID;
 loginFrame.extd = extd;
 loginFrame.data_length_code = 8;
 for (int i = 0;i < 6; ++i){
    loginFrame.data[i] = data[i];
 }
 ESP32Can.writeFrame(loginFrame);
  if (!serialNumberRXed) display_status = ELTEK_TRYING_LOGIN;
 Serial.println(" ");
 Serial.println("Sent Login Frame");
}

bool processFrame(uint32_t identifier){
  char serial_out[3];  

  if (((identifier & 0xffff0000)  == 0x05000000) && (!serialNumberRXed)){ // || (rxFrame.data[0] == 0x1b/5b)) ){
            for (int i = 0; i < 6; ++i) {
		        serialNumber[i] = rxFrame.data[i+1];
                serialNumberRXed = true;
                #ifdef myDebug
		              snprintf(serial_out, 3, "%.2X", serialNumber[i]);
		              Serial.print(serial_out);
                #endif
	        }
            delay(FRAME_DELAY);
            display_status = ELTEK_TRYING_LOGIN;
            clearFrameArrayDigits();
            tickerLoginSecs.attach(LOGINTIME,counterLoginsecs);
            sendLogin(0x05004800+(PSUID << 2),1,serialNumber); 
            delay(FRAME_DELAY);
            if (use_session_info && serialNumberRXed){     
                setSession(0x05FF4004,sessionArray[SESSION_VOLT_LOCATION], MAX_VOLTAGE, sessionArray[SESSION_CURRENT_LOCATION]);
            }          
    }

    

    if( ((identifier & 0xff00fff0) == 0x05004000)){
    received_id = (identifier & 0x00FF0000) >> 16;   // get back our eltek id
    #ifdef myDebug
     Serial.print("Received ID ");Serial.println(received_id);
    #endif 
        if (received_id != PSUID) return false;  // message not for us return
    }

         
    if (serialNumberRXed){    
        switch(identifier & 0xff00ffff){
         case 0x05004004: {
           saveFrame(rxFrame.data,false,false);
           display_status = ELTEK_NORMAL;
         }
         break;
         case 0x05004008: {
           saveFrame(rxFrame.data,true,false);
           display_status = ELTEK_CURRENT_LIMITED;
           Serial.println("Current Limited ");
         }
         break;
         case 0x0500400C: {
           Serial.println("Alarm ");
           display_status = ELTEK_ALARM;
           clearFrameArrayDigits();
           alertRequest(ELTEK_ERROR);
         }
         break;
         case 0x05004010: {
            saveFrame(rxFrame.data,false,true);
            display_status = ELTEK_WALKIN;
            Serial.println("Walkin voltage ramping up");
         }
         break;
         case 0x0500bffc: {
            Serial.println("Alert Information ");
            display_status = ELTEK_ALERT_INFO;
            uint8_t alarms0 = rxFrame.data[3];
            uint8_t alarms1 = rxFrame.data[4];
            for (int i = 0; i < 8; i++) {
                    if (alarms0 & (1 << i)) {
                       frame_array[0].alerts0 = String(alarms0Strings[i]);
                    } 
                    if (alarms1 & (1 << i)) {
                      frame_array[0].alerts1 = alarms1Strings[i];
                    }
            }
         }
         break;
         default:
          #ifdef myDebug
          Serial.print("frame not needed by us.. ");Serial.println(identifier & 0xff00ffff,HEX);
          #endif
         break;
        }              
    }    
    return true;
}  // end processFrame


void setSession(uint32_t ID,uint16_t voltage, uint16_t hi_voltage, uint16_t current){
    CanFrame sessionFrame = {0};
    if (LONG_WALKIN) ID+=1;

    current = current/10;
    sessionFrame.identifier = ID;
    sessionFrame.extd = 1;
    sessionFrame.data_length_code = 8;
    sessionFrame.data[0] = lowByte(current); sessionFrame.data[1] = highByte(current);

    sessionFrame.data[2] = lowByte(voltage); sessionFrame.data[3] = highByte(voltage);
    sessionFrame.data[4] = lowByte(voltage); sessionFrame.data[5] = highByte(voltage);
    sessionFrame.data[6] = lowByte(hi_voltage); sessionFrame.data[7] = highByte(hi_voltage);
    ESP32Can.writeFrame(sessionFrame);
    Serial.println(" ");
    Serial.println("Sent Session Frame");
}

void saveFrame(uint8_t frameCopy[], bool current_limit, bool ramping){
    frame_array[0].temp_inlet = frameCopy[0];
    frame_array[0].temp_outlet = frameCopy[7];
    frame_array[0].voltage_input = (uint16_t)(256 * frameCopy[6]+ frameCopy[5]);
    frame_array[0].voltage_output = ((256 * frameCopy[4]+frameCopy[3])/100.0);
    frame_array[0].current_output = ((256 * frameCopy[2]+frameCopy[1])/10.0);
    frame_array[0].power_output = (uint16_t)((frame_array[0].current_output * frame_array[0].voltage_output));
    frame_array[0].current_limited = current_limit;
    frame_array[0].ramping_up = ramping;
    frame_array[0].alerts0 = "*";
    frame_array[0].alerts1 = "*";

    #ifdef frameINFO
    Serial.println(" ");
    Serial.println("Status ");  
    Serial.print("temp outlet ");    Serial.println(frame_array[0].temp_outlet);
    Serial.print("temp Inlet ");     Serial.println(frame_array[0].temp_inlet);
    Serial.print("Voltage Input " ); Serial.println(frame_array[0].voltage_input);
    Serial.print("Voltage Output "); Serial.println(frame_array[0].voltage_output);
    Serial.print("Current Output "); Serial.println(frame_array[0].current_output);
    Serial.print("Power Output ");   Serial.println(frame_array[0].power_output);
    #endif
}


 void setDefaultVoltage(uint16_t setVoltage){  // before setSession is active
    CanFrame defaultFrame = {0};
    defaultFrame.identifier = 0x05009c00+(PSUID << 16);
    defaultFrame.extd = 1;
    defaultFrame.data_length_code = 5;
    defaultFrame.data[0] = 0x29; //lowByte(baseVoltage); ?
    defaultFrame.data[1] = 0x15; //highByte(baseVoltage); ?
    defaultFrame.data[2] = 0;
    defaultFrame.data[3] = lowByte(setVoltage);
    defaultFrame.data[4] = highByte(setVoltage);
    ESP32Can.writeFrame(defaultFrame);
    Serial.println(" ");
    Serial.println("Sent DefaultFrame");
 }


 void alertRequest(uint8_t alarmType){
   CanFrame alertFrame = {0};
   alertFrame.identifier = 0x0500bffc;
   alertFrame.extd = 1;
   alertFrame.data_length_code = 3;
   alertFrame.data[0] = 0x08;
   alertFrame.data[1] = alarmType;
   alertFrame.data[2] = 0; 
   ESP32Can.writeFrame(alertFrame);
   Serial.println(" ");
   Serial.println("Sent Alert Frame");
 }

void clearFrameArrayDigits(){
    frame_array[0].temp_inlet = 0; frame_array[0].temp_outlet = 0;
    frame_array[0].voltage_input = 0; frame_array[0].voltage_output = 0;
    frame_array[0].current_output = 0; frame_array[0].power_output = 0;
}