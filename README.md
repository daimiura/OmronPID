Table of Contents
- [1. Source code \& Contents](#1-source-code--contents)
- [2. branches](#2-branches)
  - [2.1. main](#21-main)
  - [2.2. 2023Qt6\_branch](#22-2023qt6_branch)
- [3. Getting started](#3-getting-started)
  - [3.1. Setting E5CC](#31-setting-e5cc)
    - [3.1.1. Input type](#311-input-type)
    - [3.1.2. To communicate](#312-to-communicate)
    - [3.1.3. Summary of setting items](#313-summary-of-setting-items)
  - [3.2. .exe user](#32-exe-user)
  - [3.3. Qt user](#33-qt-user)
- [4. OmronPID.exe](#4-omronpidexe)
# 1. Source code & Contents
[Original source code](https://github.com/TripletESR/Omron_PID) can be get.
```bash
git clone https://github.com/TripletESR/Omron_PID.git
```
The original souce is be abailable in Qt4 or 5.　Here, we develop applications with additional functions, mainly for safety. The source code added safety functions is available the following
```bash
git clone https://github.com/daimiura/OmronPID.git
```
The contents are 
- html
- latex
- release
- other files (*.cpp, *.h and so on)

The reference manuals for functions, etc. are generated by Doxygen (sorry for the little I have written). html, and latex directories are for the reference manuals.
The reference manual can be seen by opening
```bash
html/index.html
```
in your browser.

Omron_PID.exe, the application that controls E5CC, resides in the release directory. This directory also contains the DLL files necessary to run this .exe file.

# 2. branches
## 2.1. main
This branch is stable version.　It corresponds to Qt4 and 5 and was written in code tailored to Qt5.

## 2.2. 2023Qt6_branch
In Qt6, thisis not available because some functions are obsolute.　To use it in Qt6, this source code needs to be rewritten.　The update is developpoing in the 2023_Qt6 branch.　Now the contents in 2023_Qt6 include some issues, so  it is not available. 

# 3. Getting started
This section describes how to use the OmronPID.exe and install Qt.
## 3.1. Setting E5CC
### 3.1.1. Input type
This application is designed to use a K thermocouple as a thermometer probe (See that the [manual](manuals/e5cc_inst-5377443-6a.pdf) for thermocouple wiring and channels). <span style = "color:blue">Select the input type 6</span> (Default is 5. Reffer [manual for connection](manuals/e5cc_inst-5377443-6a.pdf) how to select input type).
### 3.1.2. To communicate
The setting items required for communication are as follows:
- Protocol selection : Mod
- Unit No. : 1
- bit per sec : 9.6
- Parity : None
- Send waiting time : 20

See [communication manual](manuals/communicationmanual.pdf) that how to set the parameters for communication.

### 3.1.3. Summary of setting items
The lists of setting items are summarized in the following table.
|Items|Sign|Default|to be set|
|---|---|---|---|
|Input Type|CN-t|5|6|
|Protocol selection|PSEL|CWF|Mod|
|Unit No.|U-No|1|1|
|bit per sec|bPS|9.6|9.6|
|Parity|PRty|EVEN|None|
|Send waiting time|SdWt|20|20|

## 3.2. .exe user
You can use OmronPID.exe if only you download the release directory. At first, connect E5CC to PC via USB cable. Then execute OmronPID.exe. If you have problems, it can be not exist dll files.he error messages show the dll files you need. After installing Qt5, copy the files from 
```bash
C/Qt/5.15.2/WinGW 8.10 64-bit/bin/
```
to the release directory. 


## 3.3. Qt user
Qt5 and 6 are available for free from [here](https://www.qt.io/download-qt-installer) (as of March 2023).　You need an account, so please create one.　You can register with your e-mail address.
For example, we will create a Qt directory under the C directory and install it there.
```bash
$ C/Qt/
```
ModBus communication is used with E5CC.
On the Component Selection page, install the components required for ModBus.
Qt Creator is useful for designing GUI, so Qt Creator should also be installed.　The required components are as follows (Qt 6 version is optional).
- Qt 6.5.0-rc
  - MinGW 11.2.0-64bit
  -  Qt 5 Compatibility Module
  -  Qt Shader Tools
  -  Additional Libraries
     -  Qt Serial Bus
     -  Qt Serial Port
- Qt 5.15.1
  - MinGW 8.1.0 64-bit
- Developper and Designer Tools
  - Qt Creator 9.0.2
  - Qt Creator 9.0.2 CDB Debugger Support
  - Debugging Tools for Windows
  - CMake 3.24.2
  - Ninja 1.10.2
- Qt Maintenance Tool

# 4. OmronPID.exe
