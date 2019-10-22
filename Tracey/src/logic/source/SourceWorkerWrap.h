#pragma once

#include "src/logic/source/SourceWorker.h"

class SourceData;


class SourceWorkerWrap : public SourceWorker
{
  Q_OBJECT

public:
  SourceWorkerWrap(SourceData* sourceData);

protected:
  void doWork(TraceWorker::WorkType workType) override;

private:
  void appendNewLine(const QString& str);
  int getNextBorderIndex(const QString& string, int start);
  QStringList getWrapLines(const QString& string, int lineSize);

  QStringList newLines = QStringList() << "";
  int currentNewLine = 0;
  QString currentLine = "";

};