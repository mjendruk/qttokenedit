#pragma once

#include <token-edit/TokenEditMode.h>
#include <token-edit/token-edit-api.h>

namespace mjendruk {

class TokenEditView;
class TokenLineEdit;

class TOKEN_EDIT_API TokenEditEditingMode : public TokenEditMode {
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
