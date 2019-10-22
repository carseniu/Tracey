#include "Table.h"

#include "TableHeader.h"
#include "TableItemDelegate.h"


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
Table::Table(QWidget* parent) :
  QTableWidget(parent)
{
  TableHeader* horizontalHeader = new TableHeader(Qt::Horizontal, this);
  horizontalHeader->setStretchLastSection(true);
  setHorizontalHeader(horizontalHeader);

  setFrameShape(QFrame::NoFrame);
  setShowGrid(false);
  setMouseTracking(true);

  itemDelegate = new TableItemDelegate(this);
  setItemDelegate(itemDelegate);
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Table::enterEvent(QEvent*event)
{
  itemDelegate->setClearHover(false);
  update();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Table::leaveEvent(QEvent*event)
{
  itemDelegate->setClearHover(true);
  update();
}