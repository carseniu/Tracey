#pragma once

#include <QObject>

#include <QVector>

class TraceData;

struct Mark
{
  qint64 index;
  QString text;

  bool operator<(const Mark& other) const
  {
    return index < other.index;
  }

  operator QString() const 
  { 
    return "[" + QString::number(index) + ", " + text + "]"; 
  }
};


class MarkData : public QObject
{
  Q_OBJECT

public:
  MarkData(TraceData* traceData);

  void add(int index);
  void add(const QList<int> indexes);
  void clear();
  bool contains(int index) const;
  bool contains(int startIndex, int endIndex) const;
  const QVector<Mark>& getAll() const;
  int getNext(int index) const;
  int getPrevious(int index) const;
  void toggle(int index);
  
private:
  void remove(int index);

  TraceData* traceData;

  QVector<Mark> data;

signals:
  void marksChanged(int newIndex = -1);

};
