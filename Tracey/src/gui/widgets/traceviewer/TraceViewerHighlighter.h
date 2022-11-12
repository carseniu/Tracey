#pragma once

#include <QSyntaxHighlighter>

class HighlightingRule;


class TraceViewerHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  TraceViewerHighlighter(QTextDocument* document);

  void setRules(const QVector<HighlightingRule*>& rules);

protected:
  virtual void highlightBlock(const QString& text);

private:
  void highlightRule(const QString& text, HighlightingRule* rule);

  QVector<HighlightingRule*> rules;

};