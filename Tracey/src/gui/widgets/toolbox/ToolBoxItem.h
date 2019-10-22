#ifndef Z_TOOL_BOX_ITEM_H
#define Z_TOOL_BOX_ITEM_H

#include <QWidget>

class ToolBoxItemHeader;


class ToolBoxItem : public QWidget
{
  Q_OBJECT

public:
  explicit ToolBoxItem(QWidget* widget, const QString& text, QWidget* parent = nullptr);

private:
  ToolBoxItemHeader* header;
  QWidget* content;

private slots:
  void hideContent(bool hide);

signals:
  void sizeChanged();

};

#endif