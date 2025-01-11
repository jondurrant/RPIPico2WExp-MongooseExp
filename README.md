# RPIPico2WExp-MongooseExp
Experiments with Mongoose on the Raspberry Pi Pico 2 W

Mongoose is a WebServer library for embedded and non embedded use.

## Examples

### HelloWorld
Just respond to any request with a JSON messahe saying "Hello World"

### LEDControl
Control a stack of WS2812B LED on GPIO15 whith the URL
http://<IP>/api/led?color=rgbhex string
Example for blue
http://<IP>/api/led?color=0000ff

Also there is a HTML 5 color picker to set things at http://<IP>/


### LEDControlLWIP
Same functionality as LEDControl but using LWIP and FreeRTOS Kernel rather than the native Mongoose IP stack

### LEDAnim
If we are using FreeRTOS Kernel we might as well show some parallel functionality.

