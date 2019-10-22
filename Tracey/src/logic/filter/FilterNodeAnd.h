#pragma once

#include "src/logic/filter/FilterNode.h"


class FilterNodeAnd : public FilterNode
{
  Q_OBJECT

public:
  FilterNodeAnd(FilterNode* left, FilterNode* right);
  ~FilterNodeAnd();

  bool evaluate() override;

private:
  FilterNode* left;
  FilterNode* right;

};
