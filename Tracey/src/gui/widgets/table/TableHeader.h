#pragma once

#include <QHeaderView>

class Table;


class TableHeader : public QHeaderView
{
  Q_OBJECT

public:
  TableHeader(Qt::Orientation orientation, Table* table);

protected:
  virtual void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;

private:
  Q_DISABLE_COPY(TableHeader);

  Table* table;

};