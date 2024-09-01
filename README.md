# Manual Fan Controller for launch PS3 models
## Slow down or prevent the degredation of the ps3 (yellow light failure) by running the fans more aggressively than Sony's factory curves.

Originally designed using the Teensy 2.0 microcontroller (now planning to update to support other [read newer] microcontrollers). This takes control of the 
system fan away from playstation and gives the microcontroller (and the user) direct control of the fan speeds. It monitors temperatures of 
various components, most crucially the CELL and RSX, and sets a minimum fan speed based on this temperature.
### NOTE: This project was originally created using thermistors connected to the outside of the heatspreader of the CELL and RSX along with the Northbridge, intake and exhaust. These yield temperature readings likely 10C - 20C less than the actual temperatures of these components. The fan speeds are calculated with this in mind. As this is just a microcontroller using multiple temperature sensors to control (optionally multiple) fan speeds, this can be used for any such application. This project will likely be split into two repositories in the future. One that still uses just the external thermal sensors and thus can be used for any such temp/fan monitoring project, and one that will switch to reading data from the Playstation 3 motherboard to attain the actual temperature data of the components along with thermal sensors for intake/exhaust air.

## Visual Feedback
There is a 4 pin RGB LED used to indicate system temperatures at a glance. 
  - Blue colors indicate below normal operating temperatures.
  - Green indicates normal expected operating temperatures.
  - Yellow/Orange indicate temperatures are slightly higher than average even under heavy loads.
  - Red indicates the system is overheating.

The onboard LED of the Teensy Microcontroller flashes rapidly when the system is overheating. It is also used during the fan test mode to indicate
low/high speed set.

## Operating Modes
### Automatic
The fan speeds are set automatically based on the hottest component temperature. No interaction is needed from the user.
### Manual
The fan speeds are set based on the position of the dial potentiometer which can be turned using a screwdriver through the front vent holes of 
the ps3 (or with an exposed dial depending on how the mod is built). The system will not allow the fan to be set below a determined minimum 
speed which is based on the monitored temperatures. The speeds will automatically raise and lower when the temperatures indicate a needed fan 
speed higher than what the user has set, but will return to the set speed if temperatures fall low enough.
### Test
The fan speed is cycled through its full range and the temperature indicating LED changes to white. The fan will start at 0 with the LED off, then 
ramp smoothly to 100% with the LED increasing in brightness while doing so before slowning back down to 0. The mode then automatically exits and 
the fan speed and LED color both return to what is dictated by the other modes it returns to.

## Building
### Software
This is straight forward enough, just open the .ino project in the arduino editor and select the correct board type and COM port. 
### Code Edits
  - There are currently two linear maps defined for the fan speed targets. They can be choosen at compile time with the AGRESSIVE #define. Un-commenting this #define 
will use a much more aggressive 'curve' (still just a linear map) by ramping up the fan speed earlier and targeting a lower temperature. This is highly recommended
if this mod is being installed in a system that has been previously repaired with a reflow or had any past YLOD type failure.
  - Inside of temperature.h exist #defines for the balance resistors used in the temperature monitoring circuit. These should be set to equal the real world values 
of your specific 10k resistors. This will give a more accurate temperature calculation if done. Otherwise, just set them each to 10000.
### Hardware
Coming soon / this is slightly more complicated. I'm now planning on uploading a board design file and a parts list, but as this project was originally made nearly
12 years earlier than this writing I need to track down the correct (or compatible) model of thermistors used as well as actually draw the PCB in 
Eagle to share it. The original was made with a chemically etched and sticker-masked custom PCB. You can easily build the circuit using prototyping 
board but soon enough there will the option to make a proper PCB.
### Console Modification
Coming soon.
