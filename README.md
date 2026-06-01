TeamSpeak SDK Plugin for Unreal Engine 4 & 5
======================================

About
-----

A wrapper for the native C interface of the TeamSpeak SDK, shipped as plugin for Unreal Engine.
The interface provides the complete TeamSpeak SDK functionality with Unreal Engine supported types.

Features:

* C++ style interface with Unreal Engine types
* Almost all functions and callback are availible in Blueprints
* Packaged as Unreal Engine code plugin
* Fully-featured TeamSpeak SDK


Setup
-----

1. Populate the TeamSpeak SDK into the plugin's `ThirdParty` folder by running the bootstrap script from this `Plugins/` directory:

   ```
   python bootstrap.py
   ```

   This downloads the SDK version pinned in `bootstrap_config.json` (currently 3.5.0) from `files.teamspeak-services.com` and lays each platform's `client_sdk/<platform-arch>/` straight into `TeamSpeak_SDK/ThirdParty/<platform-arch>/`. To change the SDK version or trim the platform list, edit `bootstrap_config.json`. For details on how the libraries are consumed, see `TeamSpeak_SDK.Build.cs`.
2. The TeamSpeak SDK plugin is ready to use. Just place the whole `TeamSpeak_SDK` folder in your Unreal Engine project's `Plugins` path.

The Unreal Engine build environment automatically detects the current target. The bootstrap populates these platforms: `windows-x86_64`, `linux-x86_64`, `linux-armv8`, `macos-x86_64`, `macos-armv8`, `android-arm64-v8a`, `android-x86_64`, `ios`.


Classes
-------

* TeamSpeak_Manager - Wrapper around the TeamSpeak SDK
* TeamSpeakFunctionLibrary - Native Blueprint Library