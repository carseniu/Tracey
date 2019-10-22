#include "src/gui/dialogs/AboutTraceyDialog.h"

#include <QHBoxLayout>
#include <QLabel>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
AboutTraceyDialog::AboutTraceyDialog(QWidget* parent) :
  QDialog(parent)
{
  setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
  setAttribute(Qt::WA_DeleteOnClose);

  pictureLabel = new QLabel(this);
  pictureLabel->setPixmap(QPixmap(":/Application/img/Tracey.png"));

  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addWidget(pictureLabel);
}


//===================================================================================================================================================================================================//
// PROTECTED
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void AboutTraceyDialog::mouseReleaseEvent(QMouseEvent* event)
{
  accept();
}