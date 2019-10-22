#pragma once

#include "src/logic/core/TraceData.h"

#include "src/Tracey.h"

class Filter;
class SourceData;


class FilterData : public TraceData
{
  Q_OBJECT

public:
  FilterData(const QString& id, const QString& name, SourceData* sourceData);
  ~FilterData();

  void applyFilter(const QString& expression, const QVector<int>& operatorPositions, Tracey::SearchType searchType, bool matchCase, bool wholeWord, int extraLinesBefore, int extraLinesAfter);
  bool check(const QString& text);
  void clear() override;
  qint64 getData(int index);
  QString getLine(int line) override;
  int getLineNumber(int index) override;
  QString getLineNumber(int index, bool source = false) override;
  QStringList getLines(int start, int count) override;
  QList<int> getLinesForMark(int maxLines);
  int getSize() override;
  QString getTraceId() const;

protected:
  void addData(const QVector<qint64>& data);
  bool prepareWorker() override;

private:
  SourceData* sourceData;
  Filter* filter;

  QVector<qint64> data;

  QString expression = "";
  QVector<int> operatorPositions;
  Tracey::SearchType searchType = Tracey::NormalSearch;
  bool matchCase = false;
  bool wholeWord = false;
  int extraLinesBefore = 0;
  int extraLinesAfter = 0;

  QMutex mutex;

private slots:
  void onLastLineChangedAndNotAccepted(int line);
  void onSourceStopped();

signals:
  void filterErrorFound(const QString& message, int position);

};
