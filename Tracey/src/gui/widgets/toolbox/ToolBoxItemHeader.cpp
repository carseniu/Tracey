#include "ToolBoxItemHeader.h"

#include "ToolBoxItem.h"


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
ToolBoxItemHeader::ToolBoxItemHeader(const QString& text, ToolBoxItem* parent) :
  QLabel(text, parent),
  minimized(false)
{
  setAlignment(Qt::AlignCenter);
  setFixedHeight(25);

  QFont font = this->font();
  font.setBold(true);
  setFont(font);

}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ToolBoxItemHeader::mousePressEvent(QMouseEvent* event)
{
  emit clicked(minimized);

  minimized = !minimized;
}