#include <qtadvwidgets/TokenEditMode.h>

TokenEditMode::TokenEditMode(TokenEditView* view, TokenEditModeAccess* access,
                             QObject* parent)
    : QObject{parent}, _view{view}, _access{access} {}

TokenEditView* TokenEditMode::view() const { return _view; }

TokenEditModeAccess* TokenEditMode::access() const { return _access; }
