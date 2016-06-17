# armigOS

Bootloader, flash utility (via serial bus) and operating system for the Atmel ATmega328P. 

### Usage

Using armigOS requires the AVR-gcc-libraries to compile the sources. After flashing the binaries to the controller (e.g. with avrdude), the USART interface may be used to interact with the system. 

#### Flashing in the bootloader

When the controller is running the bootloader, it'll wait roughly 5 seconds before it starts its flashed program. If you press `p` in this timespan, you can paste an Intel hex file to the controller to flash it. 

### License

armigOS is released under the [GPLv3](LICENSE). 