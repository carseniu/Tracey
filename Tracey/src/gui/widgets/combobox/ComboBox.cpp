#include "src/gui/widgets/combobox/ComboBox.h"

#include "src/gui/widgets/combobox/ComboBoxDelegate.h"

#include <QAbstractItemView>
#include <QScrollBar>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
ComboBox::ComboBox(QWidget* parent) :
  QComboBox(parent)
{
  setItemDelegate(new ComboBoxDelegate(this));
  view()->setVerticalScrollBar(new QScrollBar(Qt::Vertical, view()));

  setContextMenuPolicy(Qt::NoContextMenu);
  setFixedHeight(20);
}