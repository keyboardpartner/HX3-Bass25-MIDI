
// #############################################################################
//
//     #     # ### ######  ###   ### #######
//     ##   ##  #  #     #  #     #  #     #
//     # # # #  #  #     #  #     #  #     #
//     #  #  #  #  #     #  #     #  #     #
//     #     #  #  #     #  #     #  #     #
//     #     #  #  #     #  #     #  #     #
//     #     # ### ######  ###   ### #######
//
// #############################################################################

#ifndef MIDI_IO_H
#define MIDI_IO_H

#include <Arduino.h>

uint8_t LastRunningStatusSent = 0xFF; // ungültiger Wert zum Initialisieren

void MidiTransmit(uint8_t data)
{
  /* Wait for empty transmit buffer */
  while ( !( UCSRA & (1<<UDRE)) )
  ;
  /* Put data into buffer, sends the data */
  UDR = data;
}

void MidiInit()
{
  /* Set baud rate */
  UBRRH = 0;
  UBRRL = 15; // 31250 Baud bei 8 MHz Takt, UBRRL = (F_CPU / 16 / BAUD) - 1, hier 15
  /* Enable transmitter */
  UCSRB = (1<<TXEN);
  /* Set frame format: 8 data, 1 stop bit */
  UCSRC = (3<<UCSZ0);
}


void MidiSendNoteOnNoDyn(uint8_t channel, uint8_t note) {
  uint8_t cmd_byte = 0x90 + channel - 1;
  if (cmd_byte != LastRunningStatusSent) {
    MidiTransmit(cmd_byte);
    LastRunningStatusSent = cmd_byte;
  }
  MidiTransmit(note);  // Key
  MidiTransmit(64);   // Dynamik
}

void MidiSendNoteOff(uint8_t channel, uint8_t note) {
  uint8_t cmd_byte = 0x80 + channel - 1;
  if (cmd_byte != LastRunningStatusSent) {
    MidiTransmit(cmd_byte);
    LastRunningStatusSent = cmd_byte;
  }
  MidiTransmit(note);  // Key
  MidiTransmit(64);   // Off-Dynamik
}

void MidiSendController(uint8_t channel, uint8_t cc, uint8_t value) {
  // CC senden
  uint8_t cmd_byte = 0xB0 + channel - 1;
  if (cmd_byte != LastRunningStatusSent) {
    MidiTransmit(cmd_byte);
    LastRunningStatusSent = cmd_byte;
  }
  MidiTransmit(cc);  // Controller number
  MidiTransmit(value & 0x7F);   // Wert
}

#endif