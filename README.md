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
The first parameter of rtlsdrd is the name of the audio control which the VOLUME command will control.
If no parameter is specified "Master" is used.
### With "Master" as the audio control controled by VOLUME:
```bash
(Build first)
./rtlsdrd
```

### With a specified audio control to be controlled by VOLUME:
```bash
(Build first)
./rtlsdrd <audio control name>
```
## To connect to the daemon
### If the daemon is running on the same machine
Run:
```bash
telnet localhost 2832
```
### If the daemon is running on a different machine
Run:
```bash
telnet <IP of machine running daemon> 2832
```

### Once you're connected to the daemon:
The **HELP** command shows the commands available to use
