#pragma once

class TokenEditTest;

namespace mjendruk {

struct TokenEditPasskey {
  friend class ::TokenEditTest;

 private:
  TokenEditPasskey() {}
  TokenEditPasskey(TokenEditPasskey const&) = delete;
};

}  // namespace mjendruk
