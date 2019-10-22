#pragma once

#include <QDialog>

class QLabel;


class AboutTraceyDialog : public QDialog
{
  Q_OBJECT

public:
  AboutTraceyDialog(QWidget* parent);

protected:
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  QLabel* pictureLabel;

};