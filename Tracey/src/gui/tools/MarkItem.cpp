#include "src/gui/tools/MarkItem.h"

#include <QHBoxLayout>
#include <QLabel>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
MarkItem::MarkItem(int index, const QString& text, int indexSize, QWidget* parent) :
  QFrame(parent)
{
  QLabel* indexLabel = new QLabel("<b>" + QString::number(index + 1) + "</b>|", this);
  indexLabel->setFixedWidth(indexSize + 5);
  indexLabel->setFont(QFont("Courier New", 8));
  indexLabel->setStyleSheet("color:blue");
  indexLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

  QString displayText = text;
  if (text.right(1) == "\n")
  {
    displayText = text.left(text.size() - 1);
  }

  QLabel* textLabel = new QLabel(displayText, this);
  textLabel->setStyleSheet("padding-right:10px");
  textLabel->setFont(QFont("Courier New", 8));

  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(5);
  layout->addWidget(indexLabel);
  layout->addWidget(textLabel);
}