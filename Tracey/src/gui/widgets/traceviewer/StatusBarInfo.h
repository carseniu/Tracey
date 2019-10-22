#pragma once

#include <QFrame>

class QLabel;

class StatusBarInfo : public QFrame
{
  Q_OBJECT

public:
  StatusBarInfo(QWidget* parent);

private:
  QLabel* linesValueLabel;
  QLabel* lineValueLabel;
  QLabel* columnValueLabel;
  QMovie* runningMovie;
  QLabel* runningLabel;
  QMovie* autoscrollMovie;
  QLabel* autoscrollLabel;

  QWidget* separator2;
  QWidget* separator3;

public slots:
  void setAutoscroll(bool autoscroll);
  void setCursorInfo(int line, int column);
  void setLinesInfo(int lines);
  void setRunning(bool status);

};