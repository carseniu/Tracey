#pragma once

#include <QString>

class Token
{

public:
  enum TokenType
  {
    Not,
    And,
    Or,
    LeftParenthesis,
    RightParenthesis,
    Operand,
    Empty
  };

  Token();
  Token(TokenType type, QString lexeme, int position, int precedence = 99);

  QString getLexeme() const;
  int getPosition() const;
  int getPrecedence() const;
  TokenType getType() const;

private:
  TokenType type;
  QString lexeme;
  int position;
  int precedence;

};