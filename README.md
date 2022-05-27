# PCA9685
This library is used for [Arducam Pan Tilt Camera for Raspberry Pi](https://www.arducam.com/docs/cameras-for-raspberry-pi/mini-pan-tilt-camera-kit/), as well as for the [Upgraded Camera Pan Tilt Platform for Raspberry Pi, Nvidia Jetson Nano/Xavier NX](https://www.arducam.com/product/arducam-pan-tilt-platform-for-raspberry-pi-camera-2-dof-bracket-kit-with-digital-servos-and-ptz-control-broad-b0283/).

On Raspberry Pi, make sure that ic2 support is enabled within the `raspi-config` tool, as explained [here](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c) in more details. It is required, so Python can find the board which drives the servos.
