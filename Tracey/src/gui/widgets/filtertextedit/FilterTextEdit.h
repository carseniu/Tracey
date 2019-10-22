#pragma once

#include <QPlainTextEdit>

class CommandManager;
class FilterHighlighter;


class FilterTextEdit : public QPlainTextEdit
{
  Q_OBJECT

public:
  FilterTextEdit(QWidget* parent = nullptr);

  void addOperator(const QChar& filterOperator);
  void addOperatorPositions(const QVector<int>& operatorPositions, int start);
  void deleteText(int start, int n);
  QVector<int> getOperatorPositions() const;
  QVector<int> getOperatorPositions(int start, int n) const;
  void highlightError(int position);
  void insertText(const QString& text, const QVector<int>& operatorPositions, int start);
  void setOperatorPositions(const QVector<int>& operatorPositions);
  void setSelectionAndCursorPosition(int position, int selectionStart, int selectionEnd);

protected:
  void contextMenuEvent(QContextMenuEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

private:
  void processBackspace();
  void processCopy();
  void processCut();
  void processDelete();
  void processPaste();
  void removeOperators(int start, int end);
  void shiftOperators(int index, int delta);

  QVector<int> operatorPositions;
  FilterHighlighter* filterHighlighter;
  CommandManager* commandManager;

signals:
  void saveRequested();

};