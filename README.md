# SAMMS v2.00
The Speech Assissting Multi-Microphone System (SAMMS) gives the wearer haptic feedback if they are speaking too quietly. The user can set the volume decibel threshold (WIP). The system outputs an excel file with speech amplitude data to the SD card.

## Hardware (WIP)
SAMMS uses the Sony Spresense board with a CXD5602 micro-processor.
Accelerometer: AXDL343

## Running SAMMS
Setup your Arduino library for Spresense using the [Spresense Arduino Library Getting Started Guide](https://developer.sony.com/develop/spresense/docs/arduino_set_up_en.html)

Pull down the github repository. It will pull down the develop branch.
`git clone https://github.com/orjioke33/sammsv2.git`
for SSH: `git clone git@github.com:orjioke33/sammsv2.git`

Open `subcore/SubCore.ino` in the Arduino IDE. On the top bar, click Tools > Core > SubCore 1. If the Spresense board is not plugged in already, plug it in. Upload the sketch to the Spresense board.

Open `samms_main/samms_main.ino` in the Arduino IDE. On the top bar, click Tools > Core > MainCore. Click Tools > Serial Monitor and change the baud rate to `115200`. Upload the sketch to the Spresense board. If you see `"Init Audio Library"` on the Serial Monitor without errors, it is working properly!