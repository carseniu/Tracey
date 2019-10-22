#include "src/gui/main/FilterPanel.h"

#include "src/Tracey.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/dialogs/MessageDialog.h"
#include "src/gui/main/IntValidator.h"
#include "src/gui/widgets/combobox/ComboBox.h"
#include "src/gui/widgets/filtertextedit/FilterTextEdit.h"
#include "src/gui/widgets/iconbutton/IconButton.h"
#include "src/logic/filter/FilterData.h"

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterPanel::FilterPanel(FilterData* filterData, const FilterInfo& filterInfo, HighlightingData* highlightingData, FilterWidget* parent) :
  TracePanel(highlightingData, parent),
  filterData(filterData)
{
  setFixedHeight(140);

  QFont font = QApplication::font();
  font.setPointSize(8);
  font.setBold(true);

  setHighlightingGroupId(filterInfo.highlightingGroupId);

  andButton = new IconButton(QIcon(":/Application/img/OperatorAnd.png"), this);
  andButton->setFocusPolicy(Qt::NoFocus);
  andButton->setToolTip(tr("AND (Alt-A)"));
  andButton->setFont(font);

  orButton = new IconButton(QIcon(":/Application/img/OperatorOr.png"), this);
  orButton->setToolTip(tr("OR (Alt-O)"));
  orButton->setFont(font);

  notButton = new IconButton(QIcon(":/Application/img/OperatorNot.png"), this);
  notButton->setToolTip(tr("NOT (Alt-N)"));
  notButton->setFont(font);

  openBracketButton = new IconButton(QIcon(":/Application/img/OperatorOpenBracket.png"), this);
  openBracketButton->setToolTip(tr("( (Alt-9)"));
  openBracketButton->setFont(font);

  closedBracketButton = new IconButton(QIcon(":/Application/img/OperatorClosedBracket.png"), this);
  closedBracketButton->setToolTip(tr(") (Alt-0)"));
  closedBracketButton->setFont(font);


  QIcon matchCaseIcon;
  matchCaseIcon.addFile(":/Application/img/MatchCaseOff.png", QSize(), QIcon::Normal, QIcon::Off);
  matchCaseIcon.addFile(":/Application/img/MatchCaseOn.png", QSize(), QIcon::Normal, QIcon::On);
  matchCaseButton = new IconButton(matchCaseIcon, this);
  matchCaseButton->setFocusPolicy(Qt::NoFocus);
  matchCaseButton->setToolTip(tr("Match Case"));
  matchCaseButton->setCheckable(true);

  QIcon wholeWordIcon;
  wholeWordIcon.addFile(":/Application/img/WholeWordOff.png", QSize(), QIcon::Normal, QIcon::Off);
  wholeWordIcon.addFile(":/Application/img/WholeWordOn.png", QSize(), QIcon::Normal, QIcon::On);
  wholeWordButton = new IconButton(wholeWordIcon, this);
  wholeWordButton->setFocusPolicy(Qt::NoFocus);
  wholeWordButton->setToolTip(tr("Whole Word"));
  wholeWordButton->setCheckable(true);

  QIcon icon;
  icon.addFile(":/Application/img/Start.png", QSize(), QIcon::Normal, QIcon::Off);
  icon.addFile(":/Application/img/Stop.png", QSize(), QIcon::Normal, QIcon::On);
  controlButton = new IconButton(icon, this);
  controlButton->setCheckable(true);
  controlButton->setFocusPolicy(Qt::NoFocus);
  controlButtonState = tr("Start");
  setControlButtonTooltip();

  sourceLinesCheckBox = new QCheckBox(tr("Source Lines"), this);

  // SEARCH MODE
  searchModeGroupBox = new QGroupBox(tr("Search Mode"));
  searchModeGroupBox->setFixedWidth(150);

  normalSearchRadioButton = new QRadioButton(tr("Normal"));
  normalSearchRadioButton->setFocusPolicy(Qt::NoFocus);

  wildcardSearchRadioButton = new QRadioButton(tr("Wildcard"));
  wildcardSearchRadioButton->setFocusPolicy(Qt::NoFocus);
  connect(wildcardSearchRadioButton, &QRadioButton::toggled, this, &FilterPanel::disableWholeWord);

  regexpSearchRadioButton = new QRadioButton(tr("Regular Expression"));
  regexpSearchRadioButton->setFocusPolicy(Qt::NoFocus);
  connect(regexpSearchRadioButton, &QRadioButton::toggled, this, &FilterPanel::disableWholeWord);

  QVBoxLayout* searchModeLayout = new QVBoxLayout;
  searchModeLayout->addWidget(normalSearchRadioButton);
  searchModeLayout->addWidget(wildcardSearchRadioButton);
  searchModeLayout->addWidget(regexpSearchRadioButton);
  searchModeGroupBox->setLayout(searchModeLayout);


  // EXTRA LINES
  extraLinesGroupBox = new QGroupBox(tr("Extra Lines"));
  extraLinesGroupBox->setFixedWidth(150);

  QLabel* extraLinesBeforeLabel = new QLabel(tr("Before"), this);

  extraLinesBeforeLineEdit = new QLineEdit(this);
  extraLinesBeforeLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
  extraLinesBeforeLineEdit->setFixedWidth(80);
  extraLinesBeforeLineEdit->setValidator(new IntValidator(1, 1000, this));

  QLabel* extraLinesAfterLabel = new QLabel(tr("After"), this);

  extraLinesAfterLineEdit = new QLineEdit(this);
  extraLinesAfterLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
  extraLinesAfterLineEdit->setFixedWidth(80);
  extraLinesAfterLineEdit->setValidator(new IntValidator(1, 1000, this));

  QGridLayout* extraLinesLayout = new QGridLayout;
  extraLinesLayout->addWidget(extraLinesBeforeLabel, 0, 0);
  extraLinesLayout->addWidget(extraLinesBeforeLineEdit, 0, 1);
  extraLinesLayout->addWidget(extraLinesAfterLabel, 1, 0);
  extraLinesLayout->addWidget(extraLinesAfterLineEdit, 1, 1);
  extraLinesGroupBox->setLayout(extraLinesLayout);


  filterTextEdit = new FilterTextEdit(this);

  errorLabel = new QLabel(this);
  errorLabel->setStyleSheet("font-size:8; color:red");
  errorLabel->setVisible(false);

  QHBoxLayout* controlsLayout = new QHBoxLayout;
  controlsLayout->setContentsMargins(0, 5, 0, 5);
  controlsLayout->setSpacing(5);
  controlsLayout->addWidget(andButton);
  controlsLayout->addWidget(orButton);
  controlsLayout->addWidget(notButton);
  controlsLayout->addWidget(openBracketButton);
  controlsLayout->addWidget(closedBracketButton);
  controlsLayout->addSpacing(30);
  controlsLayout->addWidget(matchCaseButton);
  controlsLayout->addWidget(wholeWordButton);
  controlsLayout->addSpacing(30);
  controlsLayout->addWidget(controlButton);
  controlsLayout->addSpacing(50);
  controlsLayout->addWidget(sourceLinesCheckBox);
  controlsLayout->addStretch();
  controlsLayout->addWidget(getHighlightingWidget());

  QVBoxLayout* filterLayout = new QVBoxLayout;
  filterLayout->setMargin(0);
  filterLayout->setSpacing(3);
  filterLayout->addWidget(filterTextEdit);
  filterLayout->addWidget(errorLabel);

  QHBoxLayout* bottomLayout = new QHBoxLayout; 
  bottomLayout->setContentsMargins(0, 5, 0, 0);
  bottomLayout->setSpacing(5);
  bottomLayout->addLayout(filterLayout);
  bottomLayout->addWidget(searchModeGroupBox);
  bottomLayout->addWidget(extraLinesGroupBox);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(0);
  mainLayout->setSpacing(0);
  mainLayout->addLayout(controlsLayout);
  mainLayout->addSpacing(3);
  mainLayout->addLayout(bottomLayout);
  mainLayout->addSpacing(3);

  init(filterInfo);

  connect(filterData, &FilterData::started, this, &FilterPanel::onFilterStarted);
  connect(filterData, &FilterData::stopped, this, &FilterPanel::onFilterStopped);
  connect(filterData, &FilterData::filterErrorFound, this, &FilterPanel::onFilterErrorFound);
  connect(andButton, &QPushButton::clicked, this, &FilterPanel::onAddAnd);
  connect(orButton, &QPushButton::clicked, this, &FilterPanel::onAddOr);
  connect(notButton, &QPushButton::clicked, this, &FilterPanel::onAddNot);
  connect(openBracketButton, &QPushButton::clicked, this, &FilterPanel::onAddOpenBracket);
  connect(closedBracketButton, &QPushButton::clicked, this, &FilterPanel::onAddClosedBracket);
  connect(controlButton, &QPushButton::clicked, this, &FilterPanel::onControlButtonClicked);
  connect(matchCaseButton, &QPushButton::toggled, this, &FilterPanel::saveRequested);
  connect(wholeWordButton, &QPushButton::toggled, this, &FilterPanel::saveRequested);
  connect(sourceLinesCheckBox, &QCheckBox::toggled, this, &FilterPanel::sourceLinesToggled);
  connect(sourceLinesCheckBox, &QCheckBox::toggled, this, &FilterPanel::saveRequested);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::addOperator(const QChar& filterOperator)
{
  filterTextEdit->addOperator(filterOperator);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int FilterPanel::getExtraLinesAfter() const
{
  return extraLinesAfterLineEdit->text().isEmpty() ? 0 : extraLinesAfterLineEdit->text().toInt();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int FilterPanel::getExtraLinesBefore() const
{
  return extraLinesBeforeLineEdit->text().isEmpty() ? 0 : extraLinesBeforeLineEdit->text().toInt();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString FilterPanel::getFilterText() const
{
  return filterTextEdit->toPlainText();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QVector<int> FilterPanel::getOperatorPositions() const
{
  return filterTextEdit->getOperatorPositions();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchType FilterPanel::getSearchType() const
{
  SearchType searchType = NormalSearch;

  if (wildcardSearchRadioButton->isChecked())
  {
  searchType = WildcardSearch;
  }
  else if (regexpSearchRadioButton->isChecked())
  {
    searchType = RegexpSearch;
  }

  return searchType;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool FilterPanel::isMatchCase() const
{
  return matchCaseButton->isChecked();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool FilterPanel::isSourceLines() const
{
  return sourceLinesCheckBox->isChecked();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool FilterPanel::isWholeWord() const
{
  return wholeWordButton->isChecked();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::toggleStartStop()
{
  controlButton->click();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::updateShortcuts()
{
  controlButtonShortcut = Config().getShortcut(SHORTCUT_START_STOP);
  setControlButtonTooltip();
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::disableControls()
{
  andButton->setEnabled(false);
  orButton->setEnabled(false);
  notButton->setEnabled(false);
  openBracketButton->setEnabled(false);
  closedBracketButton->setEnabled(false);

  matchCaseButton->setEnabled(false);
  wholeWordButton->setEnabled(false);

  filterTextEdit->setEnabled(false);

  searchModeGroupBox->setEnabled(false);
  extraLinesGroupBox->setEnabled(false);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::disableWholeWord(bool checked)
{
  if (checked)
  {
    wholeWordButton->setChecked(false);
    wholeWordButton->setEnabled(false);
  }
  else
  {
    wholeWordButton->setEnabled(true);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::enableControls()
{
  andButton->setEnabled(true);
  orButton->setEnabled(true);
  notButton->setEnabled(true);
  openBracketButton->setEnabled(true);
  closedBracketButton->setEnabled(true);

  matchCaseButton->setEnabled(true);
  wholeWordButton->setEnabled(true);

  filterTextEdit->setEnabled(true);

  searchModeGroupBox->setEnabled(true);
  extraLinesGroupBox->setEnabled(true);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::init(const FilterInfo& filterInfo)
{
  filterTextEdit->setPlainText(filterInfo.expression);
  filterTextEdit->setOperatorPositions(filterInfo.operatorPositions);

  matchCaseButton->setChecked(filterInfo.matchCase);
  wholeWordButton->setChecked(filterInfo.wholeWord);

  sourceLinesCheckBox->setChecked(filterInfo.sourceLines);

  extraLinesBeforeLineEdit->setText(filterInfo.extraLinesBefore == 0 ? "" : QString::number(filterInfo.extraLinesBefore));
  extraLinesAfterLineEdit->setText(filterInfo.extraLinesAfter == 0 ? "" : QString::number(filterInfo.extraLinesAfter));

  switch (filterInfo.searchType)
  {
  case NormalSearch:
    normalSearchRadioButton->setChecked(true);
    break;
  case WildcardSearch:
    wildcardSearchRadioButton->setChecked(true);
    break;
  case RegexpSearch:
    regexpSearchRadioButton->setChecked(true);
    break;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::setControlButtonTooltip()
{
  controlButton->setToolTip(controlButtonTooltip.arg(controlButtonState).arg(controlButtonShortcut));
}



//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::onAddAnd()
{
  filterTextEdit->addOperator('&');
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::onAddClosedBracket()
{
  filterTextEdit->addOperator(')');
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::onAddNot()
{
  filterTextEdit->addOperator('!');
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::onAddOpenBracket()
{
  filterTextEdit->addOperator('(');
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::onAddOr()
{
  filterTextEdit->addOperator('|');
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::onControlButtonClicked()
{
  if (controlButton->isChecked())
  {
    if (filterTextEdit->toPlainText().isEmpty())
    {
      MessageDialog dialog(this, tr("The filter is empty."), MessageDialog::Error);
      dialog.exec();

      controlButton->setChecked(false);
    }
    else
    {
      errorLabel->setText("");
      errorLabel->setVisible(false);

      filterData->applyFilter(filterTextEdit->toPlainText(), filterTextEdit->getOperatorPositions(), getSearchType(), isMatchCase(), isWholeWord(), getExtraLinesBefore(), getExtraLinesAfter());

      emit saveRequested();
    }
  }
  else
  {
    filterData->stop(false);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::onFilterErrorFound(const QString& message, int position)
{
  controlButton->setChecked(false);

  errorLabel->setVisible(true);
  errorLabel->setText(message + QString::number(position));

  filterTextEdit->highlightError(position);
  filterTextEdit->setFocus();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::onFilterStarted(TraceData* traceData)
{
  controlButton->setChecked(true);

  controlButtonState = tr("Stop");
  setControlButtonTooltip();

  disableControls();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FilterPanel::onFilterStopped(TraceData* traceData)
{
  controlButton->setChecked(false);

  controlButtonState = tr("Start");
  setControlButtonTooltip();

  enableControls();

  disableWholeWord(wildcardSearchRadioButton->isChecked() || regexpSearchRadioButton->isChecked());
}