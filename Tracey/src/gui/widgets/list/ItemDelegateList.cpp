#include "src/gui/widgets/list/ItemDelegateList.h"

#include <QPainter>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
ItemDelegateList::ItemDelegateList(QObject* parent) :
  QItemDelegate(parent),
  selectionType(FullRow),
  height(20)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int ItemDelegateList::getHeight() const
{
  return height;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ItemDelegateList::setHeight(int height)
{
  this->height = height;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ItemDelegateList::setSelectionType(SelectionType selectionType)
{
  this->selectionType = selectionType;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QSize ItemDelegateList::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QSize size = QItemDelegate::sizeHint(option, index);

  size.setHeight(height);

  return size;
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ItemDelegateList::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QRect rect = option.rect;
  QRect bounds;

  QRect backgroundRect;
  QRect borderRect;

  QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));

  if (selectionType == FullRow)
  {
    bounds = QRect(rect.left() + 1, rect.top() + 1, rect.width() - 3, rect.height() - 2);
  }
  else
  {
    QFontMetrics fontMetrics(option.font);

    if (selectionType == IconText)
    {
      if (icon.isNull())
      {
        bounds = QRect(rect.left() + 1, rect.top() + 1, fontMetrics.horizontalAdvance(index.data().toString()) + 5, rect.height() - 2);
      }
      else
      {
        bounds = QRect(rect.left() + 1, rect.top() + 1, fontMetrics.horizontalAdvance(index.data().toString()) + height + 5, rect.height() - 2);
      }
    }
    else if (selectionType == Text)
    {
      bounds = QRect(rect.left() + 1 + height, rect.top() + 1, fontMetrics.horizontalAdvance(index.data().toString()) + 5, rect.height() - 2);
    }
  }

  if (option.state & QStyle::State_Selected)
  {
    painter->setPen("#b8babd");
    painter->setBrush(QColor("#eaecf0"));
    painter->drawRect(bounds);
  }
  else if (option.state & QStyle::State_MouseOver)
  {
    painter->setPen("#f8f9fa");
    painter->setBrush(QColor("#f8f9fa"));
    painter->drawRect(bounds);
  }
  else
  {
    painter->fillRect(bounds, QColor("#ffffff"));
  }


  // Draw icon and text
  painter->setPen(Qt::black);
  QFontMetrics fontMetrics(option.font);
  QString text = fontMetrics.elidedText(index.data().toString(), Qt::ElideRight, rect.width() - 30);

  if (!icon.isNull())
  {
    int iconHeight = height - 4;
    painter->drawPixmap(QPoint(rect.left() + 3, rect.top() + (rect.height() - iconHeight) / 2), icon.pixmap(iconHeight, iconHeight));
    painter->drawText(QPoint(rect.left() + iconHeight + 5, rect.bottom() - (rect.height() - fontMetrics.height()) / 2 - 2), text);
  }
  else
  {
    painter->drawText(QPoint(rect.left() + 3, rect.bottom() - (rect.height() - fontMetrics.height()) / 2 - 2), text);
  }
}