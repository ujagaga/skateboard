# DIY Electric Skateboard With Hoverboard Wheels

This is the 3D model for my DIY electric skateboard. It features two howerboard motor whells and 2 passive wheels. 
The driver is from AliExpress, "36V Brushless Dual Motor Electric Skateboard Controller with remote controller":

https://www.aliexpress.com/item/1005004489103885.html?spm=a2g0o.order_list.order_list_main.116.c24d18024PqocZ

The heatsink on the original driver is small and the first electronics I used burned up, so I installed a larger heatsink.

The battery I am using is from a hoverboard as those are the cheapest ones available to me. 
To get more range, I connected two batteries in paralell.

The charge port is for Xiaomi electric scooter, so I am re-using the charger I already have.

# How to
- The screws I used are standard self curving screws for wood 3x30mm

# NOTE
I did build one using my own code for ESP8266 WiFi modules. There was a remote controller and controll circuit for a motor driver
I bought online. The whole thing worked well, but used a 90mm hub motor wheel. The bicycle tracks in my town are not that great, 
so I decided to build a new one with two howerboard wheels. The whole electronics is so cheap that it really makes no sence to develop it myself,
which is why I dropped the development and just used the factory made stuff.

Downside:
1. The LED light port is very small and I could not find a connector for it and no documentation or a module to buy to connect. 
Will use the battery indicator wires to trigger the lights.
3. The HALL sensors are not very well tuned so I disconnected them entirely. It works well without them. 

# Contact
ujagaga@gmail.com
