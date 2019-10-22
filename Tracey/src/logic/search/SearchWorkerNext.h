#pragma once

#include "src/logic/search/SearchWorker.h"

#include <QRegularExpression>

class TraceData;


class SearchWorkerNext : public SearchWorker
{
  Q_OBJECT

public:
  SearchWorkerNext(TraceData* traceData, int cursorLine, int cursorColumn, const QRegularExpression& regexp);
  ~SearchWorkerNext() override;

  void process() override;

private:
  bool checkMatch(const QRegularExpressionMatch& match);
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