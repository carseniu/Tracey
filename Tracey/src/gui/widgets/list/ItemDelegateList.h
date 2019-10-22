#pragma once


#include <QItemDelegate>


class ItemDelegateList : public QItemDelegate
{
  Q_OBJECT
public:
  enum SelectionType
  {
    IconText,
    Text,
    FullRow
  };

  ItemDelegateList(QObject* parent = nullptr);

  int getHeight() const;
  void setHeight(int height);
  void setSelectionType(SelectionType selectionType);
  QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

protected:
  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
  SelectionType selectionType;

  int height;

};