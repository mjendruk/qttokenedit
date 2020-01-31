#pragma once

#include <qtadvwidgets/qtadvwidgets_api.h>

#include <qtadvwidgets/TokenEditMode.h>

class TokenEditView;
class OmissionToken;

class QTADVWIDGETS_API TokenEditDisplayMode : public TokenEditMode {
  Q_OBJECT
 public:
  TokenEditDisplayMode(TokenEditView* view, TokenEditModeAccess* access,
                       QObject* parent = nullptr);
  
  ~TokenEditDisplayMode();
  
  void inserted(int first, int last) override;
  void removed(int first, int last) override;
  void moved(int first, int last, int to) override;
  void changed(int first, int last) override;

  void clear() override;
  void invalidate() override;
  int heightHint() const override;
  
  void activate() override;
  void deactivate() override;
  
 private:
  int omittedTokens() const;
 
 private:
  OmissionToken* _omissionToken;
};
