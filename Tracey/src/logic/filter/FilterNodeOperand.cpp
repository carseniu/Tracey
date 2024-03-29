#include "src/logic/filter/FilterNodeOperand.h"

#include "src/logic/filter/Filter.h"

#include <QDebug>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterNodeOperand::FilterNodeOperand(const QRegularExpression& regexp, Filter* filter) :
  regexp(regexp),
  filter(filter)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool FilterNodeOperand::evaluate()
{
  return regexp.match(filter->getText()).hasMatch();
}