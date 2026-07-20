# ROMS

The emulator by defailt looks for ROM files in this directory.

The directory path and the ROM file names can be configured by settings in the `auric.yaml` configuration file.
See below for more details.

The following ROM files are used for full emulation of the Oric 1 and Oric Atmos computers, with disk drive support.

## ROM files

Auric has successfully been tested with the following ROM files. Other ROM files might work, but some features
such as cassette tape turbo loading might not be enabled for other ROM files.

| ROM file name  | SHA1                                       | Description                     |
|----------------|--------------------------------------------|---------------------------------|
| `basic10.rom`  | `333116e6884d85aaa4dfc7578a91cceeea66d016` | Oric 1 ROM (for Oric 1 mode)    |
| `basic11b.rom` | `9451a1a09d8f75944dbd6f91193fc360f1de80ac` | Oric Atmos ROM (for Atmos mode) |
| `microdis.rom` | `0d2ef6e67322f48f4b7e08d8bbe68827e2074561` | Microdisc disc drive ROM        |


## Configuration

Auric uses the configuration file `auric.yaml` to configure the emulator. The ROM files are configured
in the `roms` section.

```yaml
roms:
  # Path to the ROMS directory.
  roms_directory: ./ROMS

  # Names of the ROM files.
  file_names:
    oric_1: basic10.rom
    oric_atmos: basic11b.rom
    microdisk: microdis.rom
```

Auric tries to load the `auric.yaml` configuration file from the current working directory. 
The configuration file path can be set on the command line using the `--config` or `-c` options.
