# Ogre/Gorilla/OculusRift Example

Ogre + Gorilla + OculusRift => Demo

## Feature

* Sample for Ogre 1.9 RC1
* Barrel distortion for OculusRift (based on [OgreOculus][ogreoculus])
* OculusRift Head Tracker Plug and Play at Runtime (Based on OculusSDK 0.2.5)
* 3D GUI Sample from [gorilla][gorilla]
* Support cmake 

## Build and run

### pre condition
Install Ogre. You need to set environment variable. **OGRE_HOME** = something like "D:/ogre/ogre/build/sdk".

I tested only windows. Maybe not working linux, mac

* mkdir build
* cd build
* cmake ..
* open build/demo.sln
* build **demo**
* build **INSTALL**
* demo->properties->Debugging->Working Directory : $(ProjectDir)/dist/bin
* run demo

## Usage
* wasd or left analog stick : Movement
* mouse move or right analog stick : Look
* OculusRift HeadTracker support
* mouse left click or joysitck A button : push button on 3D GUI
* esc : exit
* F5 : Normal View
* F6 : Stereo View without barrel distortion
* F7 : OculusRift View. Stereo View with barrel distirtion
* ~ : show console. this is gorilla sample.

[Video](http://www.youtube.com/watch?v=-8AOYWkNz1Y)
![screenshot](https://raw.github.com/shipduck/ogre-gorilla-oculus-rift/master/document/screenshot-oculus.jpg)

[ogreoculus]: https://bitbucket.org/rajetic/ogreoculus
[gorilla]: https://github.com/betajaen/gorilla
