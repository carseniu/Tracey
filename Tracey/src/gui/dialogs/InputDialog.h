#pragma once

#include <QDialog>

class QLabel;
class QLineEdit;


class InputDialog : public QDialog
{
  Q_OBJECT

public:
  InputDialog(QWidget* parent, const QString& title, const QString& label, const QString& value = "");

  QString getTextValue() const;

private:
  QLineEdit* inputLineEdit;

};