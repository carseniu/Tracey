#include "src/gui/main/IntValidator.h"


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
IntValidator::IntValidator(int min, int max, QObject* parent) :
  QIntValidator(min, max, parent)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QValidator::State IntValidator::validate(QString& input, int& pos) const
{
  if (input.isEmpty()) {
    return QValidator::Intermediate;
  }

  int value = input.toInt();

  if (value >= bottom() && value <= top())
  {
    return QValidator::Acceptable;
  }
  else
  {
    return QValidator::Invalid;
  }
}