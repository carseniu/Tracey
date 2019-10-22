#pragma once

#include <QDialogButtonBox>
#include <QPixmap>
#include <QRegularExpression>
#include <QString>
#include <QUuid>
#include <QVector>


namespace Tracey
{

  const QString SETTINGS_MINIMIZE_TO_TRAY = "minimizeToTray";
  const QString SETTINGS_CLOSE_TO_TRAY = "closeToTray";
  const QString SETTINGS_DIALOG_DELETE_TRACE = "deleteTrace";
  const QString SETTINGS_DIALOG_DELETE_FILTER = "deleteFilter";
  const QString SETTINGS_DIALOG_DELETE_SOURCE_HISTORY = "deleteSourceHistory";
  const QString SETTINGS_DIALOG_DELETE_SEARCH_HISTORY = "deleteSearchHistory";
  const QString SETTINGS_DIALOG_DELETE_HIGHLIGHTING_GROUP = "deleteHighlightingGroup";
  const QString SETTINGS_DIALOG_DELETE_HIGHLIGHTING_RULE = "deleteHighlightingRule";
  const QString SETTINGS_DIALOG_EXIT_APPLICATION = "exitApplication";

  const int SHORTCUT_ADD_TRACE = 0;
  const int SHORTCUT_ADD_FILTER = 1;
  const int SHORTCUT_CLEAR = 2;
  const int SHORTCUT_SAVE = 3;
  const int SHORTCUT_FIND = 4;
  const int SHORTCUT_FIND_NEXT = 5;
  const int SHORTCUT_FIND_PREVIOUS = 6;
  const int SHORTCUT_TOGGLE_MARK = 7;
  const int SHORTCUT_NEXT_MARK = 8;
  const int SHORTCUT_PREVIOUS_MARK = 9;
  const int SHORTCUT_AUTOSCROLL = 10;
  const int SHORTCUT_DELETE_TRACE = 11;
  const int SHORTCUT_DELETE_FILTER = 12;
  const int SHORTCUT_START_STOP = 13;


  enum SearchType
  {
    NormalSearch,
    RegexpSearch,
    WildcardSearch
  };

  inline QString createId()
  {
    return QUuid::createUuid().toString(QUuid::Id128).left(10);
  }

  QRegularExpression createRegexp(const QString& text, SearchType searchType, bool matchCase, bool wholeWord);
  void resizeDialogButton(QDialog* dialog, QDialogButtonBox::StandardButton standardButton, int width, int height);
  QPixmap setLightness(const QPixmap& pixmap, int factor);
  QPixmap setSaturation(const QPixmap& pixmap, qreal level);
}


struct TraceInfo
{
  QString id = "";
  QString name = "";
  int position = -1;
  QString source = "";
  int rangeType = 0;
  QString openStart = "";
  QString openEnd = "";
  int lineHandling = 0;
  QString lineSize = "";
  bool preloadContents = true;
  QString highlightingGroupId = "";
  bool autoscroll = true;
};

struct FilterInfo
{
  QString id = "";
  QString traceId = "";
  QString name = "";
  int position = -1;
  Tracey::SearchType searchType = Tracey::NormalSearch;
  bool matchCase = false;
  bool wholeWord = false;
  bool sourceLines = false;
  int extraLinesBefore = 0;
  int extraLinesAfter = 0;
  QString expression = "";
  QString highlightingGroupId = "";
  bool autoscroll = true;
  QVector<int> operatorPositions;
};

struct SearchInfo
{
  Tracey::SearchType searchType = Tracey::NormalSearch;
  bool matchCase;
  bool wholeWord;
  QString goTo;
};
