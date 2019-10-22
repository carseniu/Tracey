#pragma once

#include "src/gui/widgets/tabwidget/TabWidget.h"


class TraceTabWidget : public TabWidget
{
public:
  TraceTabWidget(QWidget* parent);

  int getNewTabNumber(const QString& name) const;

};