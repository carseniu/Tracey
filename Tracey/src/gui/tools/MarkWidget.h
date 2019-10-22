#pragma once

#include <QFrame>

#include "src/logic/core/MarkData.h"

class List;


class MarkWidget : public QFrame
{
  Q_OBJECT

public:
  MarkWidget(QWidget* parent);

  void goToNextMark();
  void goToPreviousMark();

private:
  List* markList;

public slots:
  void setMarks(const QVector<Mark>& marks, int newIndex);

private slots:
  void onMarkChanged(int index);

signals:
  void markRequested(int index);

};