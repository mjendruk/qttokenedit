#pragma once

#include <qtadvwidgets/TokenChainElement.h>
#include <qtadvwidgets/TokenEditMode.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

#include <QObject>
#include <cstdint>
#include <memory>

class QLineEdit;

class QTADVWIDGETS_API TokenChain : public QObject {
  Q_OBJECT
 public:
  TokenChain(TokenEditMode mode, QLineEdit* last, QObject* parent = nullptr);
  ~TokenChain();

  void add(TokenChainElement* element);
  void insert(int index, TokenChainElement* element);
  void move(int from, int to);
  void remove(TokenChainElement* element);

  void setMode(TokenEditMode mode);

 signals:
  void gotFocus(TokenChainElement* element);
  void lostFocus(TokenChainElement* element);

 private:
  enum class UpdateSignalConnection : std::int8_t { Yes, No };
  enum class UpdateFocus : std::int8_t { Yes, No };

  void insert(int index, TokenChainElement* element,
              UpdateSignalConnection updateConnection, UpdateFocus updateFocus);
  
  void remove(TokenChainElement* element,
              UpdateSignalConnection updateConnection, UpdateFocus updateFocus);

  TokenChainElement* takeAt(int index, UpdateSignalConnection updateConnection,
                            UpdateFocus updateFocus);

  int indexLast() const;
  TokenChainElement* at(int index) const;

 private:
  std::unique_ptr<TokenChainElement> _last;
  int _size;
  TokenEditMode _mode;
};
