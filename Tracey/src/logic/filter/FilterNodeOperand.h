#pragma once

#include "src/logic/filter/FilterNode.h"

#include <QRegularEXpression>

class Filter;


class FilterNodeOperand : public FilterNode
{
  Q_OBJECT

public:
  FilterNodeOperand(const QRegularExpression& regexp, Filter* filter);

  bool evaluate() override;

private:
  QRegularExpression regexp;
  Filter* filter;

};