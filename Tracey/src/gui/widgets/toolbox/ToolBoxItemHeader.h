#ifndef Z_TOOL_BOX_ITEM_HEADER_H
#define Z_TOOL_BOX_ITEM_HEADER_H

#include <QLabel>

class ToolBoxItem;


class ToolBoxItemHeader : public QLabel
{
  Q_OBJECT

public:
  explicit ToolBoxItemHeader(const QString& text, ToolBoxItem* parent);

protected:
  void mousePressEvent(QMouseEvent* event) override;

private:
  bool minimized;

signals:
  void clicked(bool minimized);

};

#endif