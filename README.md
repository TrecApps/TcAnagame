# TcAnagame

Note: Regardless of which directory you cloned the project in, this README will refer to that directory as `cloneDir`

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

Also, the *TcGraph* relies on 2 3rd Party projects to help load images to be used with OpenGL, and manage OpenGL math. To ensure that the project builds:

In the same Directory you cloned `TcAnagame` in, run 
* `git clone https://github.com/nothings/stb.git` 
* `git clone https://github.com/g-truc/glm.git`
* `git clone https://github.com/btzy/nativefiledialog-extended.git`

In Visual Studio (for Windows), the Project is already configured to look for the files there (it uses a relative file path, so you don't have to do anything). However, when Building the Project for nativefilefialog-extended with Visual Studio for Windows, make sure that the **C++ CMake Tools for Windows** component under **Desktop Development with C++** is installed.

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

Note: FreeType has a slightly different Procedure than the other dependencies

#### GLFW

This Dependency is used to ensure that OpenGL can be used in a Windowed-Environment

Command: `./vcpkg install glfw3:x64-windows` (Note: If you are focused on Building a 32-bit Version, omit the `:x64-windows` from the command
Dependency Name (Folder Name to copy to): `GLFW`

#### GLEW

This Dependency is used to ensure that OpenGL shaders can be referenced in code

Command: `./vcpkg install glew:x64-windows` (Note: If you are focused on Building a 32-bit Version, omit the `:x64-windows` from the command
Dependency Name (Folder Name to copy to): `GLEW`

#### FreeType 2.0

This Dependency is being used to Provide Text Management support. Howver, the version of FreeType supplied by `vcpkg` lists *bz2.dll* as a dependency, which may or may not be available on your computer. So follow these steps to get FreeType 2.0 Properly integrated into your computer.

1. Go to https://sourceforge.net/projects/freetype/files/freetype2/ and download the latest version.
2. Unzip the File.
3. In `Builds/windows/visualc`, open up the solution. If Visual Studio requests to update the project, say yes.
4. By default, the setting will be x86. If you're targeting 32-bit, you can leave it. Otherwise, make sure to set the project to x64.
5. Build the Project

Note: for the next few steps, we'll assume you cloned Anagame from a folder called `cloneDir`

6. Copy the contents of `{freetype_download}/include` to `cloneDir/Freetype_2.0/include`
7. Copy or move the `freetype.lib` file from `{freetype_download}/objs/{win32 or x64}/Debug` folder to `cloneDir/Freetype_2.0/lib`

Your project should build.

#### ffmpeg

This dependency is intended to Provide Video Encoding/Decoding Support

Command: `./vcpkg install ffmpeg:x64-windows` (Note: If you are focused on Building a 32-bit Version, omit the `:x64-windows` from the command
Dependency Name (Folder Name to copy to): `ffmpeg`

#### DLLs

Once your project builds, you'll want to move the various DLLs from your dependencies into your build directory in order to get the program to run. 

For Freetype, the DLL can be found in your `{freetype_download}/objs/{win32 or x64}/Debug` folder.

For the other dependencies, they can be found in `cloneDir/{dependency}/bin` folders

If, when running your program, it complains about not having the `zlib1.dll` file available, I find that Git Bash comes with a working copy of the DLL you can copy into your project's directory.


### Linux/Mac

I have not attempted to Clone this Repository or build it on a Mac. However, if you're interested in contributing to this project and making sure this code compiles on those platforms, I have previous made an Attempt here "https://github.com/TrecApps/Anagame_x/" you can look to for inspiration

### Linux

Note: Currently, I have not managed to build a working compilation process. However, I did begin the process of one.

Clone the project in any directory you wish.

You will need to Install Dependencies to your Linux environment before you begin (for reference, I was using Ubuntu).

#### GLFW

This Dependency is used to ensure that OpenGL can be used in a Windowed-Environment

1. From `cloneDir`, clone GLFW by typing `git clone https://github.com/glfw/glfw.git`
2. cd into the `glfw` folder.
3. Run `sudo apt-get install cmake xorg-dev libglu1-mesa-dev`. You'll need them to build glfw
4. Run `cmake -G "Unix Makefiles"`
5. Run `make ; sudo make install`

#### Freetype 2.0

This Dependency is being used to Provide Text Management support.

1. From 'cloneDir', clone FreeType by typing `git clone Https://gitlab.freedesktop.org/freetype/freetype.git` (Note: you might need to run `git config --global http.sslVerify false`)
2. cd into the resulting `freetype` folder
3. Run `make ; sudo make install`

#### GLEW

This Dependency is used to ensure that OpenGL shaders can be referenced in code

1. Download the most recent version of Glew from here: `https://sourceforge.net/projects/glew/`
2. Extract the Compressed file to `cloneDir`
3. cd into the extraxted folder
4. Run `make ; sudo make install`

#### FFMPEG

This dependency is intended to Provide Video Encoding/Decoding Support

Note: this WILL take time

1. From `cloneDir`, clone ffmpeg via `git clone https://github.com/FFmpeg/FFmpeg.git`
2. cd into `FFmpeg`
3. Run `./configure` , this is needed to enable the make process to work (without complaining that `/tests/Makfile` can't be found, in my case)
4. Run `make ; sudo make install`
