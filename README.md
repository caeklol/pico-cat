# pico-cat

RGB cat lamp for sleeping \
meows over 440MHz FM

![demo](https://github.com/user-attachments/assets/5b7f2196-7eb1-4e1f-8a78-210b7394d0ce)

## usage
press belly to cycle through LED states \
if radio is enabled, long press the interact button to make the cat meow over FM \
there is warm white (default), cool white, and RGB

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

## configuration
configuration is done via [`src/config.h`](src/config.h) \
radio and debug (via serial) are disabled by default

## reception
these are my results with a subpar antenna[^1] about 65cm (25.5in~) away at max gain \
TX antenna is a helical antenna[^2]

<audio controls>
  <source src="https://github.com/user-attachments/assets/6af0176f-af62-49cb-9074-fbdc35f72698" type="audio/mpeg">
</audio>

[^1]: quarter wave groundplane from an N-type connector. not even tuned properly, as it's for 1090MHz ADS-B
[^2]: this antenna is also tuned improperly, for GSM bands. this should hopefully reduce it so signal is only audible in my space
