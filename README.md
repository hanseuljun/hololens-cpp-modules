# hololens-cpp-modules

Since it was difficult to find any reusable code for UWP based C++ HoloLens applications, I would like to share mine.

Currently, this repository has 2 modules: LocatableCameraModule and SpatialMappingModule.

Some of the codes are from https://github.com/Microsoft/Windows-universal-samples.

# LocatableCameraModule

This module lets you access to the HoloLens's locatable camera easily. Create it, then access it using GetFrame().

# SpatialMappingModule

This module lets you access to the spatial information that your HoloLens provides. Create it, then use it by GetFrame().

# RayCaster

There is also a raycaster using frames of both modules. You can use it in 2 ways.

By choosing a preview camera's pixel, or by a ray in the psudo-world coordinate system.

Psudo-world cooridnate system means the "currentCoordinateSystem" in SimpleMain.

There are some known issues in the raycaster, for example, it crashes when HoloLens fails to find its own position.

They should be removed, but I no longer have a HoloLens. Sorry.

# Sample

This repository, which does not have enough comments, includes a sample project to introduce you how to use the modules.

Thus, I recommend you to read SampleMain.h and SampleMain.cpp before using the modules.

The sample project creates the modules then leaves simple logs.

# Team

These codes were written while I have worked in HCIL, Seoul National University (http://hcil.snu.ac.kr) with

- Han Joo Chae
- Youli Chang
- Jungin Hwang
