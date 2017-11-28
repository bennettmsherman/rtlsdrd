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
If no parameter is specified "Master" is used. The second parameter specifies the audio output device, which is used when executing aplay. 
### With "Master" as the audio control controled by VOLUME and "hw:0,0" (Pi headphone output) as the output device:
```bash
(Build first)
./rtlsdrd
```

### With a specified audio control to be controlled by VOLUME:
* "hw:0,0" will be used as the default output device in this usage
```bash
(Build first)
./rtlsdrd <audio control name>
```

### With a specified audio control to be controlled by VOLUME and output device:
```bash
(Build first)
./rtlsdrd <audio control name> <device name>
```

The valid values which can be provided as the audio control name can be found by executing ```amixer scontrols```; the output will show a list of controls, such as 
```bash
bensherman@Pi3-Raspbian:~ $ amixer scontrols
Simple mixer control 'Master',0
Simple mixer control 'Capture',0
```
The control names to provide to rtlsdrd follow "simple mixer control".

For values which can be provided as the output device, execute ```aplay -L``` and ```aplay -l```. See [this page](https://en.wikibooks.org/wiki/Configuring_Sound_on_Linux/HW_Address) and [this page](https://superuser.com/questions/53957/what-do-alsa-devices-like-hw0-0-mean-how-do-i-figure-out-which-to-use) for more information. You can also configure pulse audio as an output source if you'd like to use bluetooth streaming.

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
