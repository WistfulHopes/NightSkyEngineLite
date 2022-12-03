# NightSkyEngineLite

A "lite" version of my Unreal Engine fighting game framework, the Night Sky Engine. The gameplay code is the same, but everything else has been downsized to work with more hardware.

As Bluepritns are an Unreal-only system, I made my own scripting system based on BBScript. In fact, it's so similar that I made a fork of super-continent's BBScript rebuilder and parser at https://github.com/WistfulHopes/bbscript/tree/night-sky-script. However, the actual bytecode interpreter is written from scratch.

For the Unreal Engine version, see here: https://github.com/WistfulHopes/NightSkyEngine/
