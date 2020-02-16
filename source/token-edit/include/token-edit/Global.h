#pragma once

#include <cstdint>

namespace mjendruk {

enum class UpdateFocus : std::int8_t { Yes, No };

enum class TokenDropHint : std::int8_t { Before, After };

enum class DropIndicatorPosition : std::int8_t { None, Before, After };

enum class ShowLineEdit : std::int8_t { Never, Always };

}  // namespace mjendruk