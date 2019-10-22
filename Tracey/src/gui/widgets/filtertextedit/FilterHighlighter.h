#pragma once

#include <QSyntaxHighlighter>

class FilterTextEdit;


class FilterHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  FilterHighlighter(FilterTextEdit* filterTextEdit);
  ~FilterHighlighter();

  void highlightError(int position);
  void setFont(const QFont& font);

protected:
  void highlightBlock(const QString& text);

private:
  FilterTextEdit* filterTextEdit;

  QFont font;
  int errorPosition;

};