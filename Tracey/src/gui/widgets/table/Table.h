#pragma once

#include <QTableWidget>

class TableItemDelegate;


class Table : public QTableWidget
{
  Q_OBJECT

public:
  Table(QWidget* parent = nullptr);

protected:
  virtual void enterEvent(QEvent* event);
  virtual void leaveEvent(QEvent* event);

private:
  Q_DISABLE_COPY(Table);

  TableItemDelegate* itemDelegate;
};