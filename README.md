# CSCI 4448 Final Project #

- *Authors*:			Stephen Ham, Josh Weaver
- *Date Submitted*: 	2014/05/01
- *Description*: 		Modified Arkanoid clone

------------------
**Technical Overview**

This project was a modification & re-factoring of Vittorio Romeo's ["Arkanoid clone in 160 lines"](https://isocpp.org/blog/2013/11/arkanoid "video") for our CSCI 4448 (*Object-Oriented Analysis & Design*) course at CU-Boulder in Spring 2014.

Our goal was to modify some of the gameplay functionality and re-factor his code using at least one design pattern. A list of our modifications:

- Converted the C++ structs to classes for a proper Object-Oriented design
- Fixed various gameplay issues (like preventing the ball from bouncing off the bottom of the window and making it “fall off” instead, etc)
- Added various game elements: scoring, pausing, a “bad ball” that the player has to avoid, game-over states (winning & losing), a main menu, etc
- Refactored for general code clean-up & added the Facade design pattern by creating a new GameEngine class

Development Tools:

- Code::Blocks 13.12
- GNU C++11 via MinGW
- SFML 2.1

Files:

- `arial.ttf` (font file needed to run the game, copied from Windows\Fonts)
- `ark.cpp` (in-progress version of modified game source code)
- `ark-final.cpp` (final version of modified game source code)
- `ark_diagrams_uml.png` (UML class diagram created with Visual Paradigm)
- `ark_diagrams_use_case.png` (use case diagram)
- `ark-final.exe.old` (pre-built executable for Windows, rename the extension to run)
- `arkanoid-main.png` (image file needed to run the game, found via Google Image Search)
- `project5_readme.pdf` (readme file submitted for the course)
- `sfml-graphics-2.dll` (SFML resource file need to run the pre-built game)
- `sfml-system-2.dll` (SFML resource file need to run the pre-built game)
- `sfml-window-2.dll` (SFML resource file need to run the pre-built game)
- `slides.pptx` (PowerPoint presentation submitted for the course)

------------------------------------------
**Unpacking & Compilation on Windows**

1. If not installed already, download & install [MinGW](http://www.mingw.org/ "MinGW").
2. Install at least version 2.1 of the SFML API from this site: [http://www.sfml-dev.org/download.php](http://www.sfml-dev.org/download.php "SFML"). We originally tested with the "GCC 4.7 MinGW (DW2) - 32 bits" package, but any future GCC MinGW DW2 versions should work as well.
3. Extract/copy all files to a desired folder path.
4. To compile, enter the following at a command line (adjust the SFML location & version if needed):
	> g++ -std=c++11 -Wall -fexceptions -DSFML_STATIC -g -IC:\SFML-2.1\include -c ark-final.cpp -o ark.o
5. To link & build, enter (adjust the SFML location & version if needed):
	> g++ -LC:\SFML-2.1\lib -o ark.exe ark.o -lsfml-graphics -lsfml-window -lsfmlsystem
6. To run the game, enter:
	> ark

-----------------------------
**User Interaction Instructions**

All play instructions are provided in-game upon startup.

-----------------------------
**Known Issues/Bugs**

The program does not have mouse support and clicking on its window may cause color shifting (for unknown reasons). Exit the program through the in-game keyboard instruction only (don’t click the window’s “X” button).

----------
**References**

The initial version of our source code was transcribed from Vittorio Romeo's original video: [https://www.youtube.com/watch?v=_4K3tsKa1Uc](https://www.youtube.com/watch?v=_4K3tsKa1Uc "Arkanoid video")