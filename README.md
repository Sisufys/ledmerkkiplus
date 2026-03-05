Improved firmware for the Sähköinsinöörikilta ry's led overall patch. Uses an ISR to handle multiplexing and brightness changing with PWM to make creating new animations easier.
Leds are controlled using a function setLed(index, brightness) and the ISR runs in the background making implementing animation logic straightforward.
