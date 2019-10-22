#pragma once

#include <QFrame>

#include "src/Tracey.h"

class ComboBox;
class HistoryModel;
class IconButton;

class QCheckBox;
class QLineEdit;
class QPushButton;
class QRadioButton;


class SearchWidget : public QFrame
{
  Q_OBJECT

public:
  SearchWidget(QWidget* parent);

private:
  QRegularExpression createRegexp() const;
  SearchInfo getSearchInfo() const;
  Tracey::SearchType getSearchType() const;
  void init();
  void saveSearchItem();
  bool searchItemExists();

  HistoryModel* historyModel;

  IconButton* deleteSearchHistoryButton;
  ComboBox* findComboBox;
  QCheckBox* matchCaseCheckBox;
  QCheckBox* wholeWordCheckBox;
  QRadioButton* normalSearchRadioButton;
  QRadioButton* regexpSearchRadioButton;
  QRadioButton* wildcardSearchRadioButton;
  QPushButton* findNextButton;
  QPushButton* findPreviousButton;
  QPushButton* countButton;

  QLineEdit* goToLineEdit;
  QPushButton* goToButton;

public slots:
  void find();
  void findNext();
  void findPrevious();

private slots:
  void count();
  void deleteSearchHistory();
  void goTo();
  void disableWholeWord(bool checked);

signals:
  void countRequested(const QRegularExpression& regexp);
  void findNextRequested(const QRegularExpression& regexp);
  void findPreviousRequested(const QRegularExpression& regexp);
  void goToRequested(int line, int column);

};