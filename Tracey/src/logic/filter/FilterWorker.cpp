#include "src/logic/filter/FilterWorker.h"

#include "src/logic/filter/FilterData.h"
#include "src/logic/source/SourceData.h"

#include <QDateTime>
#include <QDebug>
#include <QThread>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterWorker::FilterWorker(FilterData* filterData, SourceData* sourceData, int extraLinesBefore, int extraLinesAfter) :
  TraceWorker(),
  filterData(filterData),
  sourceData(sourceData),
  extraLinesBefore(extraLinesBefore),
  extraLinesAfter(extraLinesAfter)
{
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterWorker::doWork(TraceWorker::WorkType workType)
{
  int currentLine = position;

  if (workType == Content)
  {
    emit contentProcessStarted();
  }

  QString newLastLine = sourceData->getLine(currentLine);

  // Check if the last line has been changed and the accepted status has been changed and add or remove the corresponding data
  if (workType == Updates && currentLine > previousAcceptedLine + extraLinesAfter)
  {
    if (lastLine != newLastLine)
    {
      if (filterData->check(newLastLine))
      {
        // If the line is accepted and was not accepted before add all the needed lines
        if (lastAcceptedLine != currentLine)
        {
          if (extraLinesCount > 0)
          {
            data << position;
          }
          else
          {
            addExtraLines();
          }

          setAcceptedLines();

          emit dataAvailable(data);
          data.clear();
        }
      }
      else
      {
        // If the line is not accepted, but it was accepted, delete the line and the before lines up until the last after lines of the previously accepted lines (lol)
        if (lastAcceptedLine == currentLine)
        {
          int startLastAccepted = position - extraLinesBefore;
          if (startLastAccepted < 0)
          {
            startLastAccepted = 0;
          }

          int startPreviousAccepted = previousAcceptedLine + extraLinesAfter + 1;
          if (startPreviousAccepted > currentLine)
          {
            startPreviousAccepted = currentLine;
          }

          emit lastLineChangedAndNotAccepted(qMax(startLastAccepted, startPreviousAccepted));
           
          extraLinesCount = 0;
        }
      }
    }
  }

  if (workType == Updates)
  {
    currentLine++;
    position++;
  }

  // Filter the existing lines
  int dataSize = sourceData->getSize();
  while (isRunning() && !isReset() && currentLine < dataSize)
  {
    QStringList lines = sourceData->getLines(currentLine, batchSize);

    for (QString line : lines)
    {
      if (position == dataSize)
      {
        break;
      }

      if (!isRunning() || isReset())
      {
        break;
      }

      if (filterData->check(line))
      {
        if (extraLinesCount > 0)
        {
          data << position;
        }
        else
        {
          addExtraLines();
        }

        setAcceptedLines();
      }
      else if (extraLinesCount > 0)
      {
        data << position;

        extraLinesCount--;
      }

      position++;
    }

    emit dataAvailable(data);
    data.clear();

    currentLine += batchSize;

    if (workType == TraceWorker::Content)
    {
      doUpdateProgress((double)position / dataSize * 100);
    }
  }

  position = sourceData->getSize() == 0 ? 0 : position - 1;
  lastLine = sourceData->getLine(position);

  if (isReset())
  {
    setReset(false);
    position = 0;
    extraLinesCount = 0;
    lastLine = "";
    lastAcceptedLine = -1;
    previousAcceptedLine = -1;
  }

  if (workType == Content)
  {
    emit contentProcessFinished();
  }
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterWorker::addExtraLines()
{
  int start = position - extraLinesBefore;

  if (data.size() > 0)
  {
    if (start <= data.last())
    {
      start = data.last() + 1;
    }
  }
  else if (start < 0)
  {
    start = 0;
  }

  for (int i = start; i <= position; ++i)
  {
    data << i;
  }

  extraLinesCount = extraLinesAfter;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterWorker::setAcceptedLines()
{
  if (lastAcceptedLine != -1)
  {
    previousAcceptedLine = lastAcceptedLine;
  }
  lastAcceptedLine = position;
}