#pragma once

#include <cstdint>

enum class UpdateFocus : std::int8_t { Yes, No };

enum class TokenDropHint : std::int8_t { Before, After };

enum class DropIndicatorPosition : std::int8_t { None, Before, After };
