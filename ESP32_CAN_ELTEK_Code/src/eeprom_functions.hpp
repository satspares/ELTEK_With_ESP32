#include <Arduino.h>
#ifndef INCLUDES_H
#include <includes.h>
#endif


/* ======= Read EEPROM on boot and set session values======== */  
void readEEPROM(){
    EEPROM.begin(EEPROM_SIZE);
    if (EEPROM.read(EEPROM_NEW_ON_ADDRESS) != 0x5a){
      for (int i = 1 ; i < EEPROM_SIZE; i++) {  
        EEPROM.write(i, 0xff); // wipe eeprom
        EEPROM.commit();
        delay(10);
       }
       EEPROM.write(EEPROM_NEW_ON_ADDRESS,0x5a);
       EEPROM.commit();
       delay(10);
       eeprom_write_session_values();
    }else{
      // eeprom_write_session_values(); // write test
      // delay(10);
       eeprom_read_session_values();

       // check for out of range
       sessionArray[SESSION_VOLT_LOCATION] = constrain(sessionArray[SESSION_VOLT_LOCATION],BASE_VOLTAGE,MAX_VOLTAGE);
       sessionArray[DEFAULT_VOLTAGE_LOCATION] = constrain(sessionArray[DEFAULT_VOLTAGE_LOCATION],BASE_VOLTAGE,MAX_VOLTAGE);
       sessionArray[SESSION_VOLT_HIGH_LOCATION] = MAX_VOLTAGE;       
       sessionArray[SESSION_CURRENT_LOCATION] = constrain(sessionArray[SESSION_CURRENT_LOCATION],10,MAX_CURRENT);
      
      for(int j = 0; j < (sizeof(sessionArray)/sizeof(uint16_t)); j++)
      {  
        Serial.print(sessionArray[j]);
        Serial.print(" ");
      }
      

    }  

     
}   

void eeprom_write_session_values(){
     EEPROM_writeAnything(ARRAY_START_ADDRESS, sessionArray);         
} 
void eeprom_read_session_values(){
     EEPROM_readAnything(ARRAY_START_ADDRESS, sessionArray);  
}