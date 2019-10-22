#pragma once

#include <QObject>

class TraceData;


class TraceWorker : public QObject
{
  Q_OBJECT

public:
  enum WorkType
  {
    Content,
    Updates
  };

  TraceWorker(bool preload = true);

  void doStop();
  bool isReset();
  bool isRunning();
  void setReset(bool reset);

protected:
  void doUpdateProgress(int step);
  virtual void doWork(WorkType workType) = 0;

private:
  bool preload;

  bool running;
  bool reset;

  qint64 progressUpdateTime;
  int updateInterval;
  int progressUpdateInterval;

public slots:
  void process();

signals:
  void dataFinished();
  void finished();
  void contentProcessFinished();
  void contentProcessStarted();
  void updatesProcessFinished();
  void updatesProcessStarted();
  void stepReady(int step);
  void stopped(TraceData* traceData);

};