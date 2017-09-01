# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program

---
The goal of this project is develop a PID controller to drive a car through the track of the udacity simulator, with minimum oscillation and without getting off-track

To achieve the goal first I develop the pid controller in C++, using the cross track error [cte] (difference between the center of the car and the current position of the vehicle), and multiplying it with a constant Kp, then the derivative of the cte is multiplyied with a constant Kd and the same for the accumulative sum of cte which is multiplied by ki

therefore the output of the actuator in this case the steering wheel is obtained from the formula
actuator = -(kp*cte + ki*sum(cte) + kd*d(cte))

to obtain the actual parameters, a tunning algorithm is necessary

to tune the algorithm I used the Twiddle algorithm suggested in the classroom, the Twiddle logic was written in python and communicated with binary executable written in c++ through the commandline pipeline.

The twiddle algorithm just changes up and down each parameter with a small detla and evaluates the result and then changes the next parameter and so on.

to evaluate the result, the algorithm let the car drive with the current parameters and retrieve the accumulative error for a period of n frames, if the car gets off the track, the simulator and the acummulative error reset automatically. 
the acummulative error is algo multiplied by an exponential decay in order to penalized parameters that couldn't keep on track for enough time. so the longer the car drives lesser is the error.

at first the algorithm converges in a local minima that couldn't satisfied the needs of driving within the lane and achieve at least one track.

then I tried manually tweaking the parameters, until i can achieve one lap. and then using those parameters to get a near local minima

I started with kp=0.25 ki=0 kd=5
and the algorithm finally converges in kp=0.240130172319 ki=0 kd=5.3309999

To run the program with the found pid parameters

cd build
./pid -s 0.240130172319,0,5.3309999,

to run the program with the twiddle algorithm
python coeff_finder.py


## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

There's an experimental patch for windows in this [PR](https://github.com/udacity/CarND-PID-Control-Project/pull/3)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

## Editor Settings

We've purposefully kept editor configuration files out of this repo in order to
keep it as simple and environment agnostic as possible. However, we recommend
using the following settings:

* indent using spaces
* set tab width to 2 spaces (keeps the matrices in source code aligned)

## Code Style

Please (do your best to) stick to [Google's C++ style guide](https://google.github.io/styleguide/cppguide.html).

## Project Instructions and Rubric

Note: regardless of the changes you make, your project must be buildable using
cmake and make!

More information is only accessible by people who are already enrolled in Term 2
of CarND. If you are enrolled, see [the project page](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/f1820894-8322-4bb3-81aa-b26b3c6dcbaf/lessons/e8235395-22dd-4b87-88e0-d108c5e5bbf4/concepts/6a4d8d42-6a04-4aa6-b284-1697c0fd6562)
for instructions and the project rubric.

## Hints!

* You don't have to follow this directory structure, but if you do, your work
  will span all of the .cpp files here. Keep an eye out for TODOs.

## Call for IDE Profiles Pull Requests

Help your fellow students!

We decided to create Makefiles with cmake to keep this project as platform
agnostic as possible. Similarly, we omitted IDE profiles in order to we ensure
that students don't feel pressured to use one IDE or another.

However! I'd love to help people get up and running with their IDEs of choice.
If you've created a profile for an IDE that you think other students would
appreciate, we'd love to have you add the requisite profile files and
instructions to ide_profiles/. For example if you wanted to add a VS Code
profile, you'd add:

* /ide_profiles/vscode/.vscode
* /ide_profiles/vscode/README.md

The README should explain what the profile does, how to take advantage of it,
and how to install it.

Frankly, I've never been involved in a project with multiple IDE profiles
before. I believe the best way to handle this would be to keep them out of the
repo root to avoid clutter. My expectation is that most profiles will include
instructions to copy files to a new location to get picked up by the IDE, but
that's just a guess.

One last note here: regardless of the IDE used, every submitted project must
still be compilable with cmake and make./
