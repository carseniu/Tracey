#pragma once

#include<QVector>

class HighlightingRule;


class HighlightingGroup 
{

public:
  HighlightingGroup(const QString& id, const QString& name);
  ~HighlightingGroup();

  void addRule(HighlightingRule* rule);
  void addRules(const QVector<HighlightingRule*>& rules);
  void deleteRule(const QString& ruleId);
  QString getId() const;
  QString getName() const;
  HighlightingRule* getRule(const QString& ruleId) const;
  QVector<HighlightingRule*> getRules() const;
  void moveRule(int oldPosition, int newPosition);
  void updateRule(HighlightingRule* newRule);

private:
  QString id;
  QString name;

  QVector<HighlightingRule*> rules;

};