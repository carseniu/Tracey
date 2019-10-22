#pragma once

#include <QFrame>

class IconButton;
class List;

class Application;
class Button;
class ColorButton;
class ComboBox;
class HighlightingData;
class HighlightingRule;
class QCheckBox;
class QLineEdit;


class HighlightingWidget : public QFrame
{
  Q_OBJECT

public:
  HighlightingWidget(Application* application);

private:
  void disableRuleToolbar();
  void enableRuleToolbar();
  QColor getColor(const QString& currentColor, const QString& title);
  HighlightingRule* getCurrentRule() const;
  void updateRule(HighlightingRule* rule);

  HighlightingData* highlightingData;

  IconButton* addGroupButton;
  IconButton* deleteGroupButton;

  ComboBox* groupComboBox;

  QCheckBox* enabledCheckBox;
  IconButton* addRuleButton;
  IconButton* deleteRuleButton;
  ColorButton* foregroundColorButton;
  ColorButton* backgroundColorButton;
  IconButton* matchCaseButton;
  IconButton* wholeWordButton;
  IconButton* findPreviousButton;
  IconButton* findNextButton;

  List* ruleList;

private slots:
  void findNext();
  void findPrevious();
  void onItemRowChanged(int oldPosition, int newPosition);
  void processAddGroup();
  void processAddRule();
  void processDeleteGroup();
  void processDeleteRule();
  void processGroupChanged(int index);
  void processRuleChanged(int index);
  void setBackgroundColor();
  void setEnable(bool enabled);
  void setForegroundColor();
  void setGroups();
  void setMatchCase(bool enabled);
  void setWholeWord(bool enabled);

signals:
  void findNextRequested(const QRegularExpression& regexp);
  void findPreviousRequested(const QRegularExpression& regexp);

};