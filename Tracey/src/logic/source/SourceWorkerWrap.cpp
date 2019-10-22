#include "src/logic/source/SourceWorkerWrap.h"

#include "src/logic/source/SourceData.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SourceWorkerWrap::SourceWorkerWrap(SourceData* sourceData) :
  SourceWorker(sourceData)
{
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourceWorkerWrap::doWork(TraceWorker::WorkType workType)
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
    QString line;
    bool replaceLastLine = false;

    fileSize = file.size();

    checkEndOfFile(getPosition(), fileSize);

    file.seek(getPosition());

    while (isRunning() && (lineLength = file.readLine(buffer, BUFFER_SIZE)) != -1)
    {
      QString line = getLineFromBuffer(workType, buffer, file.pos());

      if (line.isEmpty())
      {
        doStop();
        emit stopped(sourceData);
        break;
      }

      if (isLineInRange(workType))
      {
        currentLine.append(line);

        if ((currentLine.size() - 1 <= lineSize) || currentLine.isEmpty())
        {
          appendNewLine(currentLine);

          if (currentLine.back() == '\n')
          {
            incLineCount();
            currentLine = "";
          }
        }
        else
        {
          QStringList wrapLines = getWrapLines(currentLine, lineSize);

          if (!wrapLines.isEmpty())
          {
            for (int i = 0; i < wrapLines.size(); ++i)
            {
              appendNewLine(wrapLines.at(i));
            }

            if (wrapLines.last().back() == '\n')
            {
              incLineCount();
              currentLine = "";
            }
            else
            {
              currentLine = wrapLines.last();
            }
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

      if (newLines.size() >= getBatchSize())
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
      newLines.clear();
      newLines.append("");
      currentNewLine = 0;
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

void SourceWorkerWrap::appendNewLine(const QString& str)
{
  if (str.back() == '\n')
  {
    newLines[currentNewLine] = str.chopped(1);
    newLines.append("");
    currentNewLine++;
  }
  else
  {
    newLines[currentNewLine] = str;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int SourceWorkerWrap::getNextBorderIndex(const QString& string, int start)
{
  const int spaceIndex = string.lastIndexOf(' ', start - string.size() - 1);

  if (spaceIndex != -1)
  {
    return spaceIndex;
  }

  int otherThanSpaceIndex = -1;
  int i = start;

  while (i >= 0)
  {
    if (string.at(i).isLetterOrNumber())
    {
      i--;
    }
    else
    {
      otherThanSpaceIndex = i;
      break;
    }
  }

  return otherThanSpaceIndex;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QStringList SourceWorkerWrap::getWrapLines(const QString& str, int lineSize)
{
  QStringList result;
  QString buffer = "";
  int start = 0;
  QString string = str;

  while (start < str.size())
  {
    int end = start + lineSize;
    if (end > string.size())
    {
      end = string.size();
    }
    QString chunk = string.mid(start, end - start);

    buffer.append(chunk);

    int lastSpaceIndex = -1;
    while (true)
    {
      if (buffer.size() >= lineSize)
      {
        lastSpaceIndex = getNextBorderIndex(buffer, lineSize - 1);
      }
      else
      {
        break;
      }

      if (lastSpaceIndex == -1 || lastSpaceIndex == buffer.size() - 1)
      {
        if (!buffer.isEmpty() && buffer.size() <= lineSize)
        {
          result.append(buffer + '\n');
          buffer.clear();
        }
        else
        {
          result.append(buffer.left(lineSize + 1) + '\n');
          buffer = buffer.mid(lineSize + 1, buffer.size() - lineSize + 1);
        }
      }
      else
      {
        if (buffer.at(lastSpaceIndex) != ' ')
        {
          result.append(buffer.left(lastSpaceIndex + 1) + '\n');
        }
        else
        {
          result.append(buffer.left(lastSpaceIndex) + '\n');
        }
        
        buffer = buffer.mid(lastSpaceIndex + 1, buffer.size() - lastSpaceIndex);
      }
    }

    start += lineSize;
  }

  if (!buffer.isEmpty())
  {
    result.append(buffer);
  }

  if (result.last().back() == '\n' && str.back() != '\n' || result.last().right(2) == "\n\n")
  {
    result.last().chop(1);
  }

  return result;
}