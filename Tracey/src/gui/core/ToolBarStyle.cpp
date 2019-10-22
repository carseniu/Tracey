#include "src/gui/core/ToolBarStyle.h"

#include "src/Tracey.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOptionToolButton>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ToolBarStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
  if (element == CE_ToolButtonLabel)
  {
    const QStyleOptionToolButton* toolButtonOption = qstyleoption_cast<const QStyleOptionToolButton*>(option);

    if (option->state & State_Enabled)
    {
      QPixmap pixmap;
      if (option->state & State_Sunken || option->state & State_On)
      {
        pixmap = toolButtonOption->icon.pixmap(40, 40, QIcon::Normal, QIcon::On);
      }
      else
      {
        pixmap = toolButtonOption->icon.pixmap(40, 40, QIcon::Normal, QIcon::Off);
      }

      if (option->state & State_MouseOver && !(option->state & State_Sunken))
      {
        pixmap = setLightness(pixmap, 110);
      }

      painter->drawPixmap(4, toolButtonOption->rect.top() + 1, pixmap);
    }
    else
    {
      painter->drawPixmap(4, toolButtonOption->rect.top() + 1, toolButtonOption->icon.pixmap(40, 40, QIcon::Disabled));
    }
  }
}