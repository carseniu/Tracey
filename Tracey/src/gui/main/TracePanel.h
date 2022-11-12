#pragma once

#include <QFrame>

#include "src/gui/core/HighlightingGroup.h"

class ComboBox;
class HighlightingData;
class HighlightingGroup;
class TraceWidget;


class TracePanel : public QFrame
{
  Q_OBJECT

public:
  TracePanel(HighlightingData* highlightingData, TraceWidget* parent);

  QWidget* getHighlightingWidget() const;
  QString getHighlightingGroupId() const;
  void setHighlightingGroupId(const QString& highlightingGroupId);
  virtual void updateShortcuts() = 0;

private:
  void setHighlightingGroupIndex(int index);

  HighlightingData* highlightingData;
  ComboBox* highlightingComboBox;
  QWidget* highlightingWidget;

private slots:
  void processHighlightingChanged(const QString& groupId);
  void processHighlightingGroupChanged(int index);
  void setHighlightingGroups(const QVector<HighlightingGroup*>& groups);

signals:
  void highlightingGroupChanged(const QString& groupId);
  void saveRequested();

};