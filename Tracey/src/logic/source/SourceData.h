#pragma once

#include "src/logic/core/TraceData.h"

#include <QMutex>

class SourceWorker;


class SourceData : public TraceData
{
  Q_OBJECT

public:
  enum RangeType
  {
    FilePosition,
    Tail,
    LineNumber
  };

  enum LineHandling
  {
    Cut,
    Wrap
  };

  SourceData(const QString& id, const QString& name, QObject* parent = nullptr);
  ~SourceData();

  void clear() override;
  qint64 getFileSize() const;
  QString getLine(int line) override;
  qint64 getLineEnd() const;
  int getLineNumber(int index) override;
  QString getLineNumber(int index, bool source = false) override;
  QStringList getLines(int start, int count) override;
  QStringList getLines(const QVector<qint64> data);
  int getLineSize() const;
  int getLineStart() const;
  int getMaxLineSize() const;
  int getMinLineSize() const;
  bool getPreload() const;
  int getPositionEnd() const;
  int getPositionStart() const;
  int getSize() override;
  QString getSource() const;
  void open(const QString& source, bool preload, RangeType rangeType, qint64 openStart, qint64 openEnd, LineHandling lineHandling, int lineSize);
  void setSource(const QString& filePath);

protected:
  bool prepareWorker() override;

private:
  RangeType rangeType = FilePosition;
  LineHandling lineHandling = Cut;

  QString source;
  bool preload = true;

  QStringList data = QStringList () << "";
  int currentLine = 0;

  SourceWorker* sourceWorker;

  qint64 positionStart = 0;
  qint64 positionEnd = -1;
  qint64 lineStart = 0;
  qint64 lineEnd = -1;

  int minLineSize = 10;
  int maxLineSize = 1000;
  int lineSize = 1000;

  QMutex mutex;

private slots:
  void addData(const QStringList& newLines);
  void onContentProcessFinished();

signals:
  void endOfFileSmallerThanPosition();
  void endOfFileSmallerThanPositionStart();
  void errorReadingFile();
  void notEnoughLines(int startLine, int lineCount);

};
