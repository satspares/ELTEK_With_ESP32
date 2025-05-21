// define for debug print settings
#define myDebug
//#define frameINFO
#define displayDebug
#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>
#include <myTicker.h> // clone Ticker.h as it picks up the wrong Ticker.h (esp8266?)
#include <DWIN_Arduino.h>
#include <EEPROM.h>
#include <esp_task_wdt.h>
#include <includes.h>
#include <frames.hpp>
#include <eeprom_functions.hpp>
#include <ticker_functions.hpp>
#include <startup_functions.hpp>
#include <dwin_callback.hpp>
#include <display_update.hpp>


void setup() {
    // Setup serial for debuging.
    Serial.begin(115200);
  //Serial1.begin(115200, SERIAL_8N1,1,0); // serial1 if needed
  // uses pin 0 for current alarm at the moment
    delay(5000);
    Serial.println("Starting ");
    delay(500);
    readEEPROM();
    setupDisplay();   
   
    pinMode(CAN_TX,OUTPUT);     // maynot be needed
    pinMode(PINALARM,OUTPUT);
    digitalWrite(PINALARM,false);

    if (ESP32Can.begin(TWAI_SPEED_125KBPS, CAN_TX, CAN_RX, 10, 10)) {
        LOG_TWAI("CAN bus started!");
    } else {
        LOG_TWAI("CAN bus failed to start!");
    }

    esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
    // esp_task_wdt_add(NULL); //add current thread to WDT watch
    if (esp_task_wdt_add(NULL) != ESP_OK)
    {
        Serial.println("Unable to add wdt task");
    }

    tickerReboot.attach(60,counterBootMins);
    tickerDisplayRefresh.attach_ms(DISPLAY_REFRESH_TIME_MS,counterDisplayRefresh);
    tickerCheckCurrentLimit.attach(CURRENTLIMITSECSCHECK,checkCurrentWarning);

    display_status = ELTEK_STARTING;
    clearFrameArrayDigits();   
    if (USESERIALNO){     
     uint8_t byteCount  = 40;     
        for (int i = 0;i < 6; ++i){
            serialNumber[i] = ((PSUSERIAL >> byteCount) & 0xFF);
            byteCount -= 8;    
        }   
        serialNumberRXed = true;
        tickerLoginSecs.attach(LOGINTIME,counterLoginsecs);
        // Try login now
        sendLogin(0x05004800+(PSUID << 2),1,serialNumber); 

    }    
}

void loop() {
    if ((millis() - wdtLast >= ((WDT_TIMEOUT * 1000)) - 15000) && wdtStopped == false)  //15
    { // reset wdt every 15 seconds
        #ifdef WDTDebug
        Serial.println("Resetting WDT...");
        #endif
        esp_task_wdt_reset();
        wdtLast = millis();
    }
    hmi.listen();
   
    // You can set custom timeout, our default is 500ms
    if (ESP32Can.readFrame(rxFrame, FRAME_TIMEOUT_MS)) {
        uint32_t identifier;
        #ifdef myDebug
        Serial.printf("Received frame: %03X  \r\n", rxFrame.identifier);
        if (rxFrame.extd) {
            Serial.print("extended ");
        } else {
            Serial.print("standard ");
        }
        if (rxFrame.rtr) {
            Serial.print("RTR ");
        }
        Serial.printf("packet with id 0x%x", rxFrame.identifier);
        #endif   
        
        if (rxFrame.rtr) {
            #ifdef myDebug
            Serial.printf(" and requested length %d\n", rxFrame.data_length_code);
            #endif
        }else{
            #ifdef myDebug
            Serial.printf(" and length %d\n", rxFrame.data_length_code);
             for (int i = 0; i < rxFrame.data_length_code ; i++) {
                 Serial.print(rxFrame.data[i],HEX);
                 Serial.print(" ");
             } 
             #endif 
            counter_reboot_mins = 0; //reset reboot counter
            identifier =  (rxFrame.identifier & TWAI_EXTD_ID_MASK); 
            processFrame(identifier);        
        }
        
    }  // end if (ESP32Can.readFrame

    // after ticker timeout
    if (counter_login_secsbool){
    // Old Eltek fake the default voltage
        if (OLD_ELTEK && !use_session_info){
            setSession(0x05FF4004,sessionArray[DEFAULT_VOLTAGE_LOCATION], MAX_VOLTAGE, MAX_CURRENT);
        }

        if (serialNumberRXed){
           sendLogin(0x05004800+(PSUID << 2),1,serialNumber); 
            delay(FRAME_DELAY);
              
            Serial.println("login timeout ticker reset "); 
              if (use_session_info){   
                setSession(0x05FF4004,sessionArray[SESSION_VOLT_LOCATION],MAX_VOLTAGE,sessionArray[SESSION_CURRENT_LOCATION]);
              }else if (!OLD_ELTEK){  // set back to default voltage newer Eltek
                Serial.print("DEFAULT_VOLTAGE ");Serial.println(sessionArray[DEFAULT_VOLTAGE_LOCATION]);
                setSession(0x05FF4004,sessionArray[DEFAULT_VOLTAGE_LOCATION], MAX_VOLTAGE, MAX_CURRENT); 
              }  
             if((defaultVoltSent) && (!use_session_info) && (received_id == PSUID)){
               delay(FRAME_DELAY);  
               setDefaultVoltage(default_voltage); 
               defaultVoltSent = false;   
               Serial.println("DEFAULT VOLTAGE SET ");
               hmi.setPage(RESTARTPAGE);
               }
        }
       counter_login_secsbool = false; 
    }  // END counter_login_secsbool

    if (counter_display_msbool){
       update_display();
    }

}




