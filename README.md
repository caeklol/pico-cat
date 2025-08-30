# pico-cat

cat lamp for sleeping \
goofy light features \
meows over 440MHz FM

### hardware

##### requirements
- 2 pushbuttons
- 1 RGB LED (common cathode)
- 1 antenna

##### setup
antenna at GP7. i use a helical antenna \
RGB LED is GP1, GP2, GP3 respectively \
interact button is wired to GP4 \
toggle button is on GP10 \
active high for buttons, using builtin pulldown

##### usage
long press the interact button to make the cat meow over FM \
short press to cycle through LED states

### reception
i got best results on my RTL-SDR at 25kHz with 50us de-emphasis
