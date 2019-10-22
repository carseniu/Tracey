#pragma once 

#include <QPushButton>

class TabBar;

class QPushButton;


class TabItem : public QPushButton
{
  Q_OBJECT

public:
  TabItem(const QString& text, TabBar* parent);

  QString getText() const;
  bool isFixed() const;
  bool isSelected() const;
  void setFixed(bool fixed);
  void setText(const QString& text);
  void setSelected(bool selected);

protected:
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void resizeEvent(QResizeEvent* event);

private:
  void setLook();

  bool fixed = false;
  bool selected = false;
  bool pressed = false;

  QString text;
  int nameResizeOffset = 30;

  QString styleSheet;
  QString styleSheetSelected;
  QString styleSheetMoving;

signals:
  void contextMenuRequested(const QPoint& point);
  void itemMoved(const QPoint& localPosition, const QPoint& parentPosition);
  void itemPressed(const QPoint& localPosition, const QPoint& parentPosition);
  void itemReleased(const QPoint& parentPosition);
  void itemSelected();

};