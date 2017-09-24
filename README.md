# What is this?
This project is a wrapper for the rtl_fm demodulator. It sets up a TCP server (by default on TCP 2832) which can can be connected to by any client which supports the command set (the full set will be uploaded here later).

## Dependencies:

### __rtl-sdr__
From my repo - https://github.com/bennettmsherman/rtl-sdr (see the README for installation instructions)

### __g++ and make__
```bash
sudo apt-get install g++ make
```

### __boost C++ libs__ (threading and networking)
Tested on Debian-based distros; install with:
```bash
sudo apt-get install libboost-all-dev
```

### __aplay__ (for audio output)
```bash
sudo apt-get install alsa-utils
```

## To build:
```bash
cd rtlsdrd/Build
make
```

## To clean:
```bash
cd rtlsdrd/Build
make clean
```

## To run:
```bash
(Build first)
./rtlsdrd
HELP shows available commands
```
