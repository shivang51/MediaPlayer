#include <thread>
#include <iostream>

#include "./src/Application.h"

int main(int argc, char** argv)
{
	std::string filename = "";

	Application app;

	if (argc > 1)
	{
		filename = argv [1];
		app.play(filename);
	}
	app.run();
	return 0;
}