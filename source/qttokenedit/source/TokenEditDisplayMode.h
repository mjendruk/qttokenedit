#pragma once

#include "TokenEditMode.h"

namespace mjendruk {

class TokenEditView;
class OmissionToken;
class FinalWidgetPlaceholder;

class TokenEditDisplayMode : public TokenEditMode {
  Q_OBJECT
 public:
  TokenEditDisplayMode(TokenEditView* view, AbstractTokenEditModeAccess* access,
                       QObject* parent = nullptr);

  ~TokenEditDisplayMode();

  void inserted(int first, int last, UpdateFocus uf) override;
  void removed(int first, int last, UpdateFocus uf) override;
  void moved(int first, int last, int to) override;
  void changed(int first, int last, QVector<int> const& roles) override;

  void invalidate() override;
  int heightHint() const override;

  void activate() override;
  void deactivate() override;

  QWidget* omissionToken() const;

 private:
  int omittedTokens() const;

 private:
  OmissionToken* _omissionToken;
};

}  // namespace mjendruk
