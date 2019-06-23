# Experiments with the Meross MSS310 Smart Plug

This repository contains code to experiment with the Meross MSS310
smart plug. The plug contains a daughter board with a Mediatek MT7688KN
SoC. The device boots into u-boot, which then boots the real application
which uses the eCos operating system.

The initial scaffold for the code in this repository has been taken
from Nicholas FitzRoy-Dale’s [bare-metal hello world][1]. It is used to
determine how to run my own software on the device and only serves as
proof-of-concept. The real application will get its own repository,
some day..., perhaps... .

# Running the blink application

- Get a cross compiler for MIPS. Or simply build one yourself using
  `crosstool-ng`. Run `make`. You probably need to change the path to
  the cross compiler in the `Makefile`.

- Open the plug and solder four wires to the CPU board (RX, TX, GND,
  3V3) and connect an USB-to-serial port to RX, TX and GND. Use your
  preferred terminal program to connect to the serial port. Parameters
  are 57600,8N1.

- Do **NOT** plug the device into a power outlet! Instead, connect
  an external power-supply, 3.3V capable of delivering 500mA. While
  the board draws less than 200mA when running, it doesn't boot if
  the power supply's current limit is set to 200mA.

- Hit `4` immediately when then device boots. This should drop you on
  the u-boot command line.

- Enter `loadb 1000000` in the serial console, then disconnect your
  terminal.

- From the host's command line, run `kermit -s blink.bin` to upload
  the compiled binary.

- Connect to the MSS10 via the terminal program again. The `loadb`
  command should have terminated (you might need to hit the `Return`
  key). Enter `go 1000000` and the LEDs on the device should start
  cycling through all color combinations. Pressing the button should
  reboot the device.


[1]: https://github.com/nfd/ci20-hello-world