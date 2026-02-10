# HX3 Bass25 MIDI

![Bass25 Pic](https://github.com/keyboardpartner/HX3-Bass25-MIDI/blob/main/docs/Midibass25_m.JPG)

### Firmware for KeyboardPartner Bass25 MIDI Scan Controller

Based on ATtiny2313 MCU, scans two manuals and one 25-note bass pedal through a series of 4014 CMOS shift registers (8 for keybeds with 61 keys, 3 for bass pedals). Scan time with 8 MHz crystal (for low power consumtion) is around 360 µs, though using a delay of 2 to 4 ms for debounce is recommended.

Simultaneous scanning of manuals up to 61 keys and bass pedals up to 25 keys with single switches, contact to ground. Sends notes on channel 1 (Upper), 2 (Lower) and 3 (Pedal). Leave upper and lower inputs open (high level pullup) for bass pedal only.

**Note:** This low-cost design will not work with FATAR or verlocity-sensitive keybeds. If needed, use our **MIDI Scan controller** design.

Connects to HX3 boards, organs or expander by MIDI cable. MIDI output jack 5-pin DIN. Could be placed inside MIDI pedal, size 170 x 50 mm. Power supply by MIDI phantom supply or 9V wall wart (SNT9).
To utilize the phantom supply, two jumpers on HX3 board must be set. Please see HX3 Mainboard Installation Manual.

Schematics to be found on http://updates.keyboardpartner.de/Files/index.php?dir=Schematics%20%28Schaltbilder%29

In case of Arduino IDE, you may have to copy *include* and *lib* directory contects into sketch directory. Anyway, we **highly recommend** using PlatformIO with VSCode for development!

**Version Info:**
 * Optimized for very small MCUs, no buffering for MIDI Tx
 * Fast scanning with direct I/O
 * TODO: Changes for later use of ATtiny404 MCU,[env:attiny404] does not work yet

C. Meyer 11/2010 - 02/2026