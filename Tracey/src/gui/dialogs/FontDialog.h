#pragma once

#include <QDialog>

class List;

class QCheckBox;
class QLabel;


class FontDialog : public QDialog
{
  Q_OBJECT

public:
  FontDialog(const QFont& font, bool onlyMonospacedFonts, bool showMonospacedOption, QWidget* parent);

  QFont getFont() const;
  static QFont getFont(const QFont& font, bool onlyMonospacedFonts, bool showMonospacedOption, QWidget* parent);

private:
  QFont createFont();
  void fillFamilyList();
  void fillSizeList();
  void fillStyleList();
  void setFont();

  List* familyList;
  List* styleList;
  List* sizeList;

  QCheckBox* monospacedCheckBox;

  QLabel* sampleText;
  QPushButton* okButton;
  QPushButton* cancelButton;

  QFont font;

private slots:
  void processFamilyChanged(const QString& family);
  void processSizeChanged(const QString& size);
  void processStyleChanged(const QString& style);
  void updateFonts(int state);

};