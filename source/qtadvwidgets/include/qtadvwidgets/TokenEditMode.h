#pragma once

enum class TokenEditMode : std::int8_t {
  NeverShowLineEdit,
  ShowLineEditIfEmpty,
  AlwaysShowLineEdit,
};
