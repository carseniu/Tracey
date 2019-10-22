#pragma once

#include <QObject>

class TraceData;


class SearchWorker : public QObject
{
  Q_OBJECT

public:
  SearchWorker();
  virtual ~SearchWorker();

  bool isCanceled() const;
  virtual void process() = 0;
  void setCanceled(bool cancel);

protected:
  void doUpdateProgress(int step, int count = 0);

private:
  bool cancel;
  qint64 progressUpdateTime;
  int progressUpdateInterval;

signals:
  void finished();
  void matchFound(int line, int start, int end);
  void matchNotFound();
  void updateReady(int step, int count);

};