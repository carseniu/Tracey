#include "src/logic/source/SourceWorkerCut.h"

#include "src/logic/source/SourceData.h"

#include <QDebug>
#include <QFile>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SourceWorkerCut::SourceWorkerCut(SourceData* sourceData) :
  SourceWorker(sourceData)
{
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourceWorkerCut::doWork(TraceWorker::WorkType workType)
{
  SourceData* sourceData = getSourceData();

  QFile file(sourceData->getSource());
  qint64 fileSize = 0;

  int lineSize = getLineSize();

  if (workType == Content)
  {
    emit contentProcessStarted();
  }

  if (file.open(QFile::ReadOnly | QFile::Text))
  {
    const int BUFFER_SIZE = 4096 * 16;
    char buffer[BUFFER_SIZE];
    qint64 lineLength = 0;

    fileSize = file.size();

    checkEndOfFile(getPosition(), fileSize);

    file.seek(getPosition());

    int bufferSize = BUFFER_SIZE;

    while (isRunning() && (lineLength = file.readLine(buffer, BUFFER_SIZE)) != -1)
    {
      QString line = getLineFromBuffer(workType, buffer, file.pos());

      if (line.isEmpty())
      {
        doStop();
        emit stopped(sourceData);
        break;
      }

      lineEnd = false;

      if (isLineInRange(workType))
      {
        currentLine.append(line);

        if (!ignore)
        {
          if (currentLine.back() == '\n')
          {
            lineEnd = true;
          }

          if (currentLine.size() - 1 <= lineSize)
          {
            appendNewLine(currentLine, workType);
          }
          else
          {
            if (lineEnd && currentLine.size() > 1)
            {
              currentLine = currentLine.chopped(1);
            }

            if (currentLine.size() > lineSize)
            {
              currentLine = currentLine.left(lineSize); 

              if (lineEnd)
              {
                currentLine.append("\n");
                appendNewLine(currentLine, workType);
              }
              else
              {
                appendNewLine(currentLine, workType);
                ignore = true;
              }
            }
            else
            {
              appendNewLine(currentLine, workType);
            }
          }

          if (lineEnd)
          {
            currentLine = "";
            incLineCount();
          }
        }
        else
        {
          if (currentLine.right(1) == "\n")
          {
            ignore = false;
            appendNewLine(currentLine, workType);
            currentLine = "";
          }
          else
          {
            continue;
          }
        }
      }
      else
      {
        if (line.back() == '\n')
        {
          incLineCount();
        }
      }

      if (newLines.size() == getBatchSize())
      {
        emit dataAvailable(newLines);
        newLines.clear();
        newLines.append("");
        currentNewLine = 0;
      }

      if (workType == TraceWorker::Content)
      {
        if (isLineEndReached())
        {
          doStop();
          emit stopped(sourceData);
          break;
        }

        doUpdateProgress((double)file.pos() / fileSize * 100);
      }
    }

    if (isReset())
    {
      setReset(false);
      ignore = false;
      lineEnd = false;
      newLines.clear();
      newLines.append("");
      currentNewLine = 0;
      lastLine = "";
      currentLine = "";
    }

    if (!newLines.isEmpty() && !(newLines.size() == 1 && newLines.first().isEmpty()))
    {
      emit dataAvailable(newLines);
      newLines.clear();
      newLines.append("");
      currentNewLine = 0;
    }

    setPosition(file.pos());
    doUpdateProgress(100);

    file.close();
  }
  else
  {
    emit errorReadingFile();

    doStop();
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
void SourceWorkerCut::appendNewLine(const QString& str, TraceWorker::WorkType workType)
{
  if (str.back() == '\n')
  {
    newLines[currentNewLine] = str.chopped(1);

    if (isLastLineInRange(workType))
    { 
      return;
    }

    newLines.append("");
    currentNewLine++;
  }
  else
  {
    newLines[currentNewLine] = str;
  }
}