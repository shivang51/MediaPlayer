#include "Button.h"

Widgets::Button::Button(Widgets::Widget* parent, std::string text, int x, int y, int width, int height)
{
    parent->AddWidget(this);
}

Widgets::Button::~Button()
{ }

void Widgets::Button::OnUpdate()
{ }

void Widgets::Button::OnClick()
{ }

void Widgets::Button::OnDoubleClick()
{ }
