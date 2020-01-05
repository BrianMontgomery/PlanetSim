#include "PSIMPCH.h"

#include "Application/Application.h"


int main() {
	//initialize logging (the three day bug...)
	Log::init();

	Application app;

	app.run();

	Log::shutdown();
	system("PAUSE");

	return EXIT_SUCCESS;
}