#pragma once

#include "src/logic/core/TraceWorker.h"

#include <QVector>

class FilterData;
class SourceData;


class FilterWorker : public TraceWorker
{
  Q_OBJECT

public:
  FilterWorker(FilterData* filterData, SourceData* sourceData, int extraLinesBefore, int extraLinesAfter);

protected:
  void doWork(TraceWorker::WorkType workType) override;

private:
  void addExtraLines();
  void setAcceptedLines();

  FilterData* filterData;
  SourceData* sourceData;
  int extraLinesBefore;
  int extraLinesAfter;

  QVector<qint64> data;
  int position = 0;
  int batchSize = 30000;
  int extraLinesCount = 0;
  QString lastLine = "";

  int lastAcceptedLine = -1;
  int previousAcceptedLine = -1;

signals:
  void dataAvailable(const QVector<qint64>& data);
  void lastLineChangedAndNotAccepted(int line);

};