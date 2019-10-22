#include "src/gui/dialogs/AboutQtDialog.h"

#include <QFile>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
AboutQtDialog::AboutQtDialog(QWidget* parent) :
  QDialog(parent)
{
  setFixedSize(500, 500);
  setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
  setAttribute(Qt::WA_DeleteOnClose);

  qtLogoLabel = new QLabel(this);
  qtLogoLabel->setPixmap(QPixmap(":/Application/img/Qt.png"));
  qtLogoLabel->setStyleSheet("QLabel{border:1px solid transparent}");

  QFile styleFile(":/Application/qt/AboutQt.html");
  styleFile.open(QFile::ReadOnly);
  QString aboutQt(styleFile.readAll());

  aboutQtTextEdit = new QTextBrowser(this);
  aboutQtTextEdit->setOpenExternalLinks(true);
  aboutQtTextEdit->setHtml(aboutQt);

  okButton = new QPushButton(tr("OK"), this);
  okButton->setFixedSize(80, 20);
  okButton->setFocus(Qt::TabFocusReason);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

  QGridLayout* layout = new QGridLayout(this);
  layout->setMargin(10);
  layout->setVerticalSpacing(10);
  layout->addWidget(qtLogoLabel, 0, 0, 1, 1, Qt::AlignTop);
  layout->addWidget(aboutQtTextEdit, 0, 1, 1, 1);
  layout->addWidget(okButton, 1, 0, 1, 2, Qt::AlignRight);
}