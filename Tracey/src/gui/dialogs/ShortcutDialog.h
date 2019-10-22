#pragma once

#include <QDialog>

class QLabel;


class ShortcutDialog : public QDialog
{
  Q_OBJECT

public:
  static QKeySequence getShortcut(const QList<QAction*>& actionList, int actionIndex, QWidget* parent);

protected:
  void keyPressEvent(QKeyEvent* event) override;

private:
  ShortcutDialog(const QList<QAction*>& actionList, int actionIndex, QWidget* parent);

  void checkDuplicate();

  QList<QAction*> actionList;

  QLabel* actionIconLabel;
  QLabel* actionTextLabel;

  QLabel* actionExistsLabel;
  QLabel* existingActionIconLabel;
  QLabel* existingActionTextLabel;

  QString actionString;

  int actionIndex;

  QKeySequence shortcut;

  static QKeySequence s_shortcut;

private slots:
  void accept();

};
