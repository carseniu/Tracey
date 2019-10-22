#pragma once

#include <QProxyStyle>


class ToolBarStyle : public QProxyStyle
{
  Q_OBJECT

public:
  void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0) const override;

};