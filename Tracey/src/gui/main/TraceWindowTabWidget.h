#pragma once

#include "src/gui/main/TraceTabWidget.h"


class TraceWindowTabWidget : public TraceTabWidget
{
  Q_OBJECT

public:
  TraceWindowTabWidget(QWidget* parent);

protected:
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

signals:
  void fileDropped(const QString& file);

};
