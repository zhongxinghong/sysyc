//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: main.cc
//  Created Date: 2021-04-27
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cstdlib>
#include <iostream>

#include "optparse/optparse.h"
#include "facade/compiler.h"

using namespace sysyc::facade;

bool parse_args(int argc, char const *argv[], Config &config)
{
  const std::string usage = \
    "Usage: sysyc FILE [OPTION]";

  const std::string description = \
    "A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)";

  auto parser = optparse::OptionParser()
    .usage(usage)
    .description(description);

  parser.add_option("-S")
        .action("store_true")
        .help("convert SysY to ASM");

  parser.add_option("-e")
        .action("store_true")
        .help("convert SysY to Eeyore IR");

  parser.add_option("-t")
        .action("store_true")
        .help("convert SysY to Tigger IR");

  parser.add_option("-o")
        .action("store")
        .dest("output_file")
        .metavar("FILE")
        .help("specify output file");

  auto options = parser.parse_args(argc, argv);
  auto args = parser.args();

  if (args.size() == 0)
  {
    std::cerr << "source file is missing" << std::endl;
    return false;
  }

  if (args.size() > 1)
  {
    std::cerr << "too many source file" << std::endl;
    return false;
  }

  config.input_file = args[0];

  if (!options.is_set("output_file"))
  {
    std::cerr << "output file is missing" << std::endl;
    return false;
  }

  config.output_file = options["output_file"];

  if (!options.get("S"))
  {
    config.target = Config::Target::SYSY;
  }
  else if (options.get("e"))
  {
    config.target = Config::Target::EEYORE;
  }
  else if (options.get("t"))
  {
    config.target = Config::Target::TIGGER;
  }
  else
  {
    config.target = Config::Target::RISCV;
  }

  return true;
}

int main(int argc, char const *argv[])
{
  Config config;
  Compiler compiler;

  if (!parse_args(argc, argv, config))
    return EXIT_FAILURE;

  if (!compiler.run(config))
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
