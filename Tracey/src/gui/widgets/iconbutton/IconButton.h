#pragma once

#include <QPushButton>


class IconButton : public QPushButton
{
  Q_OBJECT

public:
  enum State {
    Normal,
    Hover,
    Pressed,
  };

  IconButton(const QIcon& icon, QWidget* parent);

protected:
  void changeEvent(QEvent * event) override;
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent* event) override;

private:
  int hoverLightness;
  State state;
  State stateBeforeDisable;

  bool focus;
};