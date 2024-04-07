# skateboard

This is the setup of my DIY skateboard. It features one brushless motor whell drive and 3 passive wheels. 
The driver is from AliExpress, "BLDC 3 Phase 400W DC Hall Brushless Motor Controller PWM Hall Motor Control Driver Board With Forward/Reverse/Brake DC 6V-60V":

https://www.aliexpress.com/item/1005002287420689.html?spm=a2g0o.order_list.order_list_main.15.7d0f18026H4ZWR

The heatsink on the original driver is small and the first electronics I used burned up, so I installed a larger heatsink.

The remote command module is an esp8266 based development board, hosting a web server, so you can controll the skateboard using any smart phone web browser.

There is also a remote controller which also uses esp8266 to act as a web sockets client and send command to the server.

The battery I am using is from a hoverboard as those are the cheapest ones available to me. The wheels are 90mm in diameter, so there is sufficient space under the board for the battery box buit from a small lunch box. The rest of the electronics is encased in a 3D printed case.

## NOTE

The current implementation uses a GPIO pin PWM to command the speed. The voltage is up to 3V3. The driver expects up to 5V, so it does not reach maximum speed. If you want more speed, use the schematic "ESP12_ctrl_high_speed.png" 
and uncomment in config.h "#define REVERSE_SPEED_POLARITY"  

## Some references:

https://www.losant.com/blog/making-the-esp8266-low-powered-with-deep-sleep

Pins D0 and D6 remain high during bootloader mode.
