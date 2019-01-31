# skateboard
This is the UDP branch. The skateboard receiver runs in AP mode and initiates a UDP server. It listens for incomming messages and expects them to arrive every 200ms.
This practically means that the remote controller keeps sending messages and can not put the modem to sleep mode, so keeps spending 80mA.
I tried disabling and enabling WiFi every 200ms, but then it gets disconnected and will not send UDP messages.
Will try a different approach.


