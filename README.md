The old CPP REVERSE
2022_12_12

Build it using linking SoilFramework to it
SoilFramework can be found here:

https://github.com/ViniBZ/SoilFramework

To run:

CPP_LOGGER origin_directory destiny_directory

takes .cpp files from orig_directory, copies them to destiny directory with a
fprintf at the beginning and before any return, in every function/method implementation.

These fprintf calls are set to save a message to LOGGER.txt file in the current working directory. The message states the source file name, function name, and, if before the return/end of function, it says 'END'

Each fprintf call uses a new file descriptor just for its call. So watch out for overhead.
LOGGER.txt can get pretty big. Watch out for that too.

The original .cpp files remain unchanged in the origin_directory.

For the altered code to work it will probably need <iostream> for some of the functions

