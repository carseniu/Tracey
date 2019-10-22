#pragma once

#include <QStringListModel>


class HistoryModel : public QStringListModel
{
  Q_OBJECT

public:
  enum Type
  {
    Source,
    Search
  };

  HistoryModel(const Type& type, QObject* parent);

private:
  Type type;

private slots:
  void processDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
  void processModelReset();
  
};