#pragma once

#include "src/gui/main/TracePanel.h"

#include "src/gui/main/SourceWidget.h"
#include "src/logic/source/SourceData.h"


class ComboBox;
class HistoryModel;
class IconButton;
class SourceData;
class SourceWidget;

class QCheckBox;
class QGroupBox;
class QRadioButton;
class QLabel;
class QLineEdit;


class SourcePanel : public TracePanel
{
  Q_OBJECT

public:
  SourcePanel(SourceData* sourceData, const TraceInfo& traceInfo, HighlightingData* highlightingData, HistoryModel* historyModel, SourceWidget* parent);

  void addSource(const QString& file);
  int getLineHandling() const;
  QString getLineSize() const;
  QString getOpenEnd() const;
  QString getOpenStart() const;
  int getRangeType() const;
  QString getSource() const;
  bool isPreloadFileContents() const;
  void setSource(const QString& source);
  void toggleStartStop();
  void updateShortcuts() override;

private:
  QString getAbbreviationCase(const QString& text);
  qint64 getValueFromAbbreviation(const QString& text);
  void init(const TraceInfo& traceInfo);
  void setAbbreviationCase(QLineEdit* lineEdit);
  void stopAndShowError(const QString& message);
  void setControlButtonTooltip();
  void showOpenEndError(const QString& message);
  void showOpenStartError(const QString& message);
  bool validateLineSize();

  SourceData* sourceData;
  SourceWidget* sourceWidget;

  IconButton* deleteSourceHistoryButton;
  ComboBox* sourceComboBox;
  IconButton* browseButton;
  IconButton* controlButton;

  QGroupBox* fileContentGroupBox;
  QCheckBox* preloadCheckBox;
  ComboBox* rangeTypeComboBox;
  QLabel* openStartLabel;
  QLineEdit* openStartLineEdit;
  QLabel* openEndLabel;
  QLineEdit* openEndLineEdit;

  QGroupBox* lineSizeGroupBox;
  ComboBox* lineHandlingComboBox;
  QLineEdit* lineSizeLineEdit;

  QString currentSource;

  QString controlButtonTooltip = "%1 (%2)";
  QString controlButtonState = tr("Start");
  QString controlButtonShortcut = "Control+Return";

private slots:
  void onControlButtonClicked();
  void onContentProcessFinished();
  void onEndOfFileSmallerThanPosition();
  void onEndOfFileSmallerThanPositionStart();
  void onErrorReadingFile();
  void onOpenEndChanged(const QString& text);
  void onOpenStartChanged(const QString& text);
  void onNotEnoughLines(int startLine, int lineCount);
  void onPreloadChanged(bool checked);
  void onRangeTypeChanged(int index);
  void processBrowse();
  void restoreCurrentSource();
  void saveCurrentSource();

signals:
  void deleteSourceHistoryRequested();

};