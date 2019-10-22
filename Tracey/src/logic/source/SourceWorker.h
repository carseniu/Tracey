#pragma once

#include "src/logic/core/TraceWorker.h"

class SourceData;


class SourceWorker : public TraceWorker
{
  Q_OBJECT

public:
  SourceWorker(SourceData* sourceData);
  int getLineCount() const;

protected:
  virtual void doWork(TraceWorker::WorkType workType) = 0;
  void checkEndOfFile(qint64 position, qint64 fileSize);
  int getBatchSize() const;
  QString getLineFromBuffer(TraceWorker::WorkType workType, char buffer[], qint64 filePosition);
  int getLineSize() const;
  qint64 getPosition() const;
  SourceData* getSourceData() const;
  void incLineCount();
  bool isLineEndReached() const;
  bool isLastLineInRange(TraceWorker::WorkType workType);
  bool isLineInRange(TraceWorker::WorkType workType);
  void setPosition(qint64 position);

private:
  SourceData* sourceData;

  qint64 position;
  qint64 positionEnd;
  qint64 lineStart;
  qint64 lineEnd;
  int lineSize;

  int batchSize = 1000;
  int lineCount = 0;

signals:
  void dataAvailable(const QStringList& newLines);
  void endOfFileSmallerThanPosition();
  void errorReadingFile();
  void lineAvailable(const QString& line, bool replaceLastLine);

};