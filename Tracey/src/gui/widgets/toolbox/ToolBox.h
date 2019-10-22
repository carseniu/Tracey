#ifndef Z_TOOL_BOX_H
#define Z_TOOL_BOX_H

#include <QFrame>

class ToolBoxItem;

class QScrollArea;
class QVBoxLayout;


class ToolBox : public QFrame
{
  Q_OBJECT

public:
  ToolBox(QWidget* parent = nullptr);

  void addItem(QWidget* widget, const QString& text);
  bool eventFilter(QObject* watched, QEvent* event) override;

protected:
  void resizeEvent(QResizeEvent* event) override;


private:
  void setContentWidth();

  QList<ToolBoxItem*> items;
  QFrame* contentWidget;
  QScrollArea* scrollArea;

  QVBoxLayout* itemLayout;
  bool scrollBarVisible;
  int spacing;

private slots:
  void updateLayout();

};

#endif