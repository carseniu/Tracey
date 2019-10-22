#pragma once

#include <QObject>

#include "src/logic/core/MarkData.h"

#include <QMutex>
#include <QVector>

class MarkData;
class TraceWorker;

class QTimer;


class TraceData : public QObject
{
  Q_OBJECT

public:
  TraceData(const QString& id, const QString& name, QObject* parent = nullptr);
  virtual ~TraceData();

  void addMark(int index);
  void addMarks(const QList<int>& lines);
  virtual void clear() = 0;
  void deleteAll();
  void deleteMarks();
  const QVector<Mark>& getMarks() const;
  QString getId() const;
  virtual QString getLine(int line) = 0;
  virtual int getLineNumber(int index) = 0;
  virtual QString getLineNumber(int index, bool source) = 0;
  virtual QStringList getLines(int start, int count) = 0;
  QString getName() const;
  int getNextMark(int index) const;
  int getPosition() const;
  int getPreviousMark(int index) const;
  virtual int getSize() = 0;
  bool markExists(int index) const;
  bool markExists(int startIndex, int endIndex) const;
  void reset();
  bool saveAs(const QString& filePath);
  void setName(const QString& name);
  void setPosition(int position);
  void setTraceWorker(TraceWorker* traceWorker);
  void start();
  void stop(bool clear);
  void toggleMark(int index);

protected:
  virtual bool prepareWorker() = 0;

private:
  QMutex mutex;

  QString id;
  QString name;
  int position;

  MarkData* markData;

  TraceWorker* traceWorker;

  QTimer* updateTimer;
  int updateInterval;

  int oldDataSize;
  QString oldLastLine;

private slots:
  void deleteWorker();
  void processUpdate();

signals:
  void marksChanged(int newIndex);
  void contentProcessFinished();
  void contentProcessStarted();
  void deleted();
  void nameChanged(const QString& name);
  void sizeChanged(int lines);
  void started(TraceData* traceData);
  void stepReady(int step);
  void stopped(TraceData* traceData);
  void updated();
  void updatesProcessFinished();
  void updatesProcessStarted();

};
