
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

### As of 1/17/18, rtlsdrd now contains an argument parser
In the previous versions, the audio control name and output device were positional arguments. As of this update, 
standard linux command line arguments can be specified using "-" and "--". To see the full list of arguments supported,
run
```bash
./rtlsdrd -h
```
As of 1/17/18, the supported args are:
```
SHORT(-)  EXTENDED(--)  TAKES PARAM  DESCRIPTION                                                                                                                        
--------  ------------  -----------  -----------                                                                                                                        
    h     help              no       Prints the arguments supported by this program                                                                                     
    a     audiocontrol      yes      Sets the audio control name used for volume control; see output of 'amixer scontrols'. Default: "Master"                           
    o     outputdevice      yes      Sets the ALSA name/address of the device to output the audio stream to; see 'aplay -L' and 'aplay -l'. Default: "plughw:0,0"                               
    p     password          yes      Sets the password which clients must provide to use this server. Default: no password                                                                      
    P     port              yes      Set the TCP port number of this rtlsdrd server. Default: 2832         
```
Note that the defaults for each argument are specified at the end of their description.

#### General Argument Usage
To use arguments with rtlsdrd, reference the supported arguments in the table above. Short arguments are a single character, and are prefixed with a "-", such as "-a". Extended arguments are more verbose, and are prefixed with a "--", such as "--audiocontrol". The "TAKES PARAM" category indicates whether each argument requires a parameter. If "yes" is specified, then a parameter must be provided with the given argument, such as "-a Master" or "--outputdevice Pulse". If "no" is specified, than no parameter should be given for the argument. 

### With a specified audio control to be controlled by VOLUME and output device:
```bash
(Build first)
./rtlsdrd -a <audio control name> -o <device name>
```

The valid values which can be provided as the audio control name can be found by executing ```amixer scontrols```; the output will show a list of controls, such as 
```bash
bensherman@Pi3-Raspbian:~ $ amixer scontrols
Simple mixer control 'Master',0
Simple mixer control 'Capture',0
```
The control names to provide to rtlsdrd follow "simple mixer control".

For values which can be provided as the output device, execute ```aplay -L``` and ```aplay -l```. See [this page](https://en.wikibooks.org/wiki/Configuring_Sound_on_Linux/HW_Address) and [this page](https://superuser.com/questions/53957/what-do-alsa-devices-like-hw0-0-mean-how-do-i-figure-out-which-to-use) for more information. You can also configure pulse audio as an output source if you'd like to use bluetooth streaming.

### Password Usage
As of 1/17/18, rtlsdrd supports password protection. The password is to be provided at runtime with the argument "-p" or "--password". The password must not start with a "-" or "--". At this time, the control applications do not support authentication, although this functionality will be implemented in the near future. The password is required when a client first connects to rtlsdrd. If an invalid password is provided, the connection is dropped. If the user-provided password is correct, full control to rtlsdrd is granted. 

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
