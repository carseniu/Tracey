#include "src/gui/dialogs/FontDialog.h"

#include "src/gui/widgets/list/List.h"

#include <QCheckBox>
#include <QFontDatabase>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QVBoxLayout>


//===================================================================================================================================================================================================//
// PUBLIC
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
FontDialog::FontDialog(const QFont& font, bool onlyMonospacedFonts, bool showMonospacedOption, QWidget* parent) :
  QDialog(parent),
  font(font)
{
  setFixedSize(500, 330);
  setWindowIcon(QIcon(":/Application/img/Icon.png"));
  setWindowTitle(tr("Font"));
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  // The lists
  familyList = new List(this);
  familyList->setFixedSize(230, 200);
  QScrollBar* familyListScrollBar = new QScrollBar(Qt::Vertical, familyList);
  familyListScrollBar->setObjectName("SmallScrollBar");
  familyList->setVerticalScrollBar(familyListScrollBar);

  styleList = new List(this);
  styleList->setFixedSize(150, 200);
  QScrollBar* styleListScrollBar = new QScrollBar(Qt::Vertical, styleList);
  styleListScrollBar->setObjectName("SmallScrollBar");
  styleList->setVerticalScrollBar(styleListScrollBar);

  sizeList = new List(this);
  sizeList->setFixedHeight(200);
  QScrollBar* sizeListScrollBar = new QScrollBar(Qt::Vertical, sizeList);
  sizeListScrollBar->setObjectName("SmallScrollBar");
  sizeList->setVerticalScrollBar(sizeListScrollBar);

  QHBoxLayout* topLayout = new QHBoxLayout;
  topLayout->setMargin(0);
  topLayout->setSpacing(5);
  topLayout->addWidget(familyList);
  topLayout->addWidget(styleList);
  topLayout->addWidget(sizeList);

  // Monospace fonts
  monospacedCheckBox = new QCheckBox(tr("Show only monospaced fonts"), this);
  if (onlyMonospacedFonts)
  {
    monospacedCheckBox->setChecked(true);
  }
  if (!showMonospacedOption)
  {
    monospacedCheckBox->setVisible(false);
  }

  // Sample text
  sampleText = new QLabel("", this);
  sampleText->setObjectName("FontDialogSampleText");
  sampleText->setAlignment(Qt::AlignCenter);
  sampleText->setMinimumHeight(90);
  sampleText->setText("AaBbCc");

  // The buttons
  okButton = new QPushButton(tr("OK"), this);
  okButton->setFixedSize(80, 20);
  okButton->setFocus(Qt::TabFocusReason);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

  cancelButton = new QPushButton(tr("Cancel"), this);
  cancelButton->setFixedSize(80, 20);
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  QVBoxLayout* buttonsLayout = new QVBoxLayout;
  buttonsLayout->setContentsMargins(0, 0, 0, 0);
  buttonsLayout->addStretch();
  buttonsLayout->addWidget(okButton);
  buttonsLayout->addWidget(cancelButton);

  // The bottom layout
  QHBoxLayout* bottomLayout = new QHBoxLayout;
  bottomLayout->addWidget(sampleText);
  bottomLayout->addLayout(buttonsLayout);

  // All together now, bitch
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(3);
  layout->setSpacing(5);
  layout->addLayout(topLayout);
  layout->addWidget(monospacedCheckBox);
  layout->addLayout(bottomLayout);

  // Init font lists
  setFont();

  // Connection
  connect(monospacedCheckBox, &QCheckBox::stateChanged, this, &FontDialog::updateFonts);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QFont FontDialog::getFont() const
{
  return font;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QFont FontDialog::getFont(const QFont& font, bool onlyMonospacedFonts, bool showMonospacedOption, QWidget* parent)
{
  FontDialog fontDialog(font, onlyMonospacedFonts, showMonospacedOption, parent);
  fontDialog.exec();

  return fontDialog.getFont();
}


//===================================================================================================================================================================================================//
// PRIVATE
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
QFont FontDialog::createFont()
{
  QFontDatabase fdb;
  QFont font;
  QString family = familyList->currentItem()->text();
  QString style = styleList->currentItem()->text();

  font.setFamily(family);
  font.setWeight(fdb.weight(family, style));
  font.setItalic (fdb.italic(family, style));
  font.setPointSize(sizeList->currentItem()->text().toInt());

  return font;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FontDialog::fillFamilyList()
{
  QFontDatabase fdb;

  QStringList families = fdb.families();

  for (QString family : families)
  {
    if (monospacedCheckBox->isChecked())
    {
      if (fdb.isFixedPitch(family))
      {
        familyList->addItem(family);
      }
    }
    else
    {
      familyList->addItem(family);
    }

    if (family == font.family())
    {
      familyList->setCurrentRow(familyList->count() - 1);
    }
  }

  if (familyList->currentRow() == -1)
  {
    familyList->setCurrentRow(0);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FontDialog::fillSizeList()
{
  QFontDatabase fdb;

  QList<int> sizes = fdb.pointSizes(familyList->currentItem()->text(), styleList->currentItem()->text());

  for (int size : sizes)
  {
    sizeList->addItem(QString::number(size));
  }

  if (font.pointSize() == -1)
  {
    sizeList->setCurrentRow(0);
  }
  else
  {
    for (int i = 0; i < sizes.size(); ++i)
    {
      if (sizes.at(i) == font.pointSize())
      {
        sizeList->setCurrentRow(i);
      }
    }
  }

  if (sizeList->currentRow() == -1)
  {
    sizeList->setCurrentRow(0);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FontDialog::fillStyleList()
{
  QFontDatabase fdb;

  QStringList styles = fdb.styles(familyList->currentItem()->text());

  styleList->addItems(styles);

  for (int i = 0; i < styles.size(); ++i)
  {
    if (styles.at(i) == fdb.styleString(font))
    {
      styleList->setCurrentRow(i);
      break;
    }
  }

  if (styleList->currentRow() == -1)
  {
    styleList->setCurrentRow(0);
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FontDialog::setFont()
{
  QObject::disconnect(familyList, &QListWidget::currentTextChanged, this, &FontDialog::processFamilyChanged);
  QObject::disconnect(styleList, &QListWidget::currentTextChanged, this, &FontDialog::processStyleChanged);
  QObject::disconnect(sizeList, &QListWidget::currentTextChanged, this, &FontDialog::processSizeChanged);

  familyList->clear();
  styleList->clear();
  sizeList->clear();

  fillFamilyList();
  fillStyleList();
  fillSizeList();

  font = createFont();
  sampleText->setFont(font);

  connect(familyList, &QListWidget::currentTextChanged, this, &FontDialog::processFamilyChanged);
  connect(styleList, &QListWidget::currentTextChanged, this, &FontDialog::processStyleChanged);
  connect(sizeList, &QListWidget::currentTextChanged, this, &FontDialog::processSizeChanged);
}


//===================================================================================================================================================================================================//
// PRIVATE SLOTS
//===================================================================================================================================================================================================//

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FontDialog::processFamilyChanged(const QString& family)
{
  QObject::disconnect(styleList, &QListWidget::currentTextChanged, this, &FontDialog::processStyleChanged);
  QObject::disconnect(sizeList, &QListWidget::currentTextChanged, this, &FontDialog::processSizeChanged);

  styleList->clear();
  sizeList->clear();

  fillStyleList();
  fillSizeList();

  font = createFont();
  sampleText->setFont(font);

  connect(styleList, &QListWidget::currentTextChanged, this, &FontDialog::processStyleChanged);
  connect(sizeList, &QListWidget::currentTextChanged, this, &FontDialog::processSizeChanged);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FontDialog::processSizeChanged(const QString& size)
{
  font = createFont();
  sampleText->setFont(getFont());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FontDialog::processStyleChanged(const QString& style)
{
  QObject::disconnect(sizeList, &QListWidget::currentTextChanged, this, &FontDialog::processSizeChanged);

  sizeList->clear();

  fillSizeList();

  font = createFont();
  sampleText->setFont(font);

  connect(sizeList, &QListWidget::currentTextChanged, this, &FontDialog::processSizeChanged);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void FontDialog::updateFonts(int state)
{
  setFont();
}