/**
 * file.cpp
 *
 * These codes are licensed under Apache-2.0 License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#include <support/file.hpp>
#include <support/format.hpp>
#include <support/utils.hpp>
#include <support/exception.hpp>

namespace maple
{

// Exception class for errors related to file operations.
struct FileError : public ErrorBase {
  explicit FileError(const std::string& what_arg)
    : ErrorBase{what_arg}
  {
  }
};

// Load a file to std::string.
[[nodiscard]] std::string loadFile(const std::string_view       program_name,
                                   const std::filesystem::path& path)
{
  if (!std::filesystem::exists(path)) {
    throw FileError{formatErrorMessage(
      program_name,
      format("%s: No such file or directory", path.string()))};
  }

  if (auto file = std::ifstream{path, std::ios_base::binary}) {
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
  }

  throw FileError{
    formatErrorMessage(program_name,
                       format("%s: Could not open file", path.string()))};
}

} // namespace maple