/**
 * These codes are licensed under LICNSE_NAME License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#include "cmd.hpp"
#include <boost/program_options.hpp>
#include <spica/support/utils.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <iostream>

namespace program_options = boost::program_options;

namespace
{

[[nodiscard]] program_options::options_description createOptionsDesc()
{
  program_options::options_description desc{"Options"};

  // clang-format off
  desc.add_options()
    ("help,h", "Display this information.")
    ("version,v", "Display version.")
    ("JIT", "Perform Just-in-time(JIT) compilation.\n"
      "If there are multiple input files, they are linked and executed.")
    ("emit", program_options::value<std::string>(),
      "Set a compilation target. Assembly file is 'asm', "
      "object file is 'obj', LLVM IR is 'llvm'.\n"
      "If there are multiple input files, compile each to the target. Not linked.")
    ("Opt,O", program_options::value<unsigned int>()->default_value(spica::DEFAULT_OPT_LEVEL),
      "Specify the optimization level.\n"
      "Possible values are 0 1 2 3 and the meaning is the same as clang.")
    ("relocation-model",
      program_options::value<std::string>()->default_value("pic"),
      "Set the relocation model. Possible values are 'static' or 'pic'.\n"
      "If llvm is specified for the emit option, this option is disabled.")
    ("input-file", program_options::value<std::vector<std::string>>(),
      "Input file. Non-optional arguments are equivalent to this.")
    ;
  // clang-format on

  return desc;
}

[[nodiscard]] program_options::variables_map
getVariableMap(const program_options::options_description& desc,
               const int                                   argc,
               const char* const* const                    argv)
{
  program_options::positional_options_description p;

  p.add("input-file", -1);

  program_options::variables_map v_map;
  program_options::store(program_options::command_line_parser(argc, argv)
                           .options(desc)
                           .positional(p)
                           .run(),
                         v_map);
  program_options::notify(v_map);

  return v_map;
}

[[nodiscard]] std::vector<std::string>
getInputFiles(const program_options::variables_map& v_map)
{
  if (v_map.contains("input-file"))
    return v_map["input-file"].as<std::vector<std::string>>();
  else
    return std::vector<std::string>{};
}

std::ostream& writeHelp(std::ostream&                               ostm,
                        const std::string_view                      command,
                        const program_options::options_description& desc)
{
  fmt::print(ostm, "Usage: {} [options] file...\n", command);
  return ostm << desc;
}

} // namespace

namespace spica
{

[[nodiscard]] Context parseCmdlineOption(const int                argc,
                                         const char* const* const argv)
try {
  const auto desc = createOptionsDesc();

  const auto v_map = getVariableMap(desc, argc, argv);

  if (argc == 1) {
    writeHelp(std::cerr, *argv, desc);
    std::exit(EXIT_SUCCESS);
  }
  if (v_map.contains("version")) {
    std::cout << "spica version " << getVersion() << std::endl;
    std::exit(EXIT_SUCCESS);
  }
  else if (v_map.contains("help")) {
    writeHelp(std::cout, *argv, desc);
    std::exit(EXIT_SUCCESS);
  }

  auto input_files = getInputFiles(v_map);

  if (input_files.empty()) {
    std::cerr << formatError(*argv, "no input files\n") << std::flush;
    std::exit(EXIT_FAILURE);
  }

  return {std::move(input_files),
          v_map.contains("JIT"),
          v_map.contains("emit")
            ? std::make_optional(stringToLower(v_map["emit"].as<std::string>()))
            : std::nullopt,
          v_map["Opt"].as<unsigned int>(),
          stringToLower(v_map["relocation-model"].as<std::string>())};
}
catch (const program_options::error& err) {
  std::cerr << formatError(*argv, err.what())
            << (isBackNewline(err.what()) ? "" : "\n") << std::flush;
  std::exit(EXIT_FAILURE);
}

} // namespace spica