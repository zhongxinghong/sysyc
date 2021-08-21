//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: unit-lexer.cc
//  Created Date: 2021-05-01
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include "utils.hpp"
#include "frontend/lexer.h"

using sysyc::frontend::Lexer;

namespace
{
struct print_lcno
{
  Lexer &lexer;

  print_lcno(Lexer &lexer) : lexer(lexer) {}

  friend std::ostream &operator<<(std::ostream &os, const print_lcno &p)
  {
    os << ", lineno: " << p.lexer.get_lineno()
       << ", colno: " << p.lexer.get_colno();
    return os;
  }
};

} // namespace

static int test_lexer(Lexer &lexer, const char *output_file)
{
  using namespace sysyc::frontend;

  std::fstream fout;
  fout.open(output_file, std::ios_base::out);

  Token token;
  std::string s_kw, s_op;

  do
  {
    switch (token = lexer.next_token())
    {
    case Token::ID:
      fout << "(ID, " << lexer.id_val() << ")" << print_lcno(lexer) << std::endl;
      break;
    case Token::KEYWORD:
      assert(__impl::kw_enum2str(lexer.kw_val(), &s_kw));
      fout << "(KEYWORD, \"" << s_kw << "\")" << print_lcno(lexer) << std::endl;
      break;
    case Token::INTEGER:
      fout << "(INTEGER, " << lexer.int_val() << ")" << print_lcno(lexer) << std::endl;
      break;
    case Token::OPERATOR:
      assert(__impl::op_enum2str(lexer.op_val(), &s_op));
      fout << "(OPERATOR, \"" << s_op << "\")" << print_lcno(lexer) << std::endl;
      break;
    case Token::SEPARATOR:
      fout << "(SEPARATOR, '" << lexer.sep_val() << "')" << print_lcno(lexer) << std::endl;
      break;
    case Token::END:
      fout << "(END)" << print_lcno(lexer) << std::endl;
      break;
    case Token::ERROR:
      fout << "(ERROR)" << print_lcno(lexer) << std::endl;
      break;
    default:
      assert(false && "unknown token");
      break;
    }
  } while (token != Token::ERROR && token != Token::END);

  fout.close();

  return token == Token::END ? 0 : -1;
}

int main(int argc, char const *argv[])
{
  using namespace sysyc::frontend;
  using namespace sysyc::misc;

  if (!sysyc::test::check_argc(argc, 3))
    return EXIT_FAILURE;

  int rc = -1;

  std::fstream fin;
  fin.open(argv[1], std::ios_base::in);

  Logger logger;
  Lexer lexer(fin, logger);

  rc = test_lexer(lexer, argv[2]);

  fin.close();

  if (rc != 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
