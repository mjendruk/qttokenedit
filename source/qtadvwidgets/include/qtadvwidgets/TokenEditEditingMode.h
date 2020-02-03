#pragma once

#include <qtadvwidgets/TokenEditMode.h>
#include <qtadvwidgets/qtadvwidgets_api.h>

class TokenEditView;
class TokenLineEdit;

class QTADVWIDGETS_API TokenEditEditingMode : public TokenEditMode {
  Q_OBJECT
 public:
  TokenEditEditingMode(TokenEditView* view, AbstractTokenEditModeAccess* access,
                       QObject* parent = nullptr);

  ~TokenEditEditingMode();

  void inserted(int first, int last) override;
  void removed(int first, int last) override;
  void moved(int first, int last, int to) override;
  void changed(int first, int last, QVector<int> const& roles) override;

  void clear() override;

  void invalidate() override;
  int heightHint() const override;

  void activate() override;
  void deactivate() override;

  TokenLineEdit* lineEdit() const;

 private:
  TokenLineEdit* _lineEdit;
};
