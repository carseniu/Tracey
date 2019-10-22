#pragma once

#include <QItemDelegate>

class ComboBoxDelegate : public QItemDelegate
{
public:
  ComboBoxDelegate(QObject* parent = nullptr);

  void setHeight(int height);
  void setIconMode(bool status);
  void setWidth(int width);
  QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

protected:
  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
  int width;
  int height;

};