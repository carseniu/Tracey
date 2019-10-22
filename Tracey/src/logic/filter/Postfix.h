#pragma once

#include <QObject>

#include "src/Tracey.h"

#include <QQueue>

class Filter;
class FilterNode;
class Token;


class Postfix : public QObject
{
  Q_OBJECT

public:
  Postfix(const QList<Token>& tokens);

  QString getExpression();
  FilterNode* getFilterTree(Tracey::SearchType searchType, bool matchCase, bool wholeWord, Filter* filter);

private:
  void create();
  bool isOperand(const Token& token);
  bool isOperator(const Token& token);

  QList<Token> tokens;
  QQueue<Token> postfix;

};