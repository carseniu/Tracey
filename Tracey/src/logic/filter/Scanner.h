#pragma once

#include <QObject>

#include "src/logic/filter/Token.h"

#include <QVector>


class Scanner : public QObject
{
  Q_OBJECT

public:
  enum ScannerError
  {
    NoError,
    InvalidToken,
    UnexpectedEnd,
    InvalidParenthesis
  };

  struct ScannerResult
  {
    QList<Token> tokens;
    Scanner::ScannerError error = Scanner::NoError;
    QString errorMessage = "";
    int errorPosition;
  };

  Scanner(const QString& expression, const QVector<int>& operatorPositions);

  ScannerResult scanTokens();

private:
  void addOperand();
  void addOperator();
  void checkParenthesis(ScannerResult& result);
  void checkToken(const Token& previousToken, const Token& currentToken, const Token& nextToken, ScannerResult& result);
  void checkTokens(ScannerResult& result);

  QString expression;
  QVector<int> operatorPositions;
  QList<Token> tokens;
  int startIndex;
  int currentIndex;

};