TeamSpeak 3 Plugin for Unreal Engine 4
======================================

About
-----

A wrapper for the native C interface of the TeamSpeak SDK, shipped as plugin for Unreal Engine 4.
The interface provides the complete TeamSpeak SDK functionality with Unreal Engine 4 supported types.

Features:

* C++ style interface with Unreal Engine 4 types
* Almost all functions and callback are availible in Blueprints
* Packaged as Unreal Engine 4 code plugin
* Fully-featured TeamSpeak-SDK


Setup
-----

1. In order to use the TeamSpeak functionality, download the TeamSpeak SDK from our website (http://TeamSpeak.com/downloads#sdk).
2. Now the TeamSpeak SDK has to be integrated in the Unreal Engine search paths.
   * Unzip the TeamSpeak SDK and copy the "bin", "include" and "lib" folder in "TeamSpeak_SDK/ThirdParty"
   * for more information take a look in the TeamSpeak_SDK.Build.cs file.
3. The TeamSpeak SDK plugin is ready to use. Just place the whole TeamSpeak_SDK folder in your Unreal Engine 4 Plugins path of your project.

The Unreal Engine 4 build enviroment automatically detect the current target.
At the moment we support Win64 and Win32.


Classes
-------

* TeamSpeak_Manager - Wrapper around the TeamSpeak SDK
* TeamSpeakFunctionLibrary - Native Blueprint Library