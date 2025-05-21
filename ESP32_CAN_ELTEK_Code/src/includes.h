#include <Arduino.h>
#ifndef INCLUDES_H
#define INCLUDES_H

/* ========= User Settings= ======== */

// if you have a serial enter it below
// please note it is a HEX number as per. example
// if not we will work it out
const bool USESERIALNO = false;
const uint64_t PSUSERIAL = 0x183471016561;

const uint8_t PSUID = 2;                  // Eltek ID no. we set 
const uint8_t LOGINTIME = 10;             // < (64*0.2)  time between logins set 5-10 seconds
const uint8_t REBOOTMINS = 5;             // reboot time if no can messages 
const uint8_t CURRENTLIMITSECSCHECK = 3;  // seconds to check for current limit;
const uint16_t FRAME_TIMEOUT_MS = 500;
const uint16_t DISPLAY_REFRESH_TIME_MS = 500;
const uint8_t DISPLAY_BRIGHTNESS = 25;   // range 1-100
const bool LONG_WALKIN = false;          // false for 5 seconds true for 60 seconds
const bool OLD_ELTEK = false;
// ELTEK before HE version?
// Not possible to set default voltage?
// we use a session for this does anyone know different?
// we set the session active on start for these.
// session voltage/current is active all the time.
// Also no current display limit warning


// user beep settings in milli seconds
#define BEEP_CURRENT_ALARM 65
#define BEEP_YES 50
#define BEEP_CANCEL 300
#define BEEP_ERROR 700
/* ========= Page Numbers ========== */
const uint8_t MAINPAGE = 0;
const uint8_t SESSIONPAGE = 1;
const uint8_t DEFAULTVOLTPAGE = 2;
const uint8_t RESTARTPAGE = 8;
const uint8_t CANCELSESSIONPAGE = 9;
const uint8_t ERRORPAGE = 10;
const uint8_t DEFAULTWARNING = 11;  // old eltek cant set default voltage


/* ======= Global Instances ======== */
CanFrame rxFrame;
Ticker tickerLoginSecs;
Ticker tickerReboot;
Ticker tickerDisplayRefresh;
Ticker tickerCheckCurrentLimit;

/* =============== Pins ============= */
const uint8_t CAN_TX = 4;
const uint8_t CAN_RX = 3; 
// display tx/rx/pins
const uint8_t PINRX = 20;
const uint8_t PINTX = 21;
const uint8_t PINALARM = 0;  // current alarm


/* ============= other const ========== */
const uint8_t EEPROM_SIZE = 50;
const uint8_t FRAME_DELAY = 100; // delay between sending frames
const uint8_t WDT_TIMEOUT = 30;
const uint8_t EEPROM_NEW_ON_ADDRESS = 40;
// session array location in eeprom
const uint8_t ARRAY_START_ADDRESS = 0;
// location in the array of storage location
const uint8_t SESSION_VOLT_LOCATION = 0;
const uint8_t SESSION_VOLT_HIGH_LOCATION = 1;
const uint8_t SESSION_CURRENT_LOCATION = 2;
const uint8_t DEFAULT_VOLTAGE_LOCATION = 3;
const uint16_t BASE_VOLTAGE = 4350;  // 43.5*100 
const uint16_t MAX_VOLTAGE = 5760; // 57.6*100
const uint16_t MAX_CURRENT = 6250; // 62.5*100 or 33.6*100 48/2000 elteks /10 later
const uint16_t OLD_ELTEK_DEFAULT_VOLTAGE = 5350;
const uint8_t ELTEK_WARNING = 0x04;
const uint8_t ELTEK_ERROR = 0x08;
// used by the display
const uint8_t ELTEK_STARTING = 0x00;
const uint8_t ELTEK_TRYING_LOGIN = 0x01;
const uint8_t ELTEK_ALIVE = 0x02;
const uint8_t ELTEK_NORMAL = 0x03;
const uint8_t ELTEK_WALKIN = 0x04;
const uint8_t ELTEK_CURRENT_LIMITED = 0x05;
const uint8_t ELTEK_ALERT_INFO = 0x06;
const uint8_t ELTEK_ALARM = 0x07;


/* ============= Global Vars ========== */

uint32_t wdtLast = millis();
uint16_t default_voltage = 4800;
uint8_t received_id = 0;   // id sent back from eltek
uint8_t serialNumber[6];
uint8_t counter_reboot_mins = 0;
uint8_t display_status = 0;
bool wdtStopped = false;
bool defaultVoltSent = false;
bool serialNumberRXed = false;
bool counter_login_secsbool  = false;
bool counter_display_msbool = false; 
bool use_session_info = false;
bool in_menu = false; // we are in a menu item


uint16_t sessionArray[] = {4950, 5760, 1600, 5000, 0xffff, 0xffff, 0xffff, 0xffff};
//0 set_voltage - 1 max_voltage - 2 current_set - 3 default Voltage - spares..

// From https://github.com/xjamesmorris/fp_util/blob/main/fp_util.ino
const char *alarms0Strings[] = { "OVS_LOCK_OUT", "MOD_FAIL_PRIMARY", "MOD_FAIL_SECONDARY",
                                 "HIGH_AC_SUPPLY", "LOW_AC_SUPPLY", "HIGH_TEMP", "LOW_TEMP", "CURRENT_LIMIT" };

const char *alarms1Strings[] = { "INTERNAL_VOLTAGE", "MODULE_FAIL", "MOD_FAIL_SECONDARY",
                                 "FAN1_SPEED_LOW", "FAN2_SPEED_LOW", "SUB_MOD1_FAIL", "FAN3_SPEED_LOW", "INNER_VOLT" };

  typedef struct
  {
    uint8_t temp_inlet;
    uint8_t temp_outlet;
    uint16_t voltage_input;
    float voltage_output;
    float current_output;
    uint16_t power_output;  
    bool current_limited;  
    bool ramping_up;
    String alerts0;
    String alerts1;
  }frame_type;
  frame_type frame_array[4];  //4 for later

/* ============= DWIN Global =========== */
#define DGUS_BAUD 115200
#define DGUS_SERIAL Serial0
DWIN hmi(DGUS_SERIAL, PINRX, PINTX, DGUS_BAUD);

/* ============= DWIN Const ============ */
const uint32_t LCDBackLightNormal = ((unsigned long)0x643A03E8); //Normal Brigtness 
const uint32_t LCDBackLightLow = ((unsigned long)0x641503E8);    //0x641503E8)   
const uint16_t LCDBackLightAddress = 0x0082;
// Page 0 touch controls
const uint16_t page0_right_arrow = 0x1000;
const uint16_t page0_left_arrow = 0x1001;
// Page 0 display controls
const uint16_t page0_DC_Volts = 0x1100;
const uint16_t page0_DC_Volts_SP = 0x5100;
const uint16_t page0_AC_Volts = 0x1105;
const uint16_t page0_AV_Volts_SP = 0x5120;
const uint16_t page0_Current = 0x1110;
const uint16_t page0_Current_SP = 0x5140;
const uint16_t page0_Power = 0x1115;
const uint16_t page0_power_SP = 0x5160;
const uint16_t page0_Temp_In = 0x1120;
const uint16_t page0_Temp_In_SP = 0x5180;
const uint16_t page0_Temp_Out = 0x1125;
const uint16_t page0_Temp_Out_SP = 0x5200;
const uint16_t page0_Banner_Ico = 0x5210;

/* ============= Page 1 ============ */
//Page 1 touch controls
const uint16_t page1_Return = 0x1010;
const uint16_t page1_Volt_Up_Down = 0x1011;
const uint16_t page1_Current_Up_Down = 0x1012;
const uint16_t page1_Unset = 0x1015;
const uint16_t page1_Set = 0x1016;
//Page 1 Display Controls
const uint16_t Page1_Volt_DC_Display = 0x1140;
const uint16_t Page1_Current_Display = 0x1145;
const uint16_t Page1_Volt_DC_Display_SP = 0x5250;
const uint16_t Page1_Current_Display_SP = 0x5300;

/* ============= Page 9 ============ */
// remove session warning
// Page 9 touch control 
const uint16_t Page9_Return = 0x1020;

/* ============= Page 2 ============ */  //set default voltage
// Page 2 touch controls
const uint16_t Page2_Return = 0x1030; 
const uint16_t Page2_Set_Default_Voltage = 0x1031;
const uint16_t Page2_Up_Down_control = 0x1032;
// Page 2 Display Controls
const uint16_t Page2_Voltage_Display = 0x1150;

/* ============= Page 8 ============ */
// Page 8 Control
const uint16_t Page8_Return = 0x1040;

/* ============= Page 10 ============ */  // error message page
uint16_t Error1_Message = 0x1201;
uint16_t Error2_Message = 0x1230;
uint16_t page10_Reboot = 0x1160;


/* ======= function forward references ======== */
//
void sendLogin(uint32_t ID, uint8_t extd, uint8_t data[]);
void counterLoginsecs();
void counterBootMins();
void counterDisplayRefresh();
bool processFrame(uint32_t identifier);
void setSession(uint32_t ID,uint16_t voltage, uint16_t hi_voltage, uint16_t current);
void saveFrame(uint8_t frameCopy[], bool current, bool ramping);
void alertRequest(uint8_t alarmType);
void setDefaultVoltage(uint16_t setVoltage);
void readEEPROM();   
void eeprom_write_session_values();
void eeprom_read_session_values();
void setupDisplay();
void update_display();
void setScreenRotate(byte angle);
void clearFrameArrayDigits();
void onHMIEvent(String address, int lastByte, String message, String response);
void checkCurrentWarning();

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
          EEPROM.commit();
          delay(10);
    return i;
}
template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}


#endif
// endif include guard
