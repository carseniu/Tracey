#pragma once

#include <QObject>

#include "src/Tracey.h"
#include "src/logic/filter/Scanner.h"

class FilterNode;

using namespace Tracey;


class Filter : public QObject
{
  Q_OBJECT

public:
  Filter(QObject* parent = nullptr);
  ~Filter();

  bool check(const QString& text);
  bool create(const QString& expression, const QVector<int>& operatorPositions, SearchType searchType, bool matchCase, bool wholeWord);
  Scanner::ScannerResult getScannerResult() const;
  QString getText() const;
  bool hasRootNode() const;

private:
  FilterNode* rootNode;
  Scanner::ScannerResult scannerResult;
  QString text;

signals:
  void errorFound(const QString& message, int position);

};