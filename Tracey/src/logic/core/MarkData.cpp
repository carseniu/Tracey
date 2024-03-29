#include "src/logic/core/MarkData.h"

#include "src/logic/core/TraceData.h"

#include <QDebug>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
MarkData::MarkData(TraceData* traceData) :
  QObject(traceData),
  traceData(traceData)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void MarkData::add(int index)
{
  if (contains(index) || data.size() == 500)
  {
    return;
  }

  Mark mark;
  mark.index = index;

  QString text = traceData->getLine(index).left(200);
  if (text.right(1) == "\n")
  {
    text = text.left(text.size() - 1);
  }

  mark.text = text;

  data.append(mark);

  std::sort(data.begin(), data.end());

  emit marksChanged(index);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void MarkData::add(const QList<int> indexes)
{
  for (int index : indexes)
  {
    add(index);

    if (data.size() == 100)
    {
      return;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void MarkData::clear()
{
  data.clear();

  emit marksChanged();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool MarkData::contains(int index) const
{
  for (const Mark& mark : data)
  {
    if (mark.index == index)
    {
      return true;
    }
  }

  return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool MarkData::contains(int startIndex, int endIndex) const
{
  for (int i = startIndex; i <= endIndex; ++i)
  {
    if (contains(i))
    {
      return true;
    }
  }

  return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
const QVector<Mark>& MarkData::getAll() const
{
  return data;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int MarkData::getNext(int index) const
{
  for (int i = 0; i < data.size(); ++i)
  {
    if (data.at(i).index > index)
    {
      return data.at(i).index;
    }
  }

  if (data.size() > 0)
  {
    return data.at(0).index;
  }

  return index;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int MarkData::getPrevious(int index) const
{
  for (int i = data.size() - 1; i >= 0; --i)
  {
    if (data.at(i).index < index)
    {
      return data.at(i).index;
    }
  }

  if (data.size() > 0)
  {
    return data.at(data.size() - 1).index;
  }

  return index;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void MarkData::toggle(int index)
{
  if (contains(index))
  {
    remove(index);

    emit marksChanged();
  }
  else
  {
    add(index);
  }
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void MarkData::remove(int index)
{
  for (int i = 0; i < data.size(); ++i)
  {
    if (data.at(i).index == index)
    {
      data.removeAt(i);
      break;
    }
  }
}