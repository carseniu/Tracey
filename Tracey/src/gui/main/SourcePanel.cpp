#include "src/gui/main/SourcePanel.h"

#include "src/Tracey.h"
#include "src/gui/core/Configuration.h"
#include "src/gui/core/HistoryModel.h"
#include "src/gui/dialogs/MessageDialog.h"
#include "src/gui/main/IntValidator.h"
#include "src/gui/widgets/combobox/ComboBox.h"
#include "src/gui/widgets/combobox/ComboBoxDelegate.h"
#include "src/gui/widgets/iconbutton/IconButton.h"


#include <QAbstractItemView>
#include <QCheckBox>
#include <QCompleter>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QVBoxLayout>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
SourcePanel::SourcePanel(SourceData* sourceData, const TraceInfo& traceInfo, HighlightingData* highlightingData, HistoryModel* historyModel, SourceWidget* sourceWidget) :
  TracePanel(highlightingData, sourceWidget),
  sourceData(sourceData),
  sourceWidget(sourceWidget),
  currentSource("")
{
  setFixedHeight(100);

  setHighlightingGroupId(traceInfo.highlightingGroupId);

  // FILE INPUT
  deleteSourceHistoryButton = new IconButton(QIcon(":/Application/img/DeleteHistory.png"), this);
  deleteSourceHistoryButton->setFocusPolicy(Qt::NoFocus);
  deleteSourceHistoryButton->setToolTip(tr("Delete Source History"));

  sourceComboBox = new ComboBox(this);
  sourceComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  sourceComboBox->setEditable(true);
  sourceComboBox->setDuplicatesEnabled(false);
  sourceComboBox->setInsertPolicy(QComboBox::NoInsert);
  sourceComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  sourceComboBox->setModel(historyModel);

  QCompleter* completer = new QCompleter(historyModel, sourceComboBox);
  completer->popup()->setItemDelegate(new ComboBoxDelegate);
  completer->setCompletionMode(QCompleter::PopupCompletion);
  sourceComboBox->setCompleter(completer);

  browseButton = new IconButton(QIcon(":/Application/img/Browse.png"), this);
  browseButton->setFocusPolicy(Qt::NoFocus);
  browseButton->setToolTip(tr("Browse"));

  QIcon icon;
  icon.addFile(":/Application/img/Start.png", QSize(), QIcon::Normal, QIcon::Off);
  icon.addFile(":/Application/img/Stop.png", QSize(), QIcon::Normal, QIcon::On);
  controlButton = new IconButton(icon, this);
  controlButton->setCheckable(true);
  controlButton->setFocusPolicy(Qt::NoFocus);
  controlButtonState = tr("Start");
  setControlButtonTooltip();


  // FILE CONTENTS
  fileContentGroupBox = new QGroupBox(tr("File Content"));
  fileContentGroupBox->setFixedSize(470, 55);

  preloadCheckBox = new QCheckBox(tr("Preload Content"), this);
  preloadCheckBox->setFocusPolicy(Qt::NoFocus);

  rangeTypeComboBox = new ComboBox(this);
  rangeTypeComboBox->setFixedSize(110, 20);
  rangeTypeComboBox->addItem(tr("File Position"));
  rangeTypeComboBox->addItem(tr("Tail"));
  rangeTypeComboBox->addItem(tr("Line Number"));

  openStartLabel = new QLabel(tr("Start"), this);
  openStartLineEdit = new QLineEdit(this);
  openStartLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
  openStartLineEdit->setFocusPolicy(Qt::ClickFocus);
  openStartLineEdit->setFixedSize(70, 20);
  openStartLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9][0-9]*[kKmM]{0,1}$"), this));

  openEndLabel = new QLabel(tr("End"), this);
  openEndLineEdit = new QLineEdit(this);
  openEndLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
  openEndLineEdit->setFocusPolicy(Qt::ClickFocus);
  openEndLineEdit->setFixedSize(70, 20);
  openEndLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9][0-9]*[kKmM]{0,1}$"), this));

  QHBoxLayout* fileContentsLayout = new QHBoxLayout;
  fileContentsLayout->addWidget(preloadCheckBox);
  fileContentsLayout->addSpacing(20);
  fileContentsLayout->addWidget(rangeTypeComboBox);
  fileContentsLayout->addWidget(openStartLabel, 0, Qt::AlignRight);
  fileContentsLayout->addWidget(openStartLineEdit);
  fileContentsLayout->addWidget(openEndLabel, 0, Qt::AlignRight);
  fileContentsLayout->addWidget(openEndLineEdit);
  fileContentGroupBox->setLayout(fileContentsLayout);


  // LINE SIZE
  lineSizeGroupBox = new QGroupBox(tr("Line Size"));
  lineSizeGroupBox->setFixedSize(160, 55);

  lineHandlingComboBox = new ComboBox(this);
  lineHandlingComboBox->setFixedSize(70, 20);
  lineHandlingComboBox->addItem(tr("Cut"));
  lineHandlingComboBox->addItem(tr("Wrap"));

  lineSizeLineEdit = new QLineEdit(this);
  lineSizeLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
  lineSizeLineEdit->setFocusPolicy(Qt::ClickFocus);
  lineSizeLineEdit->setFixedSize(50, 20);
  lineSizeLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[1-9][0-9]{0,2}$")));

  QHBoxLayout* lineSizeLayout = new QHBoxLayout;
  lineSizeLayout->addWidget(lineHandlingComboBox);
  lineSizeLayout->addWidget(lineSizeLineEdit);
  lineSizeGroupBox->setLayout(lineSizeLayout);


  // MAIN LAYOUT
  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  topLayout->setSpacing(5);
  topLayout->addWidget(deleteSourceHistoryButton);
  topLayout->addWidget(sourceComboBox);
  topLayout->addWidget(browseButton);
  topLayout->addWidget(controlButton);
  topLayout->addStretch();
  topLayout->addWidget(getHighlightingWidget());

  QHBoxLayout* bottomLayout = new QHBoxLayout;
  bottomLayout->setMargin(0);
  bottomLayout->setSpacing(15);
  bottomLayout->addWidget(fileContentGroupBox);
  bottomLayout->addWidget(lineSizeGroupBox);
  bottomLayout->addStretch();

  
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 3);
  mainLayout->setSpacing(5);
  mainLayout->addLayout(topLayout);
  mainLayout->addLayout(bottomLayout);

  init(traceInfo);

  // CONNECTIONS
  connect(sourceData, &SourceData::contentProcessFinished, this, &SourcePanel::onContentProcessFinished);
  connect(sourceData, &SourceData::endOfFileSmallerThanPosition, this, &SourcePanel::onEndOfFileSmallerThanPosition, Qt::QueuedConnection);
  connect(sourceData, &SourceData::endOfFileSmallerThanPositionStart, this, &SourcePanel::onEndOfFileSmallerThanPositionStart, Qt::QueuedConnection);
  connect(sourceData, &SourceData::errorReadingFile, this, &SourcePanel::onErrorReadingFile, Qt::QueuedConnection);
  connect(sourceData, &SourceData::notEnoughLines, this, &SourcePanel::onNotEnoughLines);
  connect(deleteSourceHistoryButton, &QPushButton::clicked, this, &SourcePanel::deleteSourceHistoryRequested);
  connect(browseButton, &QPushButton::clicked, this, &SourcePanel::processBrowse);
  connect(controlButton, &QPushButton::clicked, this, &SourcePanel::onControlButtonClicked);
  connect(historyModel, &HistoryModel::modelAboutToBeReset, this, &SourcePanel::saveCurrentSource);
  connect(historyModel, &HistoryModel::modelReset, this, &SourcePanel::restoreCurrentSource);
  connect(sourceComboBox->lineEdit(), &QLineEdit::textChanged, this, &SourcePanel::saveRequested);
  connect(preloadCheckBox, &QCheckBox::toggled, this, &SourcePanel::saveRequested);
  connect(preloadCheckBox, &QCheckBox::toggled, this, &SourcePanel::onPreloadChanged);
  connect(rangeTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SourcePanel::onRangeTypeChanged);
  connect(openStartLineEdit, &QLineEdit::textEdited, this, &SourcePanel::onOpenStartChanged);
  connect(openEndLineEdit, &QLineEdit::textEdited, this, &SourcePanel::onOpenEndChanged);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::addSource(const QString& file)
{
  sourceComboBox->setCurrentText(file);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int SourcePanel::getLineHandling() const
{
  return lineHandlingComboBox->currentIndex();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString SourcePanel::getLineSize() const
{
  return lineSizeLineEdit->text();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString SourcePanel::getOpenEnd() const
{
  return openEndLineEdit->text();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString SourcePanel::getOpenStart() const
{
  return openStartLineEdit->text();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
int SourcePanel::getRangeType() const
{
  return rangeTypeComboBox->currentIndex();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString SourcePanel::getSource() const
{
  return sourceComboBox->currentText();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SourcePanel::isPreloadFileContents() const
{
  return preloadCheckBox->isChecked();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::setSource(const QString& source)
{
  sourceComboBox->setCurrentText(source);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::toggleStartStop()
{
  controlButton->click();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::updateShortcuts()
{
  controlButtonShortcut = Config().getShortcut(SHORTCUT_START_STOP);
  setControlButtonTooltip();
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString SourcePanel::getAbbreviationCase(const QString& text)
{
  QString result = text.toUpper();

  for (int i = 0; i < text.size(); ++i)
  {
    if (result.at(i) == 'K')
    {
      result[i] = 'k';
    }
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
qint64 SourcePanel::getValueFromAbbreviation(const QString& text)
{
  qint64 result = 0;

  if (text.size() == 0)
  {
    result = 0;
  }
  else
  {
    switch (text.back().toLatin1())
    {
    case 'k':
      result = static_cast<qint64>(text.left(text.size() - 1).toLongLong() * 1000);
      break;
    case 'M':
      result = static_cast<qint64>(text.left(text.size() - 1).toLongLong() * 1000000);
      break;
    case 'G':
      result = static_cast<qint64>(text.left(text.size() - 1).toLongLong() * 1000000000);
      break;
    default:
      result = static_cast<qint64>(text.toLongLong());
      break;
    }
  }

  return result;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::init(const TraceInfo& traceInfo)
{
  sourceComboBox->setCurrentText(traceInfo.source);
  preloadCheckBox->setChecked(traceInfo.preloadContents);
  rangeTypeComboBox->setCurrentIndex(traceInfo.rangeType);
  openStartLineEdit->setText(traceInfo.openStart);
  openEndLineEdit->setText(traceInfo.openEnd);
  lineHandlingComboBox->setCurrentIndex(traceInfo.lineHandling);
  lineSizeLineEdit->setText(traceInfo.lineSize);

  onPreloadChanged(preloadCheckBox->isChecked());

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::setAbbreviationCase(QLineEdit* lineEdit)
{
  int cursorPosition = lineEdit->cursorPosition();
  lineEdit->setText(getAbbreviationCase(lineEdit->text()));
  lineEdit->setCursorPosition(cursorPosition);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::stopAndShowError(const QString& message)
{
  if (controlButton->isChecked())
  {
    controlButton->setChecked(false);
    fileContentGroupBox->setEnabled(true);
    lineSizeGroupBox->setEnabled(true);
  }

  MessageDialog dialog(this, message, MessageDialog::Error);
  dialog.exec();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::setControlButtonTooltip()
{
  controlButton->setToolTip(controlButtonTooltip.arg(controlButtonState).arg(controlButtonShortcut));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::showOpenEndError(const QString& message)
{
  MessageDialog dialog(this, message, MessageDialog::Error);
  dialog.exec();

  openEndLineEdit->setFocus();
  controlButton->setChecked(false);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::showOpenStartError(const QString& message)
{
  MessageDialog dialog(this, message, MessageDialog::Error);
  dialog.exec();

  openStartLineEdit->setFocus();
  controlButton->setChecked(false);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool SourcePanel::validateLineSize()
{
  if (lineSizeLineEdit->text().isEmpty())
  {
    return true;
  }

  bool ok = false;
  int lineSize = lineSizeLineEdit->text().toInt(&ok);

  if (ok)
  {
    if (lineSize < sourceData->getMinLineSize() || lineSize > sourceData->getMaxLineSize())
    {
      ok = false;
    }
  }

  return ok;
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::onControlButtonClicked()
{ 
  if (controlButton->isChecked())
  {
    QString source = sourceComboBox->currentText();
    QFile sourceFile(source);

    if (!sourceFile.exists())
    {
      MessageDialog dialog(this, tr("The file does not exist."), MessageDialog::Error);
      dialog.exec();

      controlButton->setChecked(false);

      return;
    }

    if (!validateLineSize())
    {
      MessageDialog dialog(this, tr("Line size must be between %1 and %2.").arg(sourceData->getMinLineSize()).arg(sourceData->getMaxLineSize()), MessageDialog::Error);
      dialog.exec();

      lineSizeLineEdit->setFocus();
      controlButton->setChecked(false);

      return;
    }

    SourceData::RangeType rangeType = static_cast<SourceData::RangeType>(rangeTypeComboBox->currentIndex());
    qint64 openStart = openStartLineEdit->text().isEmpty() ? 0 : getValueFromAbbreviation(openStartLineEdit->text());
    qint64 openEnd = openEndLineEdit->text().isEmpty() ? -1 : getValueFromAbbreviation(openEndLineEdit->text());

    if (rangeType == SourceData::Tail) {
      openStart = sourceFile.size() - qAbs(openStart);

      if (openStart < 0) {
        showOpenStartError(tr("Start value is negative."));
        return;
      }
    }

    if (rangeType == SourceData::Tail && openEnd != -1) {
      int muie = sourceFile.size();

      openEnd = sourceFile.size() - qAbs(openEnd);

      if (openEnd < 0) {
        showOpenEndError(tr("End value is negative."));
        return;
      }
    }

    if (openEnd != -1 && openEnd < openStart)
    {
      showOpenEndError(tr("End value must be bigger than or equal to the start value."));
      return;
    }

    if (rangeType == SourceData::FilePosition)
    {
      if (openStart > sourceFile.size())
      {
        showOpenStartError(tr("Start value is bigger than the file size."));
        return;
      }
    }

    SourceData::LineHandling lineHandling = static_cast<SourceData::LineHandling>(lineHandlingComboBox->currentIndex());

    int lineSize = 0;
    if (lineHandling == SourceData::Cut)
    {
      lineSize = lineSizeLineEdit->text().isEmpty() ? sourceData->getMaxLineSize() : lineSizeLineEdit->text().toInt();
    }
    else if (lineHandling == SourceData::Wrap)
    {
      lineSize = lineSizeLineEdit->text().isEmpty() ? sourceWidget->getTraceViewerLineSize() : lineSizeLineEdit->text().toInt();
    }


    sourceData->open(source, preloadCheckBox->isChecked(), rangeType, openStart, openEnd, lineHandling, lineSize);

    // Set the source in the combo box
    int index = sourceComboBox->findText(source);

    if (index != -1)
    {
      sourceComboBox->removeItem(index);
    }

    sourceComboBox->insertItem(0, source);
    sourceComboBox->setCurrentIndex(0);

    controlButtonState = tr("Stop");
    setControlButtonTooltip();

    fileContentGroupBox->setEnabled(false);
    lineSizeGroupBox->setEnabled(false);

    emit saveRequested();
  }
  else
  {
    controlButtonState = tr("Start");
    setControlButtonTooltip();

    fileContentGroupBox->setEnabled(true);
    lineSizeGroupBox->setEnabled(true);

    sourceData->stop(false);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::onContentProcessFinished()
{
  if (!openEndLineEdit->text().isEmpty())
  {
    controlButton->setChecked(false);

    controlButtonState = tr("Start");
    setControlButtonTooltip();

    fileContentGroupBox->setEnabled(true);
    lineSizeGroupBox->setEnabled(true);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::onEndOfFileSmallerThanPosition()
{
  stopAndShowError(tr("The file contents have changed for %1.\nPlease reload.").arg(sourceData->getName()));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::onEndOfFileSmallerThanPositionStart()
{
  stopAndShowError(tr("The start position is less than file size.").arg(sourceData->getName()));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::onErrorReadingFile()
{
  stopAndShowError(tr("Cannot read the file contents for %1.\nPlease check the file.").arg(sourceData->getName()));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::onOpenEndChanged(const QString& text)
{
  setAbbreviationCase(openEndLineEdit);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::onOpenStartChanged(const QString& text)
{
  setAbbreviationCase(openStartLineEdit);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::onNotEnoughLines(int startLine, int lineCount)
{
  controlButton->setChecked(false);
  controlButtonState = tr("Start");
  setControlButtonTooltip();
  fileContentGroupBox->setEnabled(true);
  lineSizeGroupBox->setEnabled(true);

  MessageDialog dialog(this, tr("Start line (%1) bigger than the number of lines in the file (%2).").arg(startLine + 1).arg(lineCount + 1), MessageDialog::Error);
  dialog.exec();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::onPreloadChanged(bool checked)
{
  if (checked)
  {
    rangeTypeComboBox->setEnabled(true);
    openStartLabel->setEnabled(true);
    openStartLineEdit->setEnabled(true);
    openEndLabel->setEnabled(true);
    openEndLineEdit->setEnabled(true);
  }
  else
  {
    rangeTypeComboBox->setEnabled(false);
    openStartLabel->setEnabled(false);
    openStartLineEdit->setEnabled(false);
    openEndLabel->setEnabled(false);
    openEndLineEdit->setEnabled(false);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::onRangeTypeChanged(int index)
{
  SourceData::RangeType rangeType = static_cast<SourceData::RangeType>(index);

  if (rangeType == SourceData::FilePosition || rangeType == SourceData::Tail)
  {
    openStartLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9][0-9]*[kKmM]{0,1}$"), this));
    openEndLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9][0-9]*[kKmM]{0,1}$"), this));
  }
  else {
    openStartLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[1-9][0-9]*[kKmM]{0,1}$"), this));
    openEndLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[1-9][0-9]*[kKmM]{0,1}$"), this));
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::processBrowse()
{
  QDir dir(sourceComboBox->currentText());

  if (dir.canonicalPath().isEmpty())
  {
    dir.setPath(QDir::rootPath());
  }

  QString source = QFileDialog::getOpenFileName(this, tr("Source"), dir.canonicalPath());

  if (!source.isEmpty())
  {
    sourceComboBox->setCurrentText(source);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::restoreCurrentSource()
{
  sourceComboBox->setCurrentText(currentSource);

  currentSource = "";
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void SourcePanel::saveCurrentSource()
{
  currentSource = sourceComboBox->currentText();
}