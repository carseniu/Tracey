#pragma once

#include <QIntValidator>


class IntValidator : public QIntValidator
{
public:

  IntValidator(int min, int max, QObject* parent);

  QValidator::State validate(QString& input, int& pos) const;

};