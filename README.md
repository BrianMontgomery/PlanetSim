# PlanetSim

Todo: ImGui cannot use docking until put on separate renderPass 
	-requires separate command buffer,
	-requires the ability to reset the command buffer on the fly,
	-requires a new set of semaphores for imgui specifically
	-new descriptor pool and command pool