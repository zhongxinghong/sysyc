//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: compiler.cc
//  Created Date: 2021-05-24
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "facade/compiler.h"
#include "frontend/parser.h"
#include "backend/ast/evaluator/evaluator.h"
#include "backend/quad/ast2quad/ast2quad.h"
#include "backend/quad/regalloc/regalloc.h"
#include "backend/quad/quadpass/quadpass.h"
#include "backend/sysy/codegen.h"
#include "backend/eeyore/codegen.h"
#include "backend/tigger/codegen.h"
#include "backend/riscv/codegen.h"

namespace sysyc
{
namespace facade
{
using namespace sysyc::frontend;
using namespace sysyc::backend::ast;
using namespace sysyc::backend::quad;
using namespace sysyc::backend::sysy;
using namespace sysyc::backend::eeyore;
using namespace sysyc::backend::tigger;
using namespace sysyc::backend::riscv;

Compiler::~Compiler()
{
  if (fin.is_open())
    fin.close();

  if (fout.is_open())
    fout.close();

  for (auto it = instseq.list.begin(), e = instseq.list.end(); it != e; ++it)
    delete (*it);

  delete ast;
}

void Compiler::report_error(const std::string &errmsg)
{
  logger.error("[Compiler] " + errmsg);
}

bool Compiler::run_parser()
{
  fin.open(input_file.c_str(), std::ios_base::in);
  assert(fin.is_open());

  Lexer lexer(fin, logger);
  Parser parser(lexer, logger);

  ast = parser.generate_ast();

  if (ast == nullptr)
  {
    report_error("run_parser failed");
    return false;
  }

  return true;
}

bool Compiler::run_evaluator()
{
  ASTEvaluator evaluator(logger);

  ast->analyze_on(evaluator);

  auto &errcnt = evaluator.get_errcnt();

  if (errcnt != 0)
  {
    report_error("run_evaluator failed, encount " +
                  std::to_string(errcnt) + " errors");
    return false;
  }

  return true;
}

bool Compiler::run_ast2quad()
{
  QuadrupleGenerator quadgen(instseq, logger);

  quadgen.link("getint", true);
  quadgen.link("getch", true);
  quadgen.link("getarray", true);
  quadgen.link("putint", false);
  quadgen.link("putch", false );
  quadgen.link("putarray", false);
  quadgen.link("starttime", "_sysy_starttime", false);
  quadgen.link("stoptime", "_sysy_stoptime", false);

  if (!ast->generate_on(quadgen))
  {
    report_error("run_ast2quad failed");
    return false;
  }

  return true;
}

bool Compiler::run_ast2quad_pass()
{
  using namespace sysyc::backend::quad::pass;

  if (!mem_check(instseq, logger))
  {
    report_error("run_ast2quad_pass.mem_check failed (pass 1)");
    return false;
  }

  if (!reorder_decl(instseq, logger))
  {
    report_error("run_ast2quad_pass.reorder_decl failed");
    return false;
  }

  if (!reduce_label(instseq, logger))
  {
    report_error("run_ast2quad_pass.reduce_label failed");
    return false;
  }

  if (!adjust_expr(instseq, logger))
  {
    report_error("run_ast2quad_pass.adjust_expr failed");
    return false;
  }

  if (!mem_check(instseq, logger))
  {
    report_error("run_ast2quad_pass.mem_check failed (pass 2)");
    return false;
  }

  return true;
}

bool Compiler::run_regalloc()
{
  RegisterAllocator regalloc(logger);

  if (!regalloc.run(instseq))
  {
    report_error("run_regalloc falied");
    return false;
  }

  return true;
}

bool Compiler::run_riscv_pass()
{
  using namespace sysyc::backend::quad::pass;

  if (!mem_check(instseq, logger))
  {
    report_error("run_riscv_pass.mem_check failed (pass 1)");
    return false;
  }

  if (!pow2_to_logical(instseq, logger))
  {
    report_error("run_riscv_pass.pow2_to_logical failed");
    return false;
  }

  if (!mem_check(instseq, logger))
  {
    report_error("run_riscv_pass.mem_check failed (pass 1)");
    return false;
  }

  return true;
}

bool Compiler::generate_sysy()
{
  fout.open(output_file.c_str(), std::ios_base::out);
  assert(fout.is_open());

  SysYCodeGenerator codegen(2, logger);

  auto ok = ast->generate_on(codegen);

  if (!ok)
  {
    report_error("generate_sysy failed");
    codegen.dump(fout);
    return false;
  }

  codegen.dump(fout);
  return true;
}

bool Compiler::generate_eeyore()
{
  fout.open(output_file.c_str(), std::ios_base::out);
  assert(fout.is_open());

  EeyoreCodeGenerator codegen(2, logger);

  if (!codegen.generate(instseq, fout))
  {
    report_error("generate_eeyore failed");
    return false;
  }

  return true;
}

bool Compiler::generate_tigger()
{
  fout.open(output_file.c_str(), std::ios_base::out);
  assert(fout.is_open());

  TiggerCodeGenerator codegen(2, logger);

  if (!codegen.generate(instseq, fout))
  {
    report_error("generate_tigger failed");
    return false;
  }

  return true;
}

bool Compiler::generate_riscv()
{
  fout.open(output_file.c_str(), std::ios_base::out);
  assert(fout.is_open());

  RISCVCodeGenerator codegen(2, logger);

  if (!codegen.generate(instseq, fout))
  {
    report_error("generate_riscv failed");
    return false;
  }

  return true;
}

bool Compiler::run(const Config &config)
{
  input_file = config.input_file;
  output_file = config.output_file;

  if (!run_parser())
    return false;

  if (!run_evaluator())
    return false;

  if (config.target == Config::Target::SYSY)
    return generate_sysy();

  if (!run_ast2quad())
    return false;

  if (!run_ast2quad_pass())
    return false;

  if (config.target == Config::Target::EEYORE)
    return generate_eeyore();

  if (!run_regalloc())
    return false;

  if (config.target == Config::Target::TIGGER)
    return generate_tigger();

  if (!run_riscv_pass())
    return false;

  if (config.target == Config::Target::RISCV)
    return generate_riscv();

  return true;
}

} // namespace facade

} // namespace sysyc
