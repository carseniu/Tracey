#pragma once

#include <QDialog>

class QLabel;
class QProgressBar;


class SearchProgressDialog : public QDialog
{
  Q_OBJECT

public:
  SearchProgressDialog(QWidget* parent);

public slots:
  void processMatchNotFound();
  void setCount(int count);
  void setStep(int step);

private:
  QLabel* titleLabel;
  QLabel* messageLabel;
  QProgressBar* progressBar;
  QPushButton* cancelButton;
  QPushButton* okButton;

};