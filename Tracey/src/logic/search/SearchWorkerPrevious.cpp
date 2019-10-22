#include "src/logic/search/SearchWorkerPrevious.h"

#include "src/logic/core/TraceData.h"

#include <QDebug>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchWorkerPrevious::SearchWorkerPrevious(TraceData* traceData, int cursorLine, int cursorColumn, const QRegularExpression& regexp) :
  SearchWorker(),
  traceData(traceData),
  cursorLine(cursorLine),
  cursorColumn(cursorColumn),
  regexp(regexp),
  batchSize(10000)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchWorkerPrevious::~SearchWorkerPrevious()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWorkerPrevious::process()
{
  int dataSize = traceData->getSize();
  int lineCount = 0;

  if (searchInLineBeforeCursor(traceData->getLine(cursorLine), cursorColumn))
  {
    emit matchFound(cursorLine, foundStart, foundEnd);
    emit finished();

    return;
  }

  for (int i = cursorLine - 1; i >= 0; i = i - batchSize)
  {
    int start = i - batchSize + 1;
    int count = batchSize;

    if (start < 0)
    {
      count = batchSize + start;
      start = 0;
    }

    QStringList lines = traceData->getLines(start, count);

    for (int j = lines.size() - 1; j >= 0; --j)
    {
      if (isCanceled())
      {
        emit finished();
        return;
      }

      lineCount++;

      if (searchInLine(lines.at(j)))
      {
        emit matchFound(start + j, foundStart, foundEnd);
        emit finished();

        return;
      }

      doUpdateProgress((double)lineCount / dataSize * 100);
    }
  }

  for (int i = dataSize - 1; i >= cursorLine + 1; i = i - batchSize)
  {
    int start = i - batchSize;
    if (start < cursorLine + 1)
    {
      start = cursorLine + 1;
    }

    QStringList lines = traceData->getLines(start, batchSize);

    for (int j = lines.size() - 1; j >= 0; --j)
    {
      if (isCanceled())
      {
        emit finished();
        return;
      }

      lineCount++;

      if (searchInLine(lines.at(j)))
      {
        emit matchFound(start + j, foundStart, foundEnd);
        emit finished();

        return;
      }

      doUpdateProgress((double)lineCount / dataSize * 100);
    }
  }

  if (searchInLineAfterCursor(traceData->getLine(cursorLine), cursorColumn))
  {
    emit matchFound(cursorLine, foundStart, foundEnd);
    emit finished();

    return;
  }

  emit matchNotFound();
  emit updateReady(100, 0);
  emit finished();
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SearchWorkerPrevious::checkMatch(QRegularExpressionMatchIterator& iterator, int cursorPosition)
{
  QList<QRegularExpressionMatch> matches;

  while (iterator.hasNext()) {
    QRegularExpressionMatch match = iterator.next();

    if (match.hasMatch()) {
      matches.append(match);
    }
  }

  if (matches.size() > 0 && matches.last().capturedEnd() == cursorPosition)
  {
    matches.removeLast();
  }

  if (matches.size() > 0)
  {
    foundStart = matches.last().capturedStart();
    foundEnd = matches.last().capturedEnd();

    return true;
  }

  return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SearchWorkerPrevious::searchInLineAfterCursor(const QString& line, int cursorPosition)
{
  QRegularExpressionMatchIterator iterator = regexp.globalMatch(line, cursorPosition);

  return checkMatch(iterator, cursorPosition);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SearchWorkerPrevious::searchInLineBeforeCursor(const QString& line, int cursorPosition)
{
  QRegularExpressionMatchIterator iterator = regexp.globalMatch(line.mid(0, cursorPosition));

  return checkMatch(iterator, cursorPosition);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SearchWorkerPrevious::searchInLine(const QString& line)
{
  QRegularExpressionMatchIterator iterator = regexp.globalMatch(line);

  return checkMatch(iterator);
}