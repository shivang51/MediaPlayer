#include <iostream>

#include "Widgets.h"

class MyWindow : public Widgets::Window
{
public:

	MyWindow() {};
	
	void OnUpdate()
	{
		Window::OnUpdate();

		this->UpdateSubWidgets();
		this->Swapbuffers();
	}
};

int main()
{
	MyWindow window;
	window.Create("Media Player", 800, 600);
	window.SetBackgroundColor({0.45f, 0.40f, 0.60f, 1.0f });



	Application app;
	app.SetMainWindow(&window);

	app.Run();
	return 0;
}