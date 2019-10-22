#pragma once

#include <QDialog>

class QLabel;
class QTextBrowser;


class AboutQtDialog : public QDialog
{
  Q_OBJECT

public:
  AboutQtDialog(QWidget* parent);

private:
  QLabel* qtLogoLabel;
  QTextBrowser* aboutQtTextEdit;
  QPushButton* okButton;
  
};