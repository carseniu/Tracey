#pragma once

#include "src/logic/search/SearchWorker.h"

#include <QRegularExpression>

class TraceData;


class SearchWorkerPrevious : public SearchWorker
{
  Q_OBJECT

public:
  SearchWorkerPrevious(TraceData* traceData, int cursorLine, int cursorColumn, const QRegularExpression& regexp);
  ~SearchWorkerPrevious() override;

  void process() override;

private:
  bool checkMatch(QRegularExpressionMatchIterator& iterator, int cursorPosition = -1);
  bool searchInLineAfterCursor(const QString& line, int cursorPosition);
  bool searchInLineBeforeCursor(const QString& line, int cursorPosition);
  bool searchInLine(const QString& line);

  TraceData* traceData;
  int cursorLine;
  int cursorColumn;
  QRegularExpression regexp;
  int batchSize;

  int foundStart;
  int foundEnd;

};