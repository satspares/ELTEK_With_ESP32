ELTEK Flatpack2 control using ESP32/c3 DWIN Display DMG48320C035_03WTC (480x320)

In the can-bus-kicad directory is a PCB design using ESP32c3 but you can use any ESP32 (5v supply needed) and a sn65hvd230 CAN Chip

I am still working on the code for the older Flatpack2's so the code is not complete yet (fine for V3 Elteks)

The display is from the DWIN Store on Aliexpress about 18.00usd plus carriage.

As the Eltex was designed to charge batteries the current limit when active reduces the output voltage to approx. 47volts.
When this happens we display a warning and sound a beep also put a ESP32 output pin high. (pull a relay etc. with a driver transistor)

The folder to upload to the display via. sdcard is DWIN_SET
Its also worth looking at the DWIN docs. in my github https://github.com/satspares/DWIN_DGUS_HMI





 
