#pragma once

#include <QObject>

#include "src/gui/core/HistoryModel.h"
#include "src/gui/main/TraceWidget.h"
#include "src/gui/tools/SearchWidget.h"

#include <QSqlDatabase>
#include <QStringList>

class HighlightingGroup;
class HighlightingRule;


class Configuration : public QObject
{
  Q_OBJECT

public:
  void addFilter(const FilterInfo& filterInfo) const; 
  void addHighlightingGroup(HighlightingGroup* group) const;
  void addHighlightingRule(HighlightingRule* rule, const QString& groupId) const;
  void addTrace(const TraceInfo& traceInfo) const;
  void deleteFilter(const QString& filterId) const;
  void deleteHighlightingGroup(const QString& groupId) const;
  void deleteHighlightingRule(const QString& ruleId) const;
  void deleteHistory(const HistoryModel::Type& type) const;
  void deleteTrace(const QString& traceId) const;
  bool getBoolProperty(const QString& id);
  int getCurrentTraceWidgetIndex(const QString& id) const;
  int getCurrentTraceWindowIndex() const;
  QVector<FilterInfo> getFilters(const QString& traceId) const;
  QVector<HighlightingGroup*> getHighlightingGroups() const;
  QVector<HighlightingRule*> getHighlightingRules(const QString& groupId) const;
  int getHighlightingRulesMaxSort(const QString& groupId) const;
  QRect getMainWindowGeometry() const;
  QByteArray getMainWindowSplitterState() const;
  QStringList getHistory(const HistoryModel::Type& type) const;
  SearchInfo getSearchInfo() const;
  QString getShortcut(int id) const;
  QVector<TraceInfo> getTraces() const;
  bool isMainWindowMaximized() const;
  void saveCurrentTraceWidgetIndex(const QString& id, int index) const;
  void saveCurrentTraceWindowIndex(int index) const;
  void saveHistory(const HistoryModel::Type& type, const QStringList& stringList) const;
  void saveMainWindowState(const QRect& geometry, bool maximized, const QByteArray& splitterState) const;
  void saveSearchInfo(const SearchInfo& searchInfo);
  void saveShortcuts(const QStringList& shortcutList);
  void setProperty(const QString& id, const QString& value);
  void updateFilter(const FilterInfo& filterInfo) const;
  void updateFilterPositions(const QVariantList& ids, const QVariantList& positions, const QVariantList& traceIds);
  void updateHighlightingRule(HighlightingRule* rule) const;
  void updateTrace(const TraceInfo& traceInfo) const;
  void updateTracePositions(const QVariantList& ids, const QVariantList& positions);

private:
  Configuration();
  ~Configuration();
  friend Configuration& Config();

  void addOperatorPositions(const FilterInfo& filterInfo) const;
  void createDatabase() const;
  void deleteOperatorPositions(const QString& filterid) const;
  void enableForeignKeys() const;
  QString getHistoryName(const HistoryModel::Type& type) const;
  QVector<int> getOperatorPositions(const QString& filterId) const;
  void loadSettings();
  void setApplicationData();

  QSqlDatabase database;
  QString version;
  QMap<QString, QString> settingsMap;

  QStringList shortcutList;

};

Configuration& Config();