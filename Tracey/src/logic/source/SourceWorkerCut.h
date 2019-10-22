#pragma once

#include "src/logic/source/SourceWorker.h"

class SourceData;


class SourceWorkerCut : public SourceWorker
{
  Q_OBJECT

public:
  SourceWorkerCut(SourceData* sourceData);

protected:
  void doWork(TraceWorker::WorkType workType) override;

private:
  void appendNewLine(const QString& str, TraceWorker::WorkType workType);

  bool ignore = false;
  bool lineEnd = false;
  QStringList newLines = QStringList() << "";
  int currentNewLine = 0;
  QString lastLine = "";
  QString currentLine = "";

};