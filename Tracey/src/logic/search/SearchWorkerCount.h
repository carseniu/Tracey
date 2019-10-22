#pragma once

#include "src/logic/search/SearchWorker.h"

#include <QRegularExpression>

class TraceData;


class SearchWorkerCount : public SearchWorker
{
  Q_OBJECT

public:
  SearchWorkerCount(TraceData* traceData, const QRegularExpression& regexp);
  ~SearchWorkerCount() override;

  void process() override;

private:
  TraceData* traceData;
  QRegularExpression regexp;
  int batchSize;

};