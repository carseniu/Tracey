#include "src/gui/core/Configuration.h"

#include "src/Tracey.h"
#include "src/gui/core/HighlightingGroup.h"
#include "src/gui/core/HighlightingRule.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::addFilter(const FilterInfo& filterInfo) const
{
  QString sql = "INSERT INTO Filter (Id, traceId, Name, Position, SearchType, MatchCase, WholeWord, SourceLines, ExtraLinesBefore, ExtraLinesAfter, Expression, HighlightingGroupId, Autoscroll) \
                   VALUES (:id, :traceId, :name, :position, :searchType, :matchCase, :wholeWord, :sourceLines, :extraLinesBefore, :extraLinesAfter, :expression, :highlightingGroupId, :autoscroll)";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":id", filterInfo.id);
  query.bindValue(":traceId", filterInfo.traceId);
  query.bindValue(":name", filterInfo.name);
  query.bindValue(":position", filterInfo.position);
  query.bindValue(":searchType", filterInfo.searchType);
  query.bindValue(":matchCase", filterInfo.matchCase);
  query.bindValue(":wholeWord", filterInfo.wholeWord);
  query.bindValue(":sourceLines", filterInfo.sourceLines);
  query.bindValue(":extraLinesBefore", filterInfo.extraLinesBefore);
  query.bindValue(":extraLinesAfter", filterInfo.extraLinesAfter);
  query.bindValue(":expression", filterInfo.expression);
  query.bindValue(":highlightingGroupId", filterInfo.highlightingGroupId);
  query.bindValue(":autoscroll", filterInfo.autoscroll);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  addOperatorPositions(filterInfo);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::addHighlightingGroup(HighlightingGroup* group) const
{
  QString sql = "INSERT INTO HighlightingGroup (Id, Name) VALUES (:id, :name)";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":id", group->getId());
  query.bindValue(":name", group->getName());

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::addHighlightingRule(HighlightingRule* rule, const QString& groupId) const
{
  QString sql = "INSERT INTO HighlightingRule (Id, HighlightingGroupId, Item, Enabled, MatchCase, WholeWord, ForegroundColor, BackgroundColor, Sort) \
                   VALUES (:id, :highlightingGroupId, :item, :enabled, :matchCase, :wholeWord, :foregroundColor, :backgroundColor, (SELECT IFNULL(MAX(Sort) + 1, 1) FROM HighlightingRule))";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":id", rule->getId());
  query.bindValue(":highlightingGroupId", groupId);
  query.bindValue(":item", rule->getItem());
  query.bindValue(":enabled", rule->isEnabled());
  query.bindValue(":matchCase", rule->isMatchCase());
  query.bindValue(":wholeWord", rule->isWholeWord());
  query.bindValue(":foregroundColor", rule->getForegroundColor());
  query.bindValue(":backgroundColor", rule->getBackgroundColor());

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::addTrace(const TraceInfo& traceInfo) const
{
  QString sql = "INSERT INTO Trace (Id, Name, Position, Source, RangeType, OpenStart, OpenEnd, LineHandling, LineSize, PreloadContents, HighlightingGroupId, Autoscroll, CurrentIndex) \
                   VALUES (:id, :name, :position, :source, :rangeType, :openStart, :openEnd, :lineHandling, :lineSize, :preloadContents, :highlightingGroupId, :autoscroll, 0)";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":id", traceInfo.id);
  query.bindValue(":name", traceInfo.name);
  query.bindValue(":position", traceInfo.position);
  query.bindValue(":source", traceInfo.source);
  query.bindValue(":rangeType", traceInfo.rangeType);
  query.bindValue(":openStart", traceInfo.openStart);
  query.bindValue(":openEnd", traceInfo.openEnd);
  query.bindValue(":lineHandling", traceInfo.lineHandling);
  query.bindValue(":lineSize", traceInfo.lineSize);
  query.bindValue(":preloadContents", traceInfo.preloadContents);
  query.bindValue(":highlightingGroupId", traceInfo.highlightingGroupId);
  query.bindValue(":autoscroll", traceInfo.autoscroll);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::deleteFilter(const QString& filterId) const
{
  QString sql = "DELETE FROM Filter WHERE Id = :filterId";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":filterId", filterId);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::deleteHighlightingGroup(const QString& groupId) const
{
  QString sql = "DELETE FROM HighlightingGroup WHERE Id = :groupId";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":groupId", groupId);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::deleteHighlightingRule(const QString& ruleId) const
{
  QString sql = "DELETE FROM HighlightingRule WHERE Id = :ruleId";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":ruleId", ruleId);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::deleteHistory(const HistoryModel::Type& type) const
{
  QString sql = "DELETE FROM " + getHistoryName(type) + "History";
  QSqlQuery query;
  query.prepare(sql);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::deleteTrace(const QString& traceId) const
{
  QString sql = "DELETE FROM Trace WHERE Id = :traceId";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":traceId", traceId);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool Configuration::getBoolProperty(const QString& id)
{
  return settingsMap[id].toInt();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int Configuration::getCurrentTraceWidgetIndex(const QString& id) const
{
  int result = 0;

  QString sql = "SELECT CurrentIndex FROM Trace WHERE Id = :id";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":id", id);

  if (query.exec())
  {
    if (query.next())
    {
      result = query.value("CurrentIndex").toInt();
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int Configuration::getCurrentTraceWindowIndex() const
{
  int result = 0;

  QString sql = "SELECT CurrentTraceWindowIndex FROM MainWindow";
  QSqlQuery query;
  query.prepare(sql);

  if (query.exec())
  {
    if (query.next())
    {
      result = query.value("CurrentTraceWindowIndex").toInt();

      if (result < 0)
      {
        result = 0;
      }
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QVector<FilterInfo> Configuration::getFilters(const QString& traceId) const
{
  QVector<FilterInfo> filters;

  QString sql = "SELECT Id, Name, SearchType, MatchCase, WholeWord, SourceLines, ExtraLinesBefore, ExtraLinesAfter, Expression, HighlightingGroupId, Autoscroll FROM Filter WHERE traceId = :traceId ORDER BY Position";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":traceId", traceId);

  if (query.exec())
  {
    while (query.next())
    {
      FilterInfo filterInfo;

      filterInfo.id = query.value("Id").toString();
      filterInfo.name = query.value("Name").toString();
      filterInfo.searchType = static_cast<SearchType>(query.value("SearchType").toInt());
      filterInfo.matchCase = query.value("MatchCase").toBool();
      filterInfo.wholeWord = query.value("WholeWord").toBool();
      filterInfo.sourceLines = query.value("SourceLines").toBool();
      filterInfo.extraLinesBefore = query.value("ExtraLinesBefore").toInt();
      filterInfo.extraLinesAfter = query.value("ExtraLinesAfter").toInt();
      filterInfo.expression = query.value("Expression").toString();
      filterInfo.highlightingGroupId = query.value("HighlightingGroupId").toString();
      filterInfo.autoscroll = query.value("Autoscroll").toBool();
      filterInfo.operatorPositions = getOperatorPositions(filterInfo.id);

      filters << filterInfo;
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return filters;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QVector<HighlightingGroup*> Configuration::getHighlightingGroups() const
{
  QVector<HighlightingGroup*> groups;

  QString sql = "SELECT Id, Name FROM HighlightingGroup";
  QSqlQuery query;
  query.prepare(sql);

  if (query.exec())
  {
    while (query.next())
    {
      QString id = query.value("Id").toString();
      QString name = query.value("Name").toString();

      HighlightingGroup* group = new HighlightingGroup(id, name);

      QVector<HighlightingRule*> rules = Config().getHighlightingRules(group->getId());
      group->addRules(rules);

      groups << group;
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return groups;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QVector<HighlightingRule*> Configuration::getHighlightingRules(const QString& groupId) const
{
  QVector<HighlightingRule*> rules;

  QString sql = "SELECT Id, Item, Enabled, MatchCase, WholeWord, ForegroundColor, BackgroundColor FROM HighlightingRule WHERE HighlightingGroupId = :groupId ORDER BY Sort";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":groupId", groupId);

  if (query.exec())
  {
    while (query.next())
    {
      QString id = query.value("Id").toString();
      QString item = query.value("Item").toString();
      bool enabled = query.value("Enabled").toBool();
      bool matchCase = query.value("MatchCase").toBool();
      bool wholeWord = query.value("WholeWord").toBool();
      QString foregroundColor = query.value("ForegroundColor").toString();
      QString backgroundColor = query.value("BackgroundColor").toString();

      HighlightingRule* rule = new HighlightingRule(id, item, foregroundColor, backgroundColor, matchCase, wholeWord, enabled);
      rules << rule;
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return rules;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int Configuration::getHighlightingRulesMaxSort(const QString& groupId) const
{
  int result = 0;

  QString sql = "SELECT IFNULL(MAX(Sort),  0) MaxSort FROM HighlightingRule WHERE HighlightingGroupId = :groupId";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":groupId", groupId);

  if (query.exec())
  {
    if (query.next())
    {
      result = query.value("MaxSort").toInt();
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QRect Configuration::getMainWindowGeometry() const
{
  QRect result(0, 0, 1600, 1000);

  QString sql = "SELECT Top, Left, Width, Height FROM MainWindow";
  QSqlQuery query;
  query.prepare(sql);

  if (query.exec())
  {
    if (query.next())
    {
      result = QRect(query.value("Left").toInt(), query.value("Top").toInt(), query.value("Width").toInt(), query.value("Height").toInt());
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QByteArray Configuration::getMainWindowSplitterState() const
{
  QByteArray result = "";

  QString sql = "SELECT SplitterState FROM MainWindow";
  QSqlQuery query;
  query.prepare(sql);

  if (query.exec())
  {
    if (query.next())
    {
      result = query.value("SplitterState").toByteArray();
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QStringList Configuration::getHistory(const HistoryModel::Type& type) const
{
  QStringList result;

  QString sql = "SELECT Item FROM " + getHistoryName(type) + "History ORDER BY Sort";
  QSqlQuery query;
  query.prepare(sql);

  if (query.exec())
  {
    while (query.next())
    {
      result << query.value("Item").toString();
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchInfo Configuration::getSearchInfo() const
{
  SearchInfo searchInfo;

  QString sql = "SELECT SearchType, MatchCase, WholeWord, GoTo FROM SearchInfo";
  QSqlQuery query;
  query.prepare(sql);

  if (query.exec())
  {
    if (query.next())
    {
      searchInfo.searchType = static_cast<SearchType>(query.value("SearchType").toInt());
      searchInfo.matchCase = query.value("MatchCase").toBool();
      searchInfo.wholeWord = query.value("WholeWord").toBool();
      searchInfo.goTo = query.value("GoTo").toString();
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return searchInfo;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString Configuration::getShortcut(int id) const
{
  if (id < 0 || id >= shortcutList.size())
  {
    return "";
  }

  return shortcutList.at(id);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QVector<TraceInfo> Configuration::getTraces() const
{
  QVector<TraceInfo> sources;

  QString sql = "SELECT Id, Name, Source, RangeType, OpenStart, OpenEnd, LineHandling, LineSize, PreloadContents, HighlightingGroupId, Autoscroll FROM Trace ORDER BY Position";
  QSqlQuery query;
  query.prepare(sql);

  if (query.exec())
  {
    while (query.next())
    {
      TraceInfo traceInfo;

      traceInfo.id = query.value("Id").toString();
      traceInfo.name = query.value("Name").toString();
      traceInfo.source = query.value("Source").toString();
      traceInfo.rangeType = query.value("RangeType").toInt();
      traceInfo.openStart = query.value("OpenStart").toString();
      traceInfo.openEnd = query.value("OpenEnd").toString();
      traceInfo.lineHandling = query.value("LineHandling").toInt();
      traceInfo.lineSize = query.value("LineSize").toString();
      traceInfo.preloadContents = query.value("PreloadContents").toBool();
      traceInfo.highlightingGroupId = query.value("HighlightingGroupId").toString();
      traceInfo.autoscroll = query.value("Autoscroll").toBool();

      sources << traceInfo;
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return sources;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool Configuration::isMainWindowMaximized() const
{
  bool result = false;

  QString sql = "SELECT Maximized FROM MainWindow";
  QSqlQuery query;
  query.prepare(sql);

  if (query.exec())
  {
    if (query.next())
    {
      result = query.value("Maximized").toBool();
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::saveCurrentTraceWidgetIndex(const QString& id, int index) const
{
  QString sql = "UPDATE Trace SET CurrentIndex = :currentIndex WHERE Id = :id";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":currentIndex", index);
  query.bindValue(":id", id);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::saveCurrentTraceWindowIndex(int index) const
{
  QString sql = "UPDATE MainWindow SET CurrentTraceWindowIndex = :currentTraceWindowIndex";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":currentTraceWindowIndex", index);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::saveHistory(const HistoryModel::Type& type, const QStringList& stringList) const
{
  QString sql = "INSERT INTO " + getHistoryName(type) + "History VALUES (?, ?)";

  QSqlQuery query;
  query.prepare(sql);

  QVariantList itemList;
  QVariantList sortList;

  for (int i = 0; i < stringList.size(); ++i)
  {
    itemList << stringList.at(i);
    sortList << i + 1;
  }

  query.addBindValue(itemList);
  query.addBindValue(sortList);

  if (!query.execBatch())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::saveMainWindowState(const QRect& geometry, bool maximized, const QByteArray& splitterState) const
{
  QString sql = "UPDATE MainWindow SET Top = :top, Left = :left, Width = :width, Height = :height, Maximized = :maximized, SplitterState = :splitterState";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":top", geometry.y());
  query.bindValue(":left", geometry.x());
  query.bindValue(":width", geometry.width());
  query.bindValue(":height", geometry.height());
  query.bindValue(":maximized", maximized);
  query.bindValue(":splitterState", splitterState);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::saveSearchInfo(const SearchInfo& searchInfo)
{
  QString sql = "UPDATE SearchInfo SET SearchType = :searchType, MatchCase = :matchCase, WholeWord = :wholeWord, GoTo = :goTo";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":searchType", searchInfo.searchType);
  query.bindValue(":matchCase", searchInfo.matchCase);
  query.bindValue(":wholeWord", searchInfo.wholeWord);
  query.bindValue(":goTo", searchInfo.goTo);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::saveShortcuts(const QStringList& shortcutList)
{
  this->shortcutList = shortcutList;

  QString sql = "UPDATE Shortcuts SET Value = ? WHERE Id = ?";

  QSqlQuery query;
  query.prepare(sql);

  QVariantList valueList;
  QVariantList idList;

  for (int i = 0; i < shortcutList.size(); ++i)
  {
    valueList << shortcutList.at(i);
    idList << i;
  }

  query.addBindValue(valueList);
  query.addBindValue(idList);

  if (!query.execBatch())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::setProperty(const QString& id, const QString& value)
{
  settingsMap[id] = value;

  QString sql = "UPDATE Settings SET VALUE = :value WHERE Id = :id";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":value", value);
  query.bindValue(":id", id);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::updateFilter(const FilterInfo& filterInfo) const
{
  QString sql = "UPDATE Filter SET Name = :name, Position = :position, SearchType = :searchType, MatchCase = :matchCase, WholeWord = :wholeWord, SourceLines = :sourceLines, ExtraLinesBefore = :extraLinesBefore, \
                   ExtraLinesAfter = :extraLinesAfter, Expression = :expression, HighlightingGroupId = :highlightingGroupId, Autoscroll = :autoscroll WHERE Id = :id";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":name", filterInfo.name);
  query.bindValue(":position", filterInfo.position);
  query.bindValue(":searchType", filterInfo.searchType);
  query.bindValue(":matchCase", filterInfo.matchCase);
  query.bindValue(":wholeWord", filterInfo.wholeWord);
  query.bindValue(":sourceLines", filterInfo.sourceLines);
  query.bindValue(":extraLinesBefore", filterInfo.extraLinesBefore);
  query.bindValue(":extraLinesAfter", filterInfo.extraLinesAfter);
  query.bindValue(":expression", filterInfo.expression);
  query.bindValue(":highlightingGroupId", filterInfo.highlightingGroupId);
  query.bindValue(":autoscroll", filterInfo.autoscroll);
  query.bindValue(":id", filterInfo.id);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  deleteOperatorPositions(filterInfo.id);

  addOperatorPositions(filterInfo);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::updateFilterPositions(const QVariantList& ids, const QVariantList& positions, const QVariantList& traceIds)
{
  QString sql = "UPDATE Filter SET Position = ? WHERE Id = ? AND TraceId = ?";
  QSqlQuery query;
  query.prepare(sql);
  query.addBindValue(positions);
  query.addBindValue(ids);
  query.addBindValue(traceIds);

  if (!query.execBatch())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::updateHighlightingRule(HighlightingRule* rule) const
{
  QString sql = "UPDATE HighlightingRule SET Item = :item, Enabled = :enabled, MatchCase = :matchCase, WholeWord = :wholeWord, ForegroundColor = :foregroundColor, BackgroundColor = :backgroundColor, Sort = :sort WHERE Id = :id";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":item", rule->getItem());
  query.bindValue(":enabled", rule->isEnabled());
  query.bindValue(":matchCase", rule->isMatchCase());
  query.bindValue(":wholeWord", rule->isWholeWord());
  query.bindValue(":foregroundColor", rule->getForegroundColor());
  query.bindValue(":backgroundColor", rule->getBackgroundColor());
  query.bindValue(":sort", rule->getSort());
  query.bindValue(":id", rule->getId());

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::updateTrace(const TraceInfo& traceInfo) const
{
  QString sql = "UPDATE Trace SET Name = :name, Position = :position, Source = :source, RangeType = :rangeType, OpenStart = :openStart, OpenEnd = :openEnd, LineHandling = :lineHandling, \
                   LineSize = :lineSize, PreloadContents = :preloadContents, HighlightingGroupId = :highlightingGroupId, Autoscroll = :autoscroll WHERE Id = :id";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":name", traceInfo.name);
  query.bindValue(":position", traceInfo.position);
  query.bindValue(":source", traceInfo.source);
  query.bindValue(":rangeType", traceInfo.rangeType);
  query.bindValue(":openStart", traceInfo.openStart);
  query.bindValue(":openEnd", traceInfo.openEnd);
  query.bindValue(":lineHandling", traceInfo.lineHandling);
  query.bindValue(":lineSize", traceInfo.lineSize);
  query.bindValue(":preloadContents", traceInfo.preloadContents);
  query.bindValue(":highlightingGroupId", traceInfo.highlightingGroupId);
  query.bindValue(":autoscroll", traceInfo.autoscroll);
  query.bindValue(":id", traceInfo.id);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::updateTracePositions(const QVariantList& ids, const QVariantList& positions)
{
  QString sql = "UPDATE Trace SET Position = ? WHERE Id = ?";
  QSqlQuery query;
  query.prepare(sql);
  query.addBindValue(positions);
  query.addBindValue(ids);

  if (!query.execBatch())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
Configuration::Configuration()
{
  bool databaseExists = true;

  QString appDataPath =  QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir appDataDir(appDataPath);
  if (!appDataDir.exists())
  {
    appDataDir.mkdir(appDataPath);
  }

  QString databasePath = appDataDir.absolutePath() + QDir::separator() + "Tracey.dat";

  if (!QFile::exists(databasePath))
  {
    databaseExists = false;
  }

  database = QSqlDatabase::addDatabase("QSQLITE");
  database.setDatabaseName(databasePath);
  bool ok = database.open();

  if (ok)
  {
    // Create the database if the database is missing
    if (!databaseExists)
    {
      createDatabase();
    }

    setApplicationData();
    enableForeignKeys();

    loadSettings();

    qDebug() << "Settings database open successfully";
  }
  else
  {
    qDebug() << "ERROR - Could not create settings database: " << database.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
Configuration::~Configuration()
{
  database.close();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::addOperatorPositions(const FilterInfo& filterInfo) const
{
  QString sql = "INSERT INTO OperatorPosition (FilterId, Position) VALUES (:filterId, :position)";
  QSqlQuery query;
  query.prepare(sql);

  QVariantList filterIdList;
  for (int i = 0; i < filterInfo.operatorPositions.size(); ++i)
  {
    filterIdList << filterInfo.id;
  }
  query.addBindValue(filterIdList);

  QVariantList positionList;
  for (int position : filterInfo.operatorPositions)
  {
    positionList << position;
  }
  query.addBindValue(positionList);

  if (!query.execBatch())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::createDatabase() const
{
  QFile script(":/Application/cfg/Configuration.sql");

  if (!script.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug() << "ERROR - Could not open settings database creation file";

    return;
  }

  QByteArray sql = script.readAll();
  QStringList queries = QString(sql).split(QChar(';'), QString::SkipEmptyParts);

  QSqlQuery query;

  for (int i = 0; i < queries.size(); ++i)
  {
    bool ok = query.exec(queries.at(i));

    if (!ok)
    {
      qDebug() << "ERROR - Query execution error: " << queries.at(i) << query.lastError();
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::deleteOperatorPositions(const QString& filterId) const
{
  QString sql = "DELETE FROM OperatorPosition WHERE FilterId = :filterId";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":filterId", filterId);

  if (!query.exec())
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::enableForeignKeys() const
{
  QString sql = "PRAGMA foreign_keys = ON";
  QSqlQuery query;

  if (!query.exec(sql))
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString Configuration::getHistoryName(const HistoryModel::Type& type) const
{
  switch (type)
  {
  case HistoryModel::Source:
    return "Source";
  case HistoryModel::Search:
    return "Search";
  default:
    return "";
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QVector<int> Configuration::getOperatorPositions(const QString& filterId) const
{
  QVector<int> operatorPositions;

  QString sql = "SELECT Position FROM OperatorPosition WHERE FilterId = :filterId ORDER BY Position";
  QSqlQuery query;
  query.prepare(sql);
  query.bindValue(":filterId", filterId);

  if (query.exec())
  {
    while (query.next())
    {
      operatorPositions << query.value("Position").toInt();
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }

  return operatorPositions;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::loadSettings()
{
  QString sql = "SELECT Id, Value FROM Settings";
  QSqlQuery query;
  query.prepare(sql);

  if (query.exec())
  {
    while (query.next())
    {
      settingsMap[query.value("Id").toString()] = query.value("Value").toString();
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }


  sql = "SELECT Id, Value FROM Shortcuts ORDER BY Id";
  query.prepare(sql);

  if (query.exec())
  {
    while (query.next())
    {
      shortcutList.append(query.value("Value").toString());
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Configuration::setApplicationData()
{
  QSqlQuery query;
  QString sql = "SELECT Version FROM Application";

  if (query.exec(sql))
  {
    if (query.next())
    {
      version = query.value("Version").toString();
    }
  }
  else
  {
    qDebug() << "ERROR - Query execution error: " << sql << query.lastError();
  }
}


//===================================================================================================================================================================================================//
// FRIEND
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
Configuration& Config()
{
  static Configuration conf;
  return conf;
};