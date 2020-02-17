#pragma once

#include "TokenEditMode.h"

namespace mjendruk {

class TokenEditView;
class TokenLineEdit;

class TokenEditEditingMode : public TokenEditMode {
  Q_OBJECT
 public:
  TokenEditEditingMode(TokenEditView* view, AbstractTokenEditModeAccess* access,
                       QObject* parent = nullptr);

  ~TokenEditEditingMode();

  void inserted(int first, int last, UpdateFocus uf) override;
  void removed(int first, int last, UpdateFocus uf) override;
  void moved(int first, int last, int to) override;
  void changed(int first, int last, QVector<int> const& roles) override;

  void invalidate() override;
  int heightHint() const override;

  void activate() override;
  void deactivate() override;
};

}  // namespace mjendruk
