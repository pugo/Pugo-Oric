# Pugo Oric

This project implements a basic Oric computer emulator with graphics, sound, etc.
All code is written from scratch.

It is my absolutely slowest project, just made out of love!

## Background

This is the result of a *very* long and low intensive hobby project I have worked on
since 2009, which currently is 14 years.

The purpose was to develop an emulator of the first computer I ever got,
as a christmas gift some 40 years ago. The computer was an Oric 1 and it
was fantastic back then!

There already is a brilliant emulator of Oric: Oricutron, which is more
complete than this project ever likely will be. The purpose of my emulator
project has been to learn more about how emulators work and to close the circle
by learning more about the computer I got as a kid.


## Timeline

As I work as a developer this has been a project I have gone back to for shorter
stints when normal work for some reasons have had less amount of coding. That means
that this project has been extremely slow. But that has always been a wonderful thing!
I have never felt any pressure to work on it and instead hacked on it by pure love.

The first thing I developed was naturally the MOS 6502 emulation. That took several
years of now-and-then coding. Then after some delay the MOS 6522 VIA (IO) chip work
started and took some more years. After that I added graphics support, keyboard
emulation, tape loading and lastly sound.

The timeline is something like:

* 2009: MOS 6502 CPU
* 2014: MOS 6522 VIA
* 2016: Graphics
* 2017: Key input
* 2020: Tape loading
* 2023: AY3-8912 sound


## Usage

The project should be possible to build on Linux using CMake.

```
$ mkdir build
$ cd build
$ cmake ..
$ make -j10
```
   
**Observe:** Currently the loaded tape iamge is hard coded in `machine.cpp`.


## Contribution

Since this is a pet project I work on only when i like to and because I feel that
I should implement all by myself I am reluctant to accept any contributions. But 
the licence is GPL v3, so feel free to fork it and do what you like as long as
you follow that license.

## License

All code is licensed with GPL v3.

---
Sweden, Linghem 2023, Anders Piniesjö
