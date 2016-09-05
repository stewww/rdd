# rdd
reckless-driving-detector

.gitignored:
Whenever you drag and drop files you don't want to check in, make sure to add this file extension/name to the .gitignore file

Do not put platform specific files in this repository. Only source files (.c/.h, etc.) or git files (.gitignore)

Video files should not be included (possible limit of repository size by github)

Infrastructural requirements:
 - API to create a trackbar on a specified matrix/window, parameters should be "window name, variable to control, min/max value, init value
 - Adding FPS on matrices (create an wrapper API for imshow that filters and calculates frame rate, can be a sliding window average)
 - Create API to get frame (kinda like how the other group was doing get x frame from x # of frames in this video)
 - Create platform specific headers for opencv_windows and opencv_linux for any differences (or other opencv version-specific function calls)
 - Generate test cases for tracking lanes/vehicles in videos, come up with API for this. This may have two components, a lightweight method that can be run in conjunction with the program, with simple comparisons being made during the execution, and more complex comparisons after the execution. This could include analysis tools for various steps in our algorithms
 - Determine performance/efficiency of a program/method (cpu cycles required, usually associated with frame rate)
 - Odroid testing

Infrastructural "nice to haves" 
 - Automate blob detection tests, maybe this will be a better approach (probably won't be)


filterBackground(Mat everything, Mat onlyStuffICareAbout);
