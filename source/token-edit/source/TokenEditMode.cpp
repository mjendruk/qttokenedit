#include "TokenEditMode.h"

namespace mjendruk {

TokenEditMode::TokenEditMode(TokenEditView* view,
                             AbstractTokenEditModeAccess* access,
                             QObject* parent)
    : QObject{parent}, _view{view}, _access{access} {}

TokenEditView* TokenEditMode::view() const { return _view; }

AbstractTokenEditModeAccess* TokenEditMode::access() const { return _access; }

}  // namespace mjendruk
