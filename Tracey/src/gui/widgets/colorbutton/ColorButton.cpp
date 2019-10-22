#include "src/gui/widgets/colorbutton/ColorButton.h"

#include<QDebug>
#include<QEvent>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
ColorButton::ColorButton(QWidget* parent) :
  QPushButton(parent),
  color("#e0e0e0")
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ColorButton::setColor(const QString& color)
{
  this->color = color;

  setLook();
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ColorButton::changeEvent(QEvent* event)
{
  if (event->type() == QEvent::EnabledChange)
  {
    setLook();
  }
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ColorButton::setLook()
{
  if (isEnabled())
  {
    setStyleSheet("ColorButton{border:1px solid #017acc; background-color:" + color + "}");
  }
  else
  {
    setStyleSheet("ColorButton{border:1px solid #d0d0d0; background-color:#e0e0e0}");
  }
}