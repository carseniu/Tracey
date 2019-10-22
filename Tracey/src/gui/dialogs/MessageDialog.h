#pragma once

#include <QDialog>

class QCheckBox;
class QLabel;
class QLineEdit;


class MessageDialog : public QDialog
{
  Q_OBJECT

public:
  enum Type
  {
    Error,
    Info,
    Question,
    Warning
  };

  MessageDialog(QWidget* parent, const QString& message, Type type = Info, bool showDontAskAgain = false);

  bool getDontAskAgain() const;

private:
  QCheckBox* dontAskAgainCheckBox;

};