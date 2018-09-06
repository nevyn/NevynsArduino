# Nevyn's Bike Lights v2

Front and rear light strips using NeoPixels, with left and right turning indicator
animations.

![visual](visual.png)

## Wiring

![wires](wires.png)

Need to update this diagram because it's mostly wrong :P Changes:

* Buttons go to ground, not 5v
* there is no center button
* buttons go to 7, 8, 9, 10 (left, right, down, up)
* Front LEDs are on pin 5
* Rear LEDS are on pin 6
* Lights go through 5v, not Vin
* there's about 500ohm resistor between lights and their data pins (5 and 6)

Haven't decided how to power this yet... Plan 1 is a 9v rechargeable battery. Plan 2 is a very
small motorcycle battery.

### Buttons

Voxar's buttons have these wires:

* striped: ground
* blue: down/stop (pin 9)
* green: right (pin 8)
* brown: front (pin 10)
* orage: left (pin 7)