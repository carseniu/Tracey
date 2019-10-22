#include "TableItemDelegate.h"

#include "Table.h"

#include <QAbstractItemView>
#include <QDebug>
#include <QLineEdit>
#include <QListWidget>
#include <QModelIndex>
#include <QPainter>
#include <QScrollBar>
#include <QTreeWidget>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
TableItemDelegate::TableItemDelegate(Table* parent) :
  QItemDelegate(parent),
  parent((Table*)parent),
  height(20),
  clearHover(false),
  hoverColumn(-1),
  hover(false)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int TableItemDelegate::getHeight() const
{
  return height;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TableItemDelegate::setClearHover(bool clearHover)
{
  this->clearHover = clearHover;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TableItemDelegate::setHeight(int height)
{
  this->height = height;
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TableItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QRect bounds = QRect(1, option.rect.top(), parent->viewport()->rect().width() - 3, option.rect.height() - 1);

  if (index.column() <= hoverColumn)
  {
    hover = false;
  }

  if (option.state & QStyle::State_Selected)
  {
    painter->setPen("#b8babd");
    painter->setBrush(QColor("#eaecf0"));
    painter->drawRect(bounds);
  }
  else
  {
    if (option.state & QStyle::State_MouseOver && !clearHover)
    {
      painter->setPen("#f8f9fa");
      painter->setBrush(QColor("#f8f9fa"));
      painter->drawRect(bounds);

      hoverColumn = index.column();
      hover = true;
    }
    else
    {
      // Background
      if (!hover)
      {
        if (index.row() % 2 == 0)
        {
          painter->fillRect(option.rect, "#fcfcfc");
        }
        else
        {
          painter->fillRect(option.rect, "#ffffff");
        }
      }
    }
  }

  // Draw icon and text
  QFontMetrics fontMetrics(option.font);

  if (option.state & QStyle::State_MouseOver || option.state & QStyle::State_Selected)
  {
    for (int i = 0; i < parent->columnCount(); ++i)
    {
      QIcon icon = qvariant_cast<QIcon>(parent->model()->index(index.row(), i).data(Qt::DecorationRole));
      QString text = parent->fontMetrics().elidedText(parent->model()->index(index.row(), i).data().toString(), Qt::ElideRight, option.rect.width() - 30);


      bounds = QRect(parent->columnViewportPosition(i), parent->rowViewportPosition(index.row()), parent->columnWidth(i), parent->rowHeight(index.row()));

      drawIconAndText(painter, fontMetrics, bounds, icon, text);
    }
  }
  else
  {
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QString text = parent->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, option.rect.width() - 30);

    drawIconAndText(painter, fontMetrics, option.rect, icon, text);
  }
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TableItemDelegate::drawBackground(QPainter* painter, const QRect& bounds, const QLinearGradient& gradient) const
{
  painter->fillRect(bounds, gradient);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TableItemDelegate::drawBorder(QPainter* painter, const QRect& bounds, const QColor& color) const
{
  painter->setPen(color);
  painter->drawRect(bounds);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void TableItemDelegate::drawIconAndText(QPainter* painter, const QFontMetrics& fontMetrics, const QRect& bounds, const QIcon& icon, const QString& text) const
{
  painter->setPen(Qt::black);

  if (!icon.isNull())
  {
    int iconHeight = height - 4;
    painter->drawPixmap(QPoint(bounds.left() + 3, bounds.top() + (bounds.height() - iconHeight) / 2), icon.pixmap(iconHeight, iconHeight));
    painter->drawText(QPoint(bounds.left() + iconHeight + 5, bounds.bottom() - (bounds.height() - fontMetrics.height()) / 2 - 2), text);
  }
  else
  {
    painter->drawText(QPoint(bounds.left() + 3, bounds.bottom() - (bounds.height() - fontMetrics.height()) / 2 - 2), text);
  }
}