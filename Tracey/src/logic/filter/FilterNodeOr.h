#pragma once

#include "src/logic/filter/FilterNode.h"


class FilterNodeOr : public FilterNode
{
  Q_OBJECT

public:
  FilterNodeOr(FilterNode* left, FilterNode* right);
  ~FilterNodeOr();

  bool evaluate() override;

private:
  FilterNode* left;
  FilterNode* right;

};