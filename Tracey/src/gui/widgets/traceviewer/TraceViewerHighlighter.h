#pragma once

#include "src/gui/core/HighlightingRule.h"

#include <QSyntaxHighlighter>


class TraceViewerHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  TraceViewerHighlighter(QTextDocument* document);

  void setRules(const QVector<HighlightingRule*>& rules);

protected:
  virtual void highlightBlock(const QString& text);

private:
  void highlightDoubleClickSelection(const QString& text);
  void highlightRule(const QString& text, HighlightingRule* rule);

  QVector<HighlightingRule*> rules;

};