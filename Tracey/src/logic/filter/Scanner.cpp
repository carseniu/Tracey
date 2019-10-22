#include "src/logic/filter/Scanner.h"

#include <QDebug>
#include <QStack>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
Scanner::Scanner(const QString& expression, const QVector<int>& operatorPositions) :
  startIndex(0),
  currentIndex(0)
{
  this->expression = expression;
  this->operatorPositions = operatorPositions;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
Scanner::ScannerResult Scanner::scanTokens()
{
  for (int i : operatorPositions)
  {
    currentIndex = i;

    if (startIndex < currentIndex)
    {
      addOperand();
    }

    addOperator();

    startIndex = currentIndex + 1;
  }

  if (startIndex < expression.size())
  {
    currentIndex = expression.size();

    addOperand();
  }

  ScannerResult result;
  result.tokens = tokens;

  checkTokens(result);

  if (result.error == NoError)
  {
    checkParenthesis(result);
  }

  return result;
}


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Scanner::addOperand()
{
  QString text = expression.mid(startIndex, currentIndex - startIndex);

  tokens << Token(Token::Operand, text, startIndex);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Scanner::addOperator()
{
  QString text = expression.mid(currentIndex, 1);

  switch (text.at(0).toLatin1())
  {
  case '(':
    tokens << Token(Token::LeftParenthesis, text, currentIndex);
    break;

  case ')':
    tokens << Token(Token::RightParenthesis, text, currentIndex);
    break;

  case '!':
    tokens << Token(Token::Not, text, currentIndex, 1);
    break;

  case '&':
    tokens << Token(Token::And, text, currentIndex, 2);
    break;

  case '|':
    tokens << Token(Token::Or, text, currentIndex, 3);
    break;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Scanner::checkParenthesis(ScannerResult& result)
{
  QStack<Token> parenthesisStack;

  for (Token& token : tokens)
  {
    if (token.getType() == Token::LeftParenthesis)
    {
      parenthesisStack.push(token);
    }
    else if (token.getType() == Token::RightParenthesis)
    {
      if (parenthesisStack.isEmpty() || parenthesisStack.top().getType() == Token::RightParenthesis)
      {
        result.error = Scanner::InvalidParenthesis;
        result.errorMessage = tr("Invalid right parenthesis at ");
        result.errorPosition = token.getPosition();
        return;
      }
      else {
        parenthesisStack.pop();
      }
    }
  }

  if (!parenthesisStack.isEmpty())
  {
    result.error = Scanner::InvalidParenthesis;
    result.errorMessage = tr("Missing right parenthesis for left parenthesis at ");
    result.errorPosition = parenthesisStack.pop().getPosition();
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Scanner::checkToken(const Token& previousToken, const Token& currentToken, const Token& nextToken, ScannerResult& result)
{
  if (currentToken.getType() == Token::Not)
  {
    if (nextToken.getType() == Token::Empty)
    {
      result.error = Scanner::UnexpectedEnd;
      result.errorMessage = tr("Missing right operand for NOT at ");
      result.errorPosition = currentToken.getPosition();
    }
    else if (nextToken.getType() != Token::Not && nextToken.getType() != Token::LeftParenthesis && nextToken.getType() != Token::Operand) {
      result.error = Scanner::InvalidToken;
      result.errorMessage = tr("Invalid right operand for NOT at ");
      result.errorPosition = currentToken.getPosition();
    }
  }
  else if (currentToken.getType() == Token::And)
  {
    if (nextToken.getType() == Token::Empty)
    {
      result.error = Scanner::UnexpectedEnd;
      result.errorMessage = tr("Missing right operand for AND at ");
      result.errorPosition = currentToken.getPosition();
    }
    else if (previousToken.getType() == Token::Empty)
    {
      result.error = Scanner::UnexpectedEnd;
      result.errorMessage = tr("Missing left operand for AND at ");
      result.errorPosition = currentToken.getPosition();
    }
    else if (nextToken.getType() != Token::Not && nextToken.getType() != Token::LeftParenthesis && nextToken.getType() != Token::Operand) {
      result.error = Scanner::InvalidToken;
      result.errorMessage = tr("Invalid right operand for AND at ");
      result.errorPosition = currentToken.getPosition();
    }
  }
  else if (currentToken.getType() == Token::Or)
  {
    if (nextToken.getType() == Token::Empty)
    {
      result.error = Scanner::UnexpectedEnd;
      result.errorMessage = tr("Missing right operand for OR at ");
      result.errorPosition = currentToken.getPosition();
    }
    else if (previousToken.getType() == Token::Empty)
    {
      result.error = Scanner::UnexpectedEnd;
      result.errorMessage = tr("Missing left operand for OR at ");
      result.errorPosition = currentToken.getPosition();
    }
    else if (nextToken.getType() != Token::Not && nextToken.getType() != Token::LeftParenthesis && nextToken.getType() != Token::Operand) {
      result.error = Scanner::InvalidToken;
      result.errorMessage = tr("Invalid right operand for OR at ");
      result.errorPosition = currentToken.getPosition();
    }
  }
  else if (currentToken.getType() == Token::LeftParenthesis)
  {
    if (nextToken.getType() == Token::Empty)
    {
      result.error = Scanner::UnexpectedEnd;
      result.errorMessage = tr("Unexpected end of expression at ");
      result.errorPosition = currentToken.getPosition();
    }
    else if (nextToken.getType() != Token::Not && nextToken.getType() != Token::LeftParenthesis && nextToken.getType() != Token::Operand) {
      result.error = Scanner::InvalidToken;
      result.errorMessage = tr("Invalid token at ");
      result.errorPosition = currentToken.getPosition();
    }
  }
  else if (currentToken.getType() == Token::RightParenthesis)
  {
    if (nextToken.getType() != Token::Empty && nextToken.getType() != Token::And && nextToken.getType() != Token::Or && nextToken.getType() != Token::RightParenthesis) {
      result.error = Scanner::InvalidToken;
      result.errorMessage = tr("Invalid token at ");
      result.errorPosition = currentToken.getPosition();
    }
  }
  else if (currentToken.getType() == Token::Operand)
  {
    if (nextToken.getType() != Token::Empty && nextToken.getType() != Token::And && nextToken.getType() != Token::Or && nextToken.getType() != Token::RightParenthesis) {
      result.error = Scanner::InvalidToken;
      result.errorMessage = tr("Invalid token at ");
      result.errorPosition = currentToken.getPosition() + currentToken.getLexeme().size();
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Scanner::checkTokens(ScannerResult& result)
{
  for (int i = 0; i < tokens.size(); ++i)
  {
    Token previousToken;
    if (i >= 1)
    {
      previousToken = tokens[i - 1];
    }

    Token nextToken;
    if (i + 1 < tokens.size())
    {
      nextToken = tokens[i + 1];
    }

    checkToken(previousToken, tokens[i], nextToken, result);

    if (result.error != NoError)
    {
      break;
    }
  }
}