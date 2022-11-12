#include "src/gui//tools/SearchWidget.h"

#include "src/Tracey.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/core/HistoryModel.h"
#include "src/gui/dialogs/MessageDialog.h"
#include "src/gui/widgets/combobox/ComboBox.h"
#include "src/gui/widgets/combobox/ComboBoxDelegate.h"
#include "src/gui/widgets/iconbutton/IconButton.h"

#include <QAbstractItemView>
#include <QDebug>
#include <QLabel>
#include <QCheckBox>
#include <QCompleter>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
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
SearchWidget::SearchWidget(QWidget* parent) :
  QFrame(parent)
{
  setFixedHeight(190);

  // FIND LAYOUT
  historyModel = new HistoryModel(HistoryModel::Search, this);

  deleteSearchHistoryButton = new IconButton(QIcon(":/Application/img/DeleteHistory.png"), this);
  deleteSearchHistoryButton->setFocusPolicy(Qt::NoFocus);
  deleteSearchHistoryButton->setToolTip(tr("Delete Search History"));
  connect(deleteSearchHistoryButton, &IconButton::clicked, this, &SearchWidget::deleteSearchHistory);

  findComboBox = new ComboBox(this);
  findComboBox->setMinimumSize(160, 20);
  findComboBox->setEditable(true);
  findComboBox->setDuplicatesEnabled(false);
  findComboBox->setInsertPolicy(QComboBox::NoInsert);
  findComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  findComboBox->setModel(historyModel);
  findComboBox->setFocusPolicy(Qt::ClickFocus);
  connect(findComboBox->lineEdit(), &QLineEdit::returnPressed, this, &SearchWidget::findNext);

  QCompleter* completer = new QCompleter(historyModel, findComboBox);
  completer->popup()->setItemDelegate(new ComboBoxDelegate);
  completer->setCompletionMode(QCompleter::PopupCompletion);
  findComboBox->setCompleter(completer);


  QHBoxLayout* findLayout = new QHBoxLayout;
  findLayout->setContentsMargins(0, 0, 0, 0);
  findLayout->setSpacing(5);
  findLayout->addWidget(deleteSearchHistoryButton);
  findLayout->addWidget(findComboBox);


  // SEARCH OPTIONS
  matchCaseCheckBox = new QCheckBox(tr("Match Case"), this);
  matchCaseCheckBox->setFixedWidth(90);
  matchCaseCheckBox->setFocusPolicy(Qt::NoFocus);

  wholeWordCheckBox = new QCheckBox(tr("Whole Word"), this);
  wholeWordCheckBox->setFixedWidth(90);
  wholeWordCheckBox->setFocusPolicy(Qt::NoFocus);


  // SEARCH MODE
  QGroupBox* searchModeGroupBox = new QGroupBox(tr("Search Mode"));
  searchModeGroupBox->setFixedWidth(150);

  normalSearchRadioButton = new QRadioButton(tr("Normal"));
  normalSearchRadioButton->setChecked(true);
  normalSearchRadioButton->setFocusPolicy(Qt::NoFocus);

  wildcardSearchRadioButton = new QRadioButton(tr("Wildcard"));
  wildcardSearchRadioButton->setFocusPolicy(Qt::NoFocus);
  connect(wildcardSearchRadioButton, &QRadioButton::toggled, this, &SearchWidget::disableWholeWord);

  regexpSearchRadioButton = new QRadioButton(tr("Regular Expression"));
  regexpSearchRadioButton->setFocusPolicy(Qt::NoFocus);
  connect(regexpSearchRadioButton, &QRadioButton::toggled, this, &SearchWidget::disableWholeWord);

  QVBoxLayout* searchModeLayout = new QVBoxLayout;
  searchModeLayout->addWidget(normalSearchRadioButton);
  searchModeLayout->addWidget(wildcardSearchRadioButton);
  searchModeLayout->addWidget(regexpSearchRadioButton);
  searchModeGroupBox->setLayout(searchModeLayout);


  // SEARCH BUTTONS
  findNextButton = new QPushButton(tr("Find Next"), this);
  findNextButton->setFixedSize(80, 20);
  findNextButton->setFocusPolicy(Qt::NoFocus);
  connect(findNextButton, &QPushButton::clicked, this, &SearchWidget::findNext);

  findPreviousButton = new QPushButton(tr("Find Previous"), this);
  findPreviousButton->setFixedSize(80, 20);
  findPreviousButton->setFocusPolicy(Qt::NoFocus);
  connect(findPreviousButton, &QPushButton::clicked, this, &SearchWidget::findPrevious);

  countButton = new QPushButton(tr("Count"), this);
  countButton->setFixedSize(80, 20);
  countButton->setFocusPolicy(Qt::NoFocus);
  connect(countButton, &QPushButton::clicked, this, &SearchWidget::count);

  QVBoxLayout* searchButtonsLayout = new QVBoxLayout;
  searchButtonsLayout->setContentsMargins(0, 0, 0, 0);
  searchButtonsLayout->setSpacing(5);
  searchButtonsLayout->addWidget(findNextButton);
  searchButtonsLayout->addWidget(findPreviousButton);
  searchButtonsLayout->addStretch();
  searchButtonsLayout->addWidget(countButton);


  // GO TO
  QFrame* separator = new QFrame(this);
  separator->setObjectName("Separator");
  separator->setFixedHeight(1);

  QLabel* goToLabel = new QLabel(tr("Line[,Column]"), this);
  goToLabel->setFixedWidth(80);

  goToLineEdit = new QLineEdit(this);
  goToLineEdit->setMaxLength(20);
  goToLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\d*\\,?\\d+$"), goToLineEdit));
  goToLineEdit->setFocusPolicy(Qt::ClickFocus);
  goToLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
  connect(goToLineEdit, &QLineEdit::returnPressed, this, &SearchWidget::goTo);

  goToButton = new QPushButton(tr("Go to..."), this);
  goToButton->setFixedSize(80, 20);
  goToButton->setFocusPolicy(Qt::NoFocus);
  connect(goToButton, &QPushButton::clicked, this, &SearchWidget::goTo);

  QHBoxLayout* goToLayout = new QHBoxLayout;
  goToLayout->setContentsMargins(0, 0, 0, 0);
  goToLayout->setSpacing(20);
  goToLayout->addWidget(goToLabel);
  goToLayout->addWidget(goToLineEdit);
  goToLayout->addWidget(goToButton);


  // MAIN LAYOUT
  QGridLayout* layout = new QGridLayout(this);
  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(10);
  layout->addLayout(findLayout, 0, 0, 1, 3);
  layout->addWidget(matchCaseCheckBox, 1, 0, 1, 1, Qt::AlignBottom);
  layout->addWidget(wholeWordCheckBox, 2, 0, 1, 1, Qt::AlignTop);
  layout->addWidget(searchModeGroupBox, 1, 1, 2, 1, Qt::AlignLeft);
  layout->addLayout(searchButtonsLayout, 1, 2, 2, 1);
  layout->addWidget(separator, 3, 0, 1, 3);
  layout->addLayout(goToLayout, 4, 0, 1, 3);

  init();
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QRegularExpression SearchWidget::createRegexp() const
{
  return Tracey::createRegexp(findComboBox->currentText(), getSearchType(), matchCaseCheckBox->isChecked(), wholeWordCheckBox->isChecked());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchInfo SearchWidget::getSearchInfo() const
{
  SearchInfo searchInfo;

  searchInfo.searchType = getSearchType();
  searchInfo.matchCase = matchCaseCheckBox->isChecked();
  searchInfo.wholeWord = wholeWordCheckBox->isChecked();
  searchInfo.goTo = goToLineEdit->text();

  return searchInfo;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SearchType SearchWidget::getSearchType() const
{
  SearchType searchType = NormalSearch;

  if (regexpSearchRadioButton->isChecked())
  {
    searchType = RegexpSearch;
  }
  else if (wildcardSearchRadioButton->isChecked())
  {
    searchType = WildcardSearch;
  }

  return searchType;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWidget::init()
{
  SearchInfo searchInfo = Config().getSearchInfo();

  switch (searchInfo.searchType)
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

  matchCaseCheckBox->setChecked(searchInfo.matchCase);
  wholeWordCheckBox->setChecked(searchInfo.wholeWord);

  goToLineEdit->setText(searchInfo.goTo);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWidget::saveSearchItem()
{
  QString source = findComboBox->currentText();

  int index = findComboBox->findText(source);

  if (index == 0)
  {
    return;
  }

  if (index != -1)
  {
    findComboBox->removeItem(index);
  }

  findComboBox->insertItem(0, source);
  findComboBox->setCurrentIndex(0);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SearchWidget::searchItemExists()
{
  if (findComboBox->currentText().isEmpty())
  {
    MessageDialog dialog(this, tr("Please enter a search item."), MessageDialog::Error);
    dialog.exec();

    return false;
  }

  return true;
}


//===================================================================================================================================================================================================//
// PUBLIC SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWidget::find()
{
  findComboBox->lineEdit()->selectAll();
  findComboBox->setFocus();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWidget::findNext()
{
  if (searchItemExists())
  {
    saveSearchItem();

    Config().saveSearchInfo(getSearchInfo());

    emit findNextRequested(createRegexp());
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWidget::findPrevious()
{
  if (searchItemExists())
  {
    saveSearchItem();

    Config().saveSearchInfo(getSearchInfo());

    emit findPreviousRequested(createRegexp());
  }
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWidget::count()
{
  if (searchItemExists())
  {
    saveSearchItem();

    Config().saveSearchInfo(getSearchInfo());

    emit countRequested(createRegexp());
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWidget::deleteSearchHistory()
{
  bool showDialog = Config().getBoolProperty(SETTINGS_DIALOG_DELETE_SEARCH_HISTORY);
  bool ok = true;

  if (showDialog)
  {
    MessageDialog dialog(this, tr("Delete search history?"), MessageDialog::Warning, true);
    ok = dialog.exec();

    Config().setProperty(SETTINGS_DIALOG_DELETE_SEARCH_HISTORY, QString::number(!dialog.getDontAskAgain()));
  }

  if (ok)
  {
    historyModel->setStringList(QStringList());
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWidget::goTo()
{
  int line = 0;
  int column = 0;
  QStringList lineColumnStr = goToLineEdit->text().split(",");

  if (!lineColumnStr.at(0).isEmpty())
  {
    line = lineColumnStr.at(0).toInt() - 1;
  }

  if (lineColumnStr.size() > 1)
  {
    column = lineColumnStr.at(1).toInt();
  }

  emit goToRequested(line, column);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SearchWidget::disableWholeWord(bool checked)
{
  if (checked)
  {
    wholeWordCheckBox->setChecked(false);
    wholeWordCheckBox->setEnabled(false);
  }
  else
  {
    wholeWordCheckBox->setEnabled(true);
  }
}
