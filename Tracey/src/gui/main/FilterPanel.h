#pragma once


#include "src/gui/main/TracePanel.h"

#include "src/Tracey.h"
#include "src/gui/main/FilterWidget.h"

class FilterTextEdit;
class IconButton;

class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;


class FilterPanel : public TracePanel
{
  Q_OBJECT

public:
  FilterPanel(FilterData* filterData, const FilterInfo& filterInfo, HighlightingData* highlightingData, FilterWidget* parent);

  void addOperator(const QChar& filterOperator);
  int getExtraLinesAfter() const;
  int getExtraLinesBefore() const;
  QString getFilterText() const;
  QVector<int> getOperatorPositions() const;
  Tracey::SearchType getSearchType() const;
  bool isMatchCase() const;
  bool isSourceLines() const;
  bool isWholeWord() const;
  void setFocus();
  void toggleStartStop();
  void updateShortcuts() override;

private:
  void disableControls();
  void disableWholeWord(bool checked);
  void enableControls();
  void init(const FilterInfo& filterInfo);
  void setControlButtonTooltip();

  FilterData* filterData;

  IconButton* andButton;
  IconButton* orButton;
  IconButton* notButton;
  IconButton* openBracketButton;
  IconButton* closedBracketButton;

  IconButton* matchCaseButton;
  IconButton* wholeWordButton;

  IconButton* controlButton;

  QCheckBox* sourceLinesCheckBox;

  QGroupBox* searchModeGroupBox;
  QRadioButton* normalSearchRadioButton;
  QRadioButton* regexpSearchRadioButton;
  QRadioButton* wildcardSearchRadioButton;

  QGroupBox* extraLinesGroupBox;
  QLineEdit* extraLinesBeforeLineEdit;
  QLineEdit* extraLinesAfterLineEdit;

  FilterTextEdit* filterTextEdit;
  QLabel* errorLabel;

  QString controlButtonTooltip = "%1 (%2)";
  QString controlButtonState = tr("Start");
  QString controlButtonShortcut = "Control+Return";

private slots:
  void onAddAnd();
  void onAddClosedBracket();
  void onAddNot();
  void onAddOpenBracket();
  void onAddOr();
  void onControlButtonClicked();
  void onFilterErrorFound(const QString& message, int position);
  void onFilterStarted(TraceData* traceData);
  void onFilterStopped(TraceData* traceData);

signals:
  void sourceLinesToggled(bool checked);

};