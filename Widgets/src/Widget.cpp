#include "Widget.h"

Widgets::Widget::Widget()
{ }

Widgets::Widget::~Widget()
{ }

void Widgets::Widget::OnUpdate()
{
    for ( auto widget : this->SubWidgets )
    {
        widget->OnUpdate();
    }
}

void Widgets::Widget::AddWidget(Widget* widget)
{
    this->SubWidgets.emplace_back(widget);
}
