# pico-cat

cat lamp for sleeping \
goofy light features \
meows over 440MHz FM

## hardware

#### requirements
- 2 pushbuttons
- 1 RGB LED (common cathode)
- 1 antenna

#### setup
antenna at GP7. i use a helical antenna \
RGB LED is GP1, GP2, GP3 respectively \
interact button is wired to GP4 \
toggle button is on GP10 \
active high for buttons, using builtin pulldown

#### usage
press to cycle through LED states \
if radio is enabled, long press the interact button to make the cat meow over FM \
there is warm white (default), cool white, and RGB

## configuration
configuration is done via [`src/config.h`](src/config.h) \
radio and debug (via serial) are disabled by default

## reception
i got best results on my RTL-SDR at 25kHz with 50us de-emphasis
