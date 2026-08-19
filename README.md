# Akai S3000XL Editor

A desktop editor for the Akai S3000XL sampler, built with C++ and JUCE.

The goal of this project is to provide a modern graphical interface for
viewing and editing S3000XL programs, keygroups, velocity zones, and
sample headers through MIDI SysEx communication.

## Current Features

- MIDI communication with the Akai S3000XL
- Program list retrieval
- Sample list retrieval
- Program header retrieval and parsing
- Keygroup header retrieval and parsing
- Sample header retrieval and parsing
- Program / Keygroup / Velocity Zone editor UI
- Sample Header editor
- Sample name and sample ID resolution
- Editing velocity zone parameters
- Sending modified keygroup data back to the S3000XL
- Program tree displaying programs, keygroups, zones, and samples

## MIDI SysEx Support

The editor currently handles several S3000XL SysEx response types:

| Opcode | Description |
|--------|-------------|
| `0x03` | Program List |
| `0x05` | Sample List |
| `0x07` | Program Data / RPDATA |
| `0x09` | Keygroup Data |
| `0x16` | Command Reply |
| `0x28` | Program Header |
| `0x2A` | Keygroup Header |
| `0x2B` | Keygroup Data Response |
| `0x2C` | Sample Header |

Incoming SysEx messages are dispatched to dedicated response handlers
inside `MainComponent`.

## Architecture

The project is gradually being separated into several layers:

### MIDI

Handles communication with the sampler.

- `MidiManager`
- `SysExSender`

### S3000 Parsing

Handles decoding and parsing of S3000XL data structures.

- `ProgramParser`
- `KeygroupParser`
- `SampleHeaderParser`
- Offset definitions
- S3000 data types

### UI

Provides editors for the sampler data.

- `MainComponent`
- `ProgramTree`
- `ProgramEditor`
- `KeyGroupEditor`
- `VelocityZoneEditor`
- `SampleHeaderEditor`

## Development Status

This project is currently under active development.

The current focus is:

1. Refactoring MIDI SysEx handling out of `MainComponent`
2. Verifying S3000XL data offsets
3. Completing sample header editing
4. Sending edited parameters back to the sampler
5. Improving separation between MIDI, parsing, model, and UI layers

## Requirements

- macOS
- Xcode
- JUCE
- Akai S3000XL
- MIDI interface

## Build

The project is generated using Projucer.

Open the `.jucer` project in Projucer and generate the Xcode project.

Then open:

`Builds/MacOSX/s3000editor_producer.xcodeproj`

and build the project in Xcode.

## Hardware

Development and testing are currently performed with a real Akai S3000XL
connected over MIDI.

## Project Status

Experimental / Work in Progress.

SysEx parsing and editing behavior is still being verified against the
hardware.
