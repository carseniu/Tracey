#include "src/logic/search/SearchWorker.h"

#include <QDateTime>
#include <QDebug>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchWorker::SearchWorker() :
  cancel(false),
  progressUpdateInterval(200),
  progressUpdateTime(QDateTime::currentMSecsSinceEpoch())
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchWorker::~SearchWorker()
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SearchWorker::isCanceled() const
{
  return cancel;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWorker::setCanceled(bool cancel)
{
  this->cancel = cancel;
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWorker::doUpdateProgress(int step, int count)
{
  qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
  if (currentTime - progressUpdateTime > progressUpdateInterval)
  {
    emit updateReady(step, count);

    progressUpdateTime = currentTime;
  }
}