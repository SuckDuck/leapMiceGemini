# Leap Mice
A simple and minimalistic solution for using a Leap Motion 
controller as a mouse on Linux with X11, intended for use 
on kiosk stations. It is compatible with both V1 and V2 
controllers via the V5 Gemini SDK. It has the following features:
- Mouse movement controlled by moving the right hand around with 
the index finger extended.
- Left-click triggered by quickly moving the hand forward and 
backward by default.
- Left-click triggered by pinch gesture when using the `-p` flag
- Easy area calibration process using the `-c` flag when running 
the command.

Currently, right-click and scrolling are not implemented, as the 
device's insufficient precision leads to false positives. But it could
be implemented on the future if I found a decent solution.

## Requirements
- The Leap Motion V5 SDK and service. Official guide [here](https://docs.ultraleap.com/linux/)
- [`xdotool`](https://github.com/jordansissel/xdotool) by Jordan Sissel.

## Make and Usage
Ensure both required packages are installed on your system, then run `sudo make install` 
in the project directory. The compiled binary will be placed in `/usr/local/bin` as `leapMice` by default.
Feel free to change the install directory and binary name by editing the [Makefile](Makefile)

Once the binary is compiled, run `leapMice -c` and follow the instructions to create the 
calibration file, which will be located at `~/.leapMice/calibration` and will be automatically 
updated each time the calibration command is executed.

Then just run `leapMice` and start using your Leap Motion controller as a mouse. 
Here are some demostration videos:

![Push Mode demonstration](media/push_example.gif) \
![Pinch Mode demonstration](media/pinch_example.gif) \
[Config demonstration](https://youtu.be/mLm4AJPfc7Q) \
*it was really hard to film this last one, so sorry for the quality*

Both binary and calibration file would be deleted when running `sudo make uninstall`

## Notes
The project currently has limited functionality, but it is probably the simplest and 
cleanest Leap Motion-based mouse available, making the source code quite accessible 
for editing. However, I hope to add more features in the near future.

Also, the [`leapTemplate`](src/leapTemplate.c) and [`utils`](src/utils.c) files contain 
some useful functions for working with Leap Motion data, such as vector operations, 
which could be used as a starting point for developing other Leap Motion-based applications.