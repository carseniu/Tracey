#pragma once

#include <QObject>

#include<QVector>

#include "src/gui/core/HighlightingGroup.h"

typedef QPair<QString, QString> ColorPair;
typedef QList<ColorPair> ColorPairList;

class HighlightingGroup;
class HighlightingRule;


class HighlightingData : public QObject
{
  Q_OBJECT

public:
  HighlightingData(QObject* parent);
  ~HighlightingData();
  QString addGroup(const QString& name);
  QString addRule(const QString& name, const QString& groupId);
  void deleteGroup(const QString& id);
  void deleteRule(const QString& id, const QString& groupId);
  HighlightingGroup* getGroup(const QString& id);
  QVector<HighlightingGroup*> getGroups() const;
  HighlightingRule* getRule(const QString& ruleId, const QString& groupId);
  QVector<HighlightingRule*> getRules(const QString& groupId);
  void moveRule(int oldPosition, int newPosition, const QString& groupId);
  void updateRule(HighlightingRule* rule, const QString& groupId);
  
private:
  QVector<HighlightingGroup*> groups;

  const ColorPairList DEFAULT_COLORS;

signals:
  void groupsChanged(QVector<HighlightingGroup*> groups);
  void highlightingChanged(const QString& groupId);

};