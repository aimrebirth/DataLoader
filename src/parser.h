#pragma once

#include <string>
#include <vector>

#include "data.h"
#include "token.h"

Schema parse(Tokens &tokens);
Schema parse_string(const std::string &s, Tokens *tokens = nullptr);
Schema parse_file(const std::string &filename, Tokens *tokens = nullptr);
