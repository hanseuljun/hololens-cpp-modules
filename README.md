# hololens-cpp-modules

Since it is difficult to find any reusable code for UWP based HoloLens applications,
I would like to share mine here.

Currently, this repository has 2 modules: LocatableCameraModule and SpatialMappingModule.

# LocatableCameraModule

This module lets you access to the HoloLens's locatable camera easily. Create it, then access it using GetFrame().

# SpatialMappingModule

This module lets you access to the spatial information that your HoloLens provides. Create it, then use it by GetFrame().

# Sample

This repository, which does not have enough comments, includes a sample project to introduce you how to use the modules.

I recommend you to read SampleMain.h and SampleMain.cpp before using the modules.

The sample project creates the modules then leaves simple relevant logs.
