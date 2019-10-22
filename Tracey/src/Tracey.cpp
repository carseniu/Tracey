#include "src/Tracey.h"

#include <QDialog>
#include <QPushButton>


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QRegularExpression Tracey::createRegexp(const QString& text, SearchType searchType, bool matchCase, bool wholeWord)
{
  QRegularExpression regexp;

  QString pattern = "";

  switch (searchType)
  {
  case NormalSearch:
    pattern = QRegularExpression::escape(text);
    break;

  case WildcardSearch:
    pattern = QRegularExpression::escape(text);
    pattern.replace("\\?", ".");
    pattern.replace("\\*", ".*");
    break;

  case RegexpSearch:
    pattern = text;
    break;
  }

  if (wholeWord)
  {
    pattern.prepend("\\b");
    pattern.append("\\b");
  }

  if (matchCase)
  {
    regexp = QRegularExpression(pattern);
  }
  else {
    regexp = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
  }

  return regexp;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void Tracey::resizeDialogButton(QDialog* dialog, QDialogButtonBox::StandardButton standardButton, int width, int height)
{
  QDialogButtonBox* box = dialog->findChild<QDialogButtonBox*>();

  if (box)
  {
    QPushButton* button = box->button(standardButton);

    if (button)
    {
      button->setMinimumWidth(width);
      button->setMinimumHeight(height);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QPixmap Tracey::setLightness(const QPixmap& pixmap, int factor)
{
  QImage image = pixmap.toImage();

  for (int i = 0; i < image.width(); ++i)
    for (int j = 0; j < image.height(); ++j)
    {
      QColor color = image.pixelColor(i, j);

      image.setPixelColor(i, j, color.lighter(factor));
    }

  return QPixmap::fromImage(image);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QPixmap Tracey::setSaturation(const QPixmap& pixmap, qreal level)
{
  QImage image = pixmap.toImage();

  for (int i = 0; i < image.width(); ++i)
    for (int j = 0; j < image.height(); ++j)
    {
      QColor color = image.pixelColor(i, j);

      qreal h;
      qreal s;
      qreal l;
      qreal a;

      color.getHslF(&h, &s, &l, &a);
      color.setHslF(h, s * level, l, a);

      image.setPixelColor(i, j, color);
    }

  return QPixmap::fromImage(image);
}