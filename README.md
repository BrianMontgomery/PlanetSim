# PlanetSim

ToDo:
Add prototypes within graphics for when I want to try implementing OpenGl

Layers: add and implement

Implement Window Events

Create three separate types of config, MachineConfig and UserConfig, Save:
Machine Config will not be editable by the user, contains info that needs to be readily changed during development
-Add configuration to the extensions and layers for instance from outside the engine using a PSIM_VULKAN_CONFIG file
-Add configuration to the device module - isDeviceSuitable - configurable desired supported features / rateDevice  / deviceExtensions

User Config will be runtime config for things which the user can safely configure without a recompile.

Save files

Work on model loading trees
Figure out how to mathematically load random terrain (basic) and render
Work on shaders
Materials
Planet Rendering (extreme zoom) (use icosahedron)