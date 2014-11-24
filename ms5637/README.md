# Team Impulse 2014
## CanSat competition
## MS5637 (barometer and temperature sensor) code

This code is standalone code to drive an MS5637. It has been successfully tested on a Teensy 3.1, using 10K pullups on SDA & SCL as specified in the datasheet.

This code has been written based on the [publicly available MS5637 datasheet.](http://www.meas-spec.com/downloads/MS5637-02BA03.pdf)

If you use this code in another context (including porting it to another platform), please be aware that the type casting is a bit fiddly - one has to be careful to cast everything into the correct data type for both pressure and temperature in order to avoid overflows. This should not be affected by the code context provided that the variable declarations are maintained as-is; should you run into difficulty, try amending these first.

If you write code based on this, please be aware that the MS5637 PROM register at 0xA0 does *not* appear to contain a calibration constant, but instead holds what we believe is a device ID. The calibration constants we are using successfully begin at 0xA2 and proceed to 0xA12/14.
