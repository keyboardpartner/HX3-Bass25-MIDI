# HX3 Bass25 MIDI

![Bass25 Pic](https://github.com/keyboardpartner/HX3-Bass25-MIDI/blob/main/docs/Midibass25_m.JPG)

### Firmware for KeyboardPartner Bass25 MIDI Scan Controller

Based on ATtiny2313 MCU, scans a 25-note bass pedal through a series of 4014 CMOS shift registers. Scan time is around 60 µs, but using a delay of 5 ms for debounce.

For scanning bass pedals up to 25 keys with single switches, contact to ground. Built-in MIDI controller, sends notes on channel 3. Connects to HX3 boards, organs or expander by MIDI cable. MIDI output jack 5-pin DIN. Could be placed inside MIDI pedal, size 170 x 50 mm. Power supply by MIDI phantom supply or 9V wall wart (SNT9).

To utilize the phantom supply, two jumpers on HX3 board must be set. Please see HX3 Mainboard Installation Manual.

Schematics to be found on http://updates.keyboardpartner.de/Files/index.php?dir=Schematics%20%28Schaltbilder

In case of Arduino IDE, you may have to copy *include* and *lib* directory contects into sketch directory. Anyway, we **highly recommend** using PlatformIO with VSCode for development!

Version Info:
 * Optimized for very small MCUs, no buffering for MIDI Tx
 * Very fast scanning with direct I/O
 * TODO: Changes for later use of ATtiny404 MCU,[env:attiny404] does not work yet
