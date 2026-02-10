# HX3-Bass25-MIDI
### Firmware for KeyboardPartner Bass25 MIDI Scan Controller, based on ATtiny2313 MCU

Scans a 25-note bass pedal through a series of 4014 CMOS shift registers. Scan time is around 60 µs, but using a delay of 5 ms for debounce.

Schematics to be found on http://updates.keyboardpartner.de/Files/index.php?dir=Schematics%20%28Schaltbilder%29/Bass25

In case of Arduino IDE, you may have to copy *include* and *lib* directory contects into sketch directory. Anyway, we **highly recommend** using PlatformIO with VSCode for development!

Version Info:
 * Optimized for very small MCUs, no buffering for MIDI Tx
 * Very fast scanning with direct I/O
 * TODO: Changes for later use of ATtiny404 MCU,[env:attiny404] does not work yet
