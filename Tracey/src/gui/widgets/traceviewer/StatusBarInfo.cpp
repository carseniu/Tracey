#include "src/gui/widgets/traceviewer/StatusBarInfo.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
StatusBarInfo::StatusBarInfo(QWidget* parent) :
  QFrame(parent)
{
  setFixedHeight(20);

  QWidget* separator1 = new QWidget(this);
  separator1->setFixedWidth(1);
  separator1->setStyleSheet("background-color:#89bff8");

  separator2 = new QWidget(this);
  separator2->setFixedWidth(1);
  separator2->setStyleSheet("background-color:#89bff8");
  separator2->setVisible(false);

  separator3 = new QWidget(this);
  separator3->setFixedWidth(1);
  separator3->setStyleSheet("background-color:#89bff8");
  separator3->setVisible(false);

  QLabel* linesLabel = new QLabel(tr("Lines:"), this);
  linesValueLabel = new QLabel(this);

  QLabel* lineLabel = new QLabel(tr("Line:"), this);
  lineValueLabel = new QLabel(this);

  QLabel* columnLabel = new QLabel(tr("Column:"), this);
  columnValueLabel = new QLabel(this);

  autoscrollMovie = new QMovie(":/Application/img/Autoscroll.gif", QByteArray(), this);
  autoscrollLabel = new QLabel(this);
  autoscrollLabel->setToolTip(tr("Autoscroll"));
  autoscrollLabel->setAlignment(Qt::AlignCenter);
  autoscrollLabel->setFixedWidth(24);
  autoscrollLabel->setMovie(autoscrollMovie);
  autoscrollLabel->setVisible(false);

  runningMovie = new QMovie(":/Application/img/Running.gif", QByteArray(), this);
  runningLabel = new QLabel(this);
  runningLabel->setToolTip(tr("Running"));
  runningLabel->setAlignment(Qt::AlignCenter);
  runningLabel->setFixedWidth(24);
  runningLabel->setMovie(runningMovie);
  runningLabel->setVisible(false);


  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addWidget(separator1);
  layout->addSpacing(5);
  layout->addWidget(linesLabel);
  layout->addWidget(linesValueLabel);
  layout->addSpacing(40);
  layout->addWidget(lineLabel);
  layout->addWidget(lineValueLabel);
  layout->addSpacing(20);
  layout->addWidget(columnLabel);
  layout->addWidget(columnValueLabel);
  layout->addSpacing(40);
  layout->addWidget(separator2);
  layout->addWidget(autoscrollLabel);
  layout->addWidget(separator3);
  layout->addWidget(runningLabel);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void StatusBarInfo::setAutoscroll(bool autoscroll)
{
  if (autoscroll)
  {
    separator2->setVisible(true);
    autoscrollLabel->setVisible(true);
    autoscrollMovie->start();
  }
  else
  {
    autoscrollMovie->stop();
    autoscrollLabel->setVisible(false);
    separator2->setVisible(false);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void StatusBarInfo::setCursorInfo(int line, int column)
{
  lineValueLabel->setText(line < 0 ? "" : QString::number(line));
  columnValueLabel->setText(column < 0 ? "" : QString::number(column));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void StatusBarInfo::setLinesInfo(int lines)
{
  linesValueLabel->setText(QString::number(lines));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void StatusBarInfo::setRunning(bool status)
{
  if (status)
  {
    separator3->setVisible(true);
    runningLabel->setVisible(true);
    runningMovie->start();
  }
  else
  {
    runningMovie->stop();
    runningLabel->setVisible(false);
    separator3->setVisible(false);
  }
}