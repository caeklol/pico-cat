# pico-cat

cat lamp for sleeping \
goofy light features \
meows over 440MHz FM

> [!CAUTION]
> Please don't enable radio features if you don't know what you're doing! \
> 440MHz is a licensed band in so get one first before proceeding

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
these are my results with a subpar antenna[^1] about 65cm (25.5in~) away at max gain \
TX antenna is a helical antenna[^2]

[^1]: quarter wave groundplane from an N-type connector. not even tuned properly, as it's for 1090MHz ADS-B
[^2]: this antenna is also tuned improperly, for GSM bands. this should hopefully reduce it so signal is only audible in my space
