#! /usr/bin/env python

""" Standard MIDI Format Decoder.

Prototype of a reader of MIDI music files for the LO52 project."""

import math, sys
import struct

MIDI_F0 = 16.35
MIDI_HEADER_SIZE = 14
MIDI_TRACK_HEADER_SIZE = 8
MIDI_EVENT_SYSEX1 = 0xF0
MIDI_EVENT_SYSEX2 = 0xF7
MIDI_EVENT_META = 0xFF

MIDI_EVENT_PITCH_CHANGE = 0xE
MIDI_EVENT_CHANNEL_AFTER_TOUCH = 0xD
MIDI_EVENT_PATCH_CHANGE = 0xC
MIDI_EVENT_CONTROL_CHANGE = 0xB
MIDI_EVENT_KEY_AFTER_TOUCH = 0xA
MIDI_EVENT_NOTE_ON = 0x9
MIDI_EVENT_NOTE_OFF = 0x8


def get_freq_from_tone(tone_offset):
    return MIDI_F0 * math.pow(math.pow(2, 1./12.), float(tone_offset))

def parse_midi_header(fd):
    fd.seek(0)
    raw_headers = fd.read(MIDI_HEADER_SIZE)
    print raw_headers
    data =  struct.unpack('!4s' # File Format
                          'i'   # Rest of header Size, should almays be 6
                          'h'   # Subformat
                          'h'   # Number of tracks
                          'h'   # music speed
                          , raw_headers)
    headers = dict()
    headers['Chunk'] = data[0]
    headers['RemHeadSize'] = data[1]
    headers['Subformat'] = data[2]
    headers['NbTracks'] = data[3]
    headers['Speed'] = data[4]
    return headers

def parse_track_headers(fd):
    raw_headers = fd.read(MIDI_TRACK_HEADER_SIZE)
    data = struct.unpack('!4s' # Start of track flag
                         'i'   # Number of bytes remaining in the track
                               # Track data + Track out
                         , raw_headers)
    headers = dict()
    headers['Chunk'] = data[0]
    headers['TrackSize'] = data[1]

    return headers

def handle_meta_event(data):
    meta_type, data = struct.unpack('!B%ds' % (len(data) - 1), data)
    length, data = struct.unpack('!B%ds' % (len(data) - 1), data)
    event_data, data = struct.unpack('!%ds%ds' % (length,
        len(data) - length), data)
    return data

def handle_sysex_event(data):
    length, data = struct.unpack('!B%ds' % (len(data) - 1), data)
    event_data, data = struct.unpack('!%ds%ds' % (length,
        len(data) - length), data)
    return data

def handle_midi_event(event, data):
    channel = event & 0b1111
    event = (event & 0b11110000) >> 4
    if event ==  MIDI_EVENT_PITCH_CHANGE:
        bottom, top, data = struct.unpack('!BB%ds' % (len(data) - 2),
                data)
    elif event == MIDI_EVENT_CHANNEL_AFTER_TOUCH:
        channel_nb, data = struct.unpack('!B%ds' % (len(data) - 1), data)
    elif event == MIDI_EVENT_PATCH_CHANGE:
        program, data = struct.unpack('!B%ds' % (len(data) - 1), data)
    elif event == MIDI_EVENT_CONTROL_CHANGE:
        ctrler_nb, value, data = struct.unpack('!BB%ds' % (len(data) - 2),
                data)
    elif event == MIDI_EVENT_KEY_AFTER_TOUCH:
        note_nb, velocity, data = struct.unpack('!BB%ds' % (len(data) - 2),
                data)
    elif event == MIDI_EVENT_NOTE_ON:
        note_nb, velocity, data = struct.unpack('!BB%ds' % (len(data) - 2),
                data)
    elif event == MIDI_EVENT_NOTE_OFF:
        note_nb, velocity, data = struct.unpack('!BB%ds' % (len(data) - 2),
                data)

    return data

def parse_track_data(fd, track_size):
    data = fd.read(track_size)
    while data:
        delta, data = struct.unpack('!B%ds' % (len(data) - 1), data)
        delta_time = delta
        while delta > 0x7F:
            delta, data = struct.unpack('!B%ds' % (len(data) - 1), data)
            delta_time += delta

        print 'Delta time : %d' % delta_time
        event, data = struct.unpack('!B%ds' % (len(data) - 1), data)
        print 'Event : 0x%X' % event

        if event == MIDI_EVENT_META:
            if data.startswith('\x2f\x00'):
                print 'Found End of Track ...'
                break
            else:
                data = handle_meta_event(data)
        elif event == MIDI_EVENT_SYSEX1 or event == MIDI_EVENT_SYSEX2:
            data = handle_sysex_event(data)
        else:
            data = handle_midi_event(event, data)

if __name__ == '__main__':
    midi_file = open(sys.argv[1])
    file_headers = parse_midi_header(midi_file)
    print file_headers

    if not (file_headers['Chunk'] == 'MThd' and
            file_headers['RemHeadSize'] == 6):
        raise IOError('The given file does not follow Standard Midi Files'
                      ' specifications.')

    for track in range(file_headers['NbTracks']):
        track_headers = parse_track_headers(midi_file)
        track_data = parse_track_data(midi_file, track_headers['TrackSize'])

