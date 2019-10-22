#pragma once

#include <QPushButton>


class ColorButton : public QPushButton
{
  Q_OBJECT

public:
  ColorButton(QWidget* parent = nullptr);

  void setColor(const QString& color);

protected:
  void changeEvent(QEvent* event) override;

private:
  void setLook();

  QString color;

};
