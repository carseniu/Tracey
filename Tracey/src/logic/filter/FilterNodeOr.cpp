#include "src/logic/filter/FilterNodeOr.h"


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterNodeOr::FilterNodeOr(FilterNode* left, FilterNode* right) :
  left(left),
  right(right)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterNodeOr::~FilterNodeOr()
{
  delete left;
  delete right;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool FilterNodeOr::evaluate()
{
  return left->evaluate() || right->evaluate();
}