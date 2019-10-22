#pragma once

#include <QItemDelegate>

class Table;

class QAbstractItemView;


class TableItemDelegate : public QItemDelegate
{
public:
  TableItemDelegate(Table* parent = nullptr);

  int getHeight() const;
  void setClearHover(bool clearHover);
  void setHeight(int height);
  void setIconMode(bool status);

protected:
  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
  Q_DISABLE_COPY(TableItemDelegate);

  void drawBackground(QPainter* painter, const QRect& bounds, const QLinearGradient& gradient) const;
  void drawBorder(QPainter* painter, const QRect& bounds, const QColor& color) const;
  void drawIconAndText(QPainter* painter, const QFontMetrics& font, const QRect& bounds, const QIcon& icon, const QString& text) const;

  Table* parent;

  int height;

  // Workaround, the hover line is not cleared when the fucking cursor leaves the table
  // Set from Table::enterEvent() and Table::leaveEvent()
  bool clearHover;

  mutable int hoverColumn;
  mutable bool hover;
};