#pragma once

#include <QListWidget>


class List : public QListWidget
{
  Q_OBJECT

public:
  List(QWidget* parent);

protected:
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent* event) override;

private:
  bool drag;
  int startDragPosition;

signals:
  void itemRowChanged(int oldPosition, int newPosition);

};