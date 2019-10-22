#include "src/gui/core/HighlightingGroup.h"

#include "src/gui/core/Configuration.h"
#include "src/gui/core/HighlightingRule.h"

#include <QDebug>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
HighlightingGroup::HighlightingGroup(const QString& id, const QString& name) :
  id(id),
  name(name)
{
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
HighlightingGroup::~HighlightingGroup()
{
  qDeleteAll(rules);
  rules.clear();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingGroup::addRule(HighlightingRule* rule)
{
  rules << rule;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingGroup::addRules(const QVector<HighlightingRule*>& rules)
{
  this->rules = rules;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingGroup::deleteRule(const QString& ruleId)
{
  for (HighlightingRule* rule : rules)
  {
    if (rule->getId() == ruleId)
    {
      rules.removeAll(rule);
      delete rule;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString HighlightingGroup::getId() const
{
  return id;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString HighlightingGroup::getName() const
{
  return name;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
HighlightingRule* HighlightingGroup::getRule(const QString& ruleId) const
{
  for (HighlightingRule* rule : rules)
  {
    if (rule->getId() == ruleId)
    {
      return rule;
    }
  }

  return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QVector<HighlightingRule*> HighlightingGroup::getRules() const
{
  return rules;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingGroup::moveRule(int oldPosition, int newPosition)
{
  rules.move(oldPosition, newPosition);

  for (int i = 0; i < rules.size(); ++i)
  {
    HighlightingRule* rule = rules[i];
    rule->setSort(i + 1);

    Config().updateHighlightingRule(rule);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void HighlightingGroup::updateRule(HighlightingRule* newRule)
{
  for (HighlightingRule* rule : rules)
  {
    if (rule->getId() == id)
    {
      rule->setEnabled(newRule->isEnabled());
      rule->setMatchCase(newRule->isMatchCase());
      rule->setWholeWord(newRule->isWholeWord());
      rule->setForegroundColor(newRule->getForegroundColor());
      rule->setBackgroundColor(newRule->getBackgroundColor());
    }
  }
}