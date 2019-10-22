#pragma once

#include <QFrame>


class MarkItem : public QFrame
{
  Q_OBJECT

public:
  MarkItem(int index, const QString& text, int indexSize, QWidget* parent);

};
