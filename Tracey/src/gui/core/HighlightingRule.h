#pragma once

#include <QRegularExpression>
#include <QString>
#include <QTextCharFormat>


class HighlightingRule
{

public:
  HighlightingRule(const QString& id, const QString& item, const QString& foregroundColor = QString("#000000"), const QString& backgroundColor = QString("#ffffff"),
    bool matchCase = false, bool wholeWord = false, bool enabled = true, int sort = 0);

  QString getBackgroundColor() const;
  QString getId() const;
  QString getItem() const;
  QString getForegroundColor() const;
  QTextCharFormat getFormat() const;
  QRegularExpression getRegularExpression() const;
  int getSort() const;
  bool isEnabled() const;
  bool isMatchCase() const;
  bool isWholeWord() const;
  void setBackgroundColor(const QString& color);
  void setEnabled(bool enabled);
  void setForegroundColor(const QString& color);
  void setFormat(const QTextCharFormat& format);
  void setItem(const QString& item);
  void setMatchCase(bool matchCase);
  void setSort(int sort);
  void setWholeWord(bool wholeWord);

private:
  QString id;
  QString item;
  QTextCharFormat format;
  QString foregroundColor;
  QString backgroundColor;
  bool wholeLine;
  bool matchCase;
  bool wholeWord;
  bool enabled;
  int sort;

};