#pragma once

#include <QFrame>

#include <QMap>

class Viewport;


class Margin : public QFrame
{
  Q_OBJECT

public:
  Margin(Viewport* viewport);

  void addIndex(int index, const QRect& rect);
  void clearIndexMap();

protected:
  void mousePressEvent(QMouseEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

private:
  Viewport* viewport;
  QMap<int, QRect> indexMap;

signals:
  void markToggled(int index);

};