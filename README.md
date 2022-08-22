# TcAnagame

## Overview

This Project is an effort to create a version of the [Anagame Project](https://github.com/TrecApps/AnaGame) that 

1. Compiles for Windows, Linux Desktop, and Apple's OS X
2. Is less bloated than the Windows-only version (already linked)

To accomplish this, lower level libraries will occasionally hold some files that create types and functions that map to each platform, such that code can use these types/functions without regard for which platform is is being compiled for.

It will also rely on cross-platform dependencies in leu of the Windows-Centric dependencies that the Original project uses:

1. OpenGL (and GLFW) in leu of Direct2D/Direct3D
2. FreeType in leu of DirectWrite
3. ffmpeg in leu of Media Foundation

## Projects

The following is a list of Projects that should be part of the Anagame Solution:

1. Anagame-Central - Basically a central hub vor video-playback and game-play
2. Anagame-Builder - Used for Video-Editing and Other more Development-Centric tasks
3. Web-Tours - Anagame's Dedicated Web-Browser, though it should be capable of running regular apps writen for Anagame - just like Central

The Libraries they are built-upon

* AnaApp - Wraps all of the underlying libraries and provides a framework for which the three Applications can structure themselves
* AnaCode - Provides Default Compilers and Runners (JavaScript, Python, Anagame centric languages) as well as out-of-the-box environments with which you can structure your projects
* AnaWeb - Provides HTML/CSS Support for User-Interface rendering
* AnaWidget - Provides Common Widgets for Interaction
* TcGraph - Provides a Wrapper for 2D/3D drawing as well as Animations/Video Playback
* TcSock - Provides networking, encryption
* TrecLib - Core Library upon which everything else is built

_Note:_ As of right now, only **TcGraph** and **TrecLib** have been created

## Dependencies

1. Pick a Directory and run `git clone https://github.com/TrecApps/TcAnagame.git` to clone the project
2. [Once The project has a *Resources* folder, Build the Project and copy this folder into the Resulting *debug* or *release* folder (if your building for x64, those folders might be behind a *x64* folder)].

### Windows

The Project is activaly being developed in Visual Studio 2022 Community Edition, on Windows 10 Home Edition, x64-bit mode. The Solution/Project Files hav been written to reference Files from a Relative Directory.

Instructions for adding dependencies for Visual Studio Projects is here: https://vcpkg.io/en/getting-started.html

However, I found it easier to tweak the instructions a bit. From Git Bash, I used:

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
./vcpkg install [dependency]
```

Note: I found that I needed to make sure my OneDrive Program was running before it would work.

For each Time you install a dependency, look for the line:
`Stored binary cache: "C:\Users\{Windows User Account}\AppData\Local\vcpkg\archives\{Number}\{Long-Hash}.zip"`

Go into this Folder and copy the File Folders into a place next to your TcAnagame Repo like So...

./TcAnagame/{Folder Contents}
./{Dependency Name}/{Copied Folders} (Check the dependency sub-sections for what "Dependency Name" should be...

#### GLFW

This Dependency is used to ensure that OpenGL can be used in a Windowed-Environment

Command: `./vcpkg install glfw3:x64-windows` (Note: If you are focused on Building a 32-bit Version, omit the `:x64-windows` from the command
Dependency Name (Folder Name to copy to): `GLFW`

#### GLEW

This Dependency is used to ensure that OpenGL shaders can be referenced in code

Command: `./vcpkg install glew:x64-windows` (Note: If you are focused on Building a 32-bit Version, omit the `:x64-windows` from the command
Dependency Name (Folder Name to copy to): `GLEW`

#### FreeType 2.0

This Dependency is being used to Provide Text Management support

Command: `./vcpkg install freetype:x64-windows` (Note: If you are focused on Building a 32-bit Version, omit the `:x64-windows` from the command
Dependency Name (Folder Name to copy to): `Freetype_2.0`

#### ffmpeg

This dependency is intended to Provide Video Encoding/Decoding Support

Command: `./vcpkg install ffmpeg:x64-windows` (Note: If you are focused on Building a 32-bit Version, omit the `:x64-windows` from the command
Dependency Name (Folder Name to copy to): `ffmpeg`

### Linux/Mac

I have not attempted to Clone this Repository or build it on a Mac or Linux Machine. However, if you're interested in contributing to this project and making sure this code compiles on those platforms, I have previous made an Attempt here "https://github.com/TrecApps/Anagame_x/" you can look to for inspiration