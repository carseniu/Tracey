#pragma once

#include "src/logic/filter/FilterNode.h"


class FilterNodeNot : public FilterNode
{
  Q_OBJECT

public:
  FilterNodeNot(FilterNode* node);
  ~FilterNodeNot();

  bool evaluate() override;

private:
  FilterNode* node;

};