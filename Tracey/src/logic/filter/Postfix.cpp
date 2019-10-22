#include "src/logic/filter/Postfix.h"

#include "src/Tracey.h"
#include "src/logic/filter/FilterNode.h"
#include "src/logic/filter/FilterNodeAnd.h"
#include "src/logic/filter/FilterNodeNot.h"
#include "src/logic/filter/FilterNodeOperand.h"
#include "src/logic/filter/FilterNodeOr.h"
#include "src/logic/filter/Token.h"

#include <QDebug>
#include <QRegularExpression>
#include <QStack>

using namespace Tracey;


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
Postfix::Postfix(const QList<Token>& tokens) :
  tokens(tokens)
{
  create();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QString Postfix::getExpression()
{
  QStack<QString> resultStack;

  for (Token& token : postfix)
  {
    if (isOperand(token))
    {
      resultStack.push(token.getLexeme());
    }
    else
    {
      if (token.getType() == Token::Not)
      {
        QString operand = resultStack.pop();
        QString unaryExpression = "(" + token.getLexeme() + operand + ")";
        resultStack.push(unaryExpression);
      }
      else
      {
        QString operand2 = resultStack.pop();
        QString operand1 = resultStack.pop();
        QString binaryExpression = "(" + operand1 + token.getLexeme() + operand2 + ")";
        resultStack.push(binaryExpression);
      }
    }
  }

  return resultStack.pop();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FilterNode* Postfix::getFilterTree(SearchType searchType, bool matchCase, bool wholeWord, Filter* filter)
{
  QStack<FilterNode*> resultStack;

  for (Token& token : postfix)
  {
    if (isOperand(token))
    {
      QRegularExpression regexp = createRegexp(token.getLexeme(), searchType, matchCase, wholeWord);

      resultStack.push(new FilterNodeOperand(regexp, filter));
    }
    else
    {
      if (token.getType() == Token::Not)
      {
        FilterNode* operand = resultStack.pop();
        resultStack.push(new FilterNodeNot(operand));
      }
      else
      {
        FilterNode* operand2 = resultStack.pop();
        FilterNode* operand1 = resultStack.pop();

        if (token.getType() == Token::And)
        {
          resultStack.push(new FilterNodeAnd(operand1, operand2));
        }
        else if (token.getType() == Token::Or)
        {
          resultStack.push(new FilterNodeOr(operand1, operand2));
        }
      }
    }
  }

  return resultStack.isEmpty() ? nullptr : resultStack.pop();
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Postfix::create()
{
  QStack<Token> operatorStack;

  Token previousToken;

  for (Token token : tokens)
  {
    if (isOperand(token))
    {
      postfix.enqueue(token);
    }
    else if (isOperator(token))
    {
      while (!operatorStack.isEmpty())
      {
        if (isOperator(operatorStack.top()) && operatorStack.top().getPrecedence() < token.getPrecedence())
        {
          postfix.enqueue(operatorStack.pop());
        }
        else
        {
          break;
        }
      }

      operatorStack.push(token);
    }
    else if (token.getType() == Token::LeftParenthesis)
    {
      operatorStack.push(token);
    }
    else if (token.getType() == Token::RightParenthesis)
    {
      while (!operatorStack.isEmpty() && !(operatorStack.top().getType() == Token::LeftParenthesis))
      {
        postfix.enqueue(operatorStack.pop());
      }

      if (operatorStack.isEmpty())
      {
        qDebug() << "Parenthesis structure is invalid at " << token.getPosition();
      }
      else
      {
        // remove left parenthesis '('
        operatorStack.pop();
      }
    }
    else
    {
      qDebug() << "Could not recognize a token: " << token.getLexeme();
    }

    previousToken = token;
  }

  while (!operatorStack.isEmpty())
  {
    Token op = operatorStack.pop();

    // Parenthesis structure is invalid.
    if (op.getType() == Token::LeftParenthesis || op.getType() == Token::RightParenthesis)
    {
      qDebug() << "Parenthesis structure is invalid at " << op.getPosition();
    }

    postfix.enqueue(op);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool Postfix::isOperand(const Token& token)
{
  return token.getType() == Token::Operand;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
bool Postfix::isOperator(const Token& token)
{
  return (token.getType() == Token::Not || token.getType() == Token::And || token.getType() == Token::Or);
}