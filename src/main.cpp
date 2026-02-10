/*
// #############################################################################
//       __ ________  _____  ____  ___   ___  ___
//      / //_/ __/\ \/ / _ )/ __ \/ _ | / _ \/ _ \
//     / ,< / _/   \  / _  / /_/ / __ |/ , _/ // /
//    /_/|_/___/_  /_/____/\____/_/_|_/_/|_/____/
//      / _ \/ _ | / _ \/_  __/ |/ / __/ _ \
//     / ___/ __ |/ , _/ / / /    / _// , _/
//    /_/  /_/ |_/_/|_| /_/ /_/|_/___/_/|_|
//
// #############################################################################
*/
// MIDI Controller for FATAR keybeds, C. Meyer 1/2026
// Banner Logos from
// https://patorjk.com/software/taag/#p=display&f=Banner&t=MAIN&x=cppComment&v=4&h=2&w=80&we=false
// 20 MHz Bootloaders:
// https://github.com/MCUdude/MiniCore/tree/master/avr/bootloaders/optiboot_flash/bootloaders

#include <Arduino.h>

#include "midi_io_nobuf.h"
#define VERSION "Bass25 0.9"

#ifdef ATTINY2313_ENV
  #define LED_PIN PORTD2  // Pin für LED
  #define _LED_OFF asm volatile("sbi %0,%1 " : : "I" (_SFR_IO_ADDR(PORTD)), "I" (LED_PIN))
  #define _LED_ON  asm volatile("cbi %0,%1 " : : "I" (_SFR_IO_ADDR(PORTD)), "I" (LED_PIN))
  // BASS und SR61 4014 Scan-Controller Pinbelegung
  #define SR_CLK   PORTB0
  #define SR_LOAD  PORTB1
  #define SR_UPR   PINB2
  #define SR_LWR   PINB3
  #define SR_PED   PINB4
  #define SR_INPORT PINB
  // Fast port bit manipulation Macros
  #define _SET_SR_CLK   asm volatile("sbi %0,%1 " : : "I" (_SFR_IO_ADDR(PORTB)), "I" (SR_CLK))
  #define _CLR_SR_CLK   asm volatile("cbi %0,%1 " : : "I" (_SFR_IO_ADDR(PORTB)), "I" (SR_CLK))
  #define _SET_SR_LOAD  asm volatile("sbi %0,%1 " : : "I" (_SFR_IO_ADDR(PORTB)), "I" (SR_LOAD))
  #define _CLR_SR_LOAD  asm volatile("cbi %0,%1 " : : "I" (_SFR_IO_ADDR(PORTB)), "I" (SR_LOAD))
#endif

#define _NOP_DLY asm volatile ("nop")

#define PEDALKEYS 25
#define MANUALKEYS 61
#define MIDI_BASE_UPR 36
#define MIDI_BASE_LWR 36
#define MIDI_BASE_PED 36
#define MIDI_CHANNEL_UPR 1
#define MIDI_CHANNEL_LWR 2
#define MIDI_CHANNEL_PED 3

uint8_t ContactStates[MANUALKEYS]; // Zustand der Pedal- und Manualtasten in einzelnen Bits
uint8_t Channel = 0;


// #############################################################################
//
//     ######  ####### ######     #    #
//     #     # #       #     #   # #   #
//     #     # #       #     #  #   #  #
//     ######  #####   #     # #     # #
//     #       #       #     # ####### #
//     #       #       #     # #     # #
//     #       ####### ######  #     # #######
//
// #############################################################################

void ScanPedal() {
  // Pedal mit BASS25 scannen, Zeit für 25 Pedaltasten etwa 25 us bei 20 MHz Takt
  uint8_t scankey, mk_ped, mk_old; // aktuelle Taste
  _SET_SR_LOAD;
  _NOP_DLY;
  _SET_SR_CLK;
  _NOP_DLY;
  _CLR_SR_CLK;
  _CLR_SR_LOAD; // Load LOW
  for (scankey = 0; scankey < PEDALKEYS; scankey++) {
    mk_ped = SR_INPORT & (1 << SR_PED); // Make-Kontakt Pedal lesen, active LOW
    // Pedal hat nur Make-Kontakt, deshalb keine State Machine
    mk_old = ContactStates[scankey] & (1 << SR_PED);
    if (mk_ped != mk_old) {
      // Zustand hat sich geändert
      ContactStates[scankey] &= ~(1 << SR_PED); // alten Zustand löschen
      ContactStates[scankey] |= mk_ped;
      if (mk_ped == 0) {
        // Pedal gedrückt, NoteOn mit fester Dynamik
        MidiSendNoteOnNoDyn(MIDI_CHANNEL_PED + Channel, MIDI_BASE_PED + scankey);
        _LED_ON;
      } else {
        // Pedal losgelassen
        MidiSendNoteOff(MIDI_CHANNEL_PED + Channel, MIDI_BASE_PED + scankey);
        _LED_OFF;
      }
    }
    _SET_SR_CLK;
    _NOP_DLY;
    _CLR_SR_CLK;
  }
}

// #############################################################################

void ScanManualsSR61() {
  // Manual mit 4014 SR scannen, Zeit für 61 Manualtasten etwa 81 us bei 20 MHz Takt
  uint8_t scankey; // aktuelle Taste
  uint8_t mk, mk_old;
  _SET_SR_LOAD;
  _NOP_DLY;
  _SET_SR_CLK;
  _NOP_DLY;
  _CLR_SR_CLK;
  _CLR_SR_LOAD; // Load LOW
  for (scankey = 0; scankey < MANUALKEYS; scankey++) {
    mk = SR_INPORT & (1 << SR_UPR); // Make-Kontakt Taste lesen, active LOW
    // Manual hat nur Make-Kontakt, deshalb keine State Machine
    mk_old = ContactStates[scankey] & (1 << SR_UPR);
    if (mk != mk_old) {
      // Zustand hat sich geändert
      ContactStates[scankey] &= ~(1 << SR_UPR);
      ContactStates[scankey] |= mk;
      if (mk == 0) {
        // Taste gedrückt
        MidiSendNoteOnNoDyn(MIDI_CHANNEL_UPR + Channel, MIDI_BASE_UPR + scankey); // Upper NoteOn mit fester Dynamik
      } else {
        // Taste losgelassen
        MidiSendNoteOff(MIDI_CHANNEL_UPR + Channel, MIDI_BASE_UPR + scankey); // Upper NoteOff
      }
    }
    mk = PINB & (1 << SR_LWR); // Make-Kontakt Taste lesen, active LOW
    // Manual hat nur Make-Kontakt, deshalb keine State Machine
    mk_old = ContactStates[scankey] & (1 << SR_LWR);
    if (mk != mk_old) {
      // Zustand hat sich geändert
      ContactStates[scankey] &= ~(1 << SR_LWR);
      ContactStates[scankey] |= mk;
      if (mk == 0) {
        // Taste gedrückt
        MidiSendNoteOnNoDyn(MIDI_CHANNEL_LWR + Channel, MIDI_BASE_LWR + scankey); // Lower NoteOn mit fester Dynamik
      } else {
        // Taste losgelassen
        MidiSendNoteOff(MIDI_CHANNEL_LWR + Channel, MIDI_BASE_LWR + scankey); // Lower NoteOff
      }
    }
    _SET_SR_CLK;
    _NOP_DLY;
    _CLR_SR_CLK;
  }
}


// #############################################################################


void configurePorts() {
  // SR61 Scan Controller
  DDRB =  B00000011; // PB0..PB1 als Ausgänge
  PORTB = B00111000; // Pull-ups für SR61- und BASS25-Eingänge aktivieren
  DDRD =  B00000110; // Nur Txd/Rxd und LED benutzt
  PORTD = B11111110; // Pull-ups für Eingänge aktivieren, LED PD2 off (high!)

  // Initialisierung der Pedal-Kontaktzustände
  for (uint8_t i = 0; i < PEDALKEYS; i++) {
    ContactStates[i] = (1 << SR_PED);
  }
}


void blinkLED(uint8_t times) {
  // Board-LED blinkt zur Bestätigung von Aktionen, z.B. Speichern von Werten im EEPROM
  for (uint8_t i=0; i < times; i++) {
    _LED_ON;
    delay(200);
    _LED_OFF;
    delay(200);
  }
}

// #############################################################################
//
//     #     #    #    ### #     #
//     ##   ##   # #    #  ##    #
//     # # # #  #   #   #  # #   #
//     #  #  # #     #  #  #  #  #
//     #     # #######  #  #   # #
//     #     # #     #  #  #    ##
//     #     # #     # ### #     #
//
// #############################################################################


// ------------------------------------------------------------------------------

void setup() {
  configurePorts(); // Port Initialisierung je nach Treibertyp
  MidiInit();
  Channel = 0; // MIDI-Kanal 3 entspricht Jumper-Einstellung 2 (0..7 für Kanal 1..8)
  //Jumper (negative Logik) an PD3 bis PD5 lesen, um MIDI-Kanal zu bestimmen, 0..7 entspricht Kanal 1..8
  if (!(PIND & (1 << PORTD3))) Channel = 1; // Bit 0
  if (!(PIND & (1 << PORTD4))) Channel += 2; // Bit 1
  if (!(PIND & (1 << PORTD5))) Channel += 4; // Bit 2
  MidiSendController(MIDI_CHANNEL_UPR + Channel, 123, 64); // All Notes Off on selected Channel
  MidiSendController(MIDI_CHANNEL_LWR + Channel, 123, 64); // All Notes Off on selected Channel
  MidiSendController(MIDI_CHANNEL_PED + Channel, 123, 64); // All Notes Off on selected Channel
  blinkLED(MIDI_CHANNEL_PED + Channel); // blinkt die gewählte Kanalnummer, 0..7 + 1 = MIDI-Kanal 1..8
 }

// #############################################################################

void loop() {
  ScanPedal();    // 60 us bei 8 MHz
  ScanManualsSR61() ; // 80 us bei 8 MHz
  delay(2); // Entprellzeit
}

