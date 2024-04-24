# ACES
Attention Monitoring and Cognition Enhancement System (ACES) Learning Material and Performance Measurement Application

Built in Qt Creator 11.0.3 (Community), QT version 6.6.1

Currently composed of a Spot The Difference, Number Memory, and Tetris style games. Each game tracks user performance
and outputs data to game-dependent log files. The log files are overwritten each time the application is launched. 
Ensure that you separately save or rename that file before launching a new instance if you want to keep its data. 

You must use MSVC instead of MinGW for building the application or else you will receive a dummy Bluetooth backend error
and be unable to communicate with the central controller.

The central controller will send a log file with the stimulation parameters back to the application's build folder upon 
ending the Tetris game. If you are testing users you should follow the main menu order of the games to get all the 
stimulation data returned correctly. Like the locally saved logfiles, this will be overwritten upon re-launch, so save them.

I also apologize if the game displays weirdly (not-centered in screen) for certain display sizes. This was initially 
developed on an ultrawide monitor and needed a scaling method for moving down to normal 1080p, but I didn't fully 
understand Qt's Designer tool at this point so some strange stuff can happen with the ui display on other machines (even if they're 1080p).
This is because Qt returns a very specific display size that is not the native resolution. You can alter the screen scaling logic 
in ACES.cpp if needed to improve the ui look on your specific display. There is debug output to see your screen size as returned by Qt.
