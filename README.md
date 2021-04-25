# arduino_morse
Recognize Morse code using Arduino

## Hardware setup

* Arduino Pro Mini: use 3.3V variant if you want to connect Nokia 5110 display directly. ATmega168 is enough, as the flash image is small
* Nokia 5110 display with PCD8544 controller built in
* Button and 10kOhm resistor

## Morse code recognition

The code uses rather naive approach - everything shorter than 150ms is considered a dot, everything longer a dash.
Pause of more than 500ms marks the end of a single character. Of course, you are free to adapt these values to suit
your typing speed. A better approach would be to somehow adapt to the typing speed of the user, for example by asking
them to type one letter first.

## Further ideas

* Dynamically determine the thresholds
* Use microphone as an input device instead of a button
