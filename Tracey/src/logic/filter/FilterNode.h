#pragma once

#include <QObject>


class FilterNode : public QObject
{
  Q_OBJECT

public:
  FilterNode();
  virtual ~FilterNode();

  virtual bool evaluate() = 0;

};