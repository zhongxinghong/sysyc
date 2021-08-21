//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: lexer.h
//  Created Date: 2021-04-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_FRONTEND_LEXER_H__
#define __SYSYC_FRONTEND_LEXER_H__

#include <vector>

#include "misc/logger.h"
#include "frontend/token.h"

namespace sysyc
{
namespace frontend
{
class Lexer
{
private:
  struct tokval_s
  {
    std::string id;
    Keyword kw;
    sysystd::int_t int_;
    Operator op;
    char sep;
  };

  using tokval_t = tokval_s;

public:
  Lexer(std::istream &in, misc::Logger &logger);

  const decltype(tokval_t::id) &id_val() const { return buf[ix - 1].val.id; }
  const decltype(tokval_t::kw) &kw_val() const { return buf[ix - 1].val.kw; }
  const decltype(tokval_t::int_) &int_val() const { return buf[ix - 1].val.int_; }
  const decltype(tokval_t::op) &op_val() const { return buf[ix - 1].val.op; }
  const decltype(tokval_t::sep) &sep_val() const { return buf[ix - 1].val.sep; }

  const std::size_t &get_lineno() const { return buf[ix - 1].lineno; }
  const std::size_t &get_colno() const { return buf[ix - 1].colno; }

  Token next_token();

  Token peek_nth_token(std::size_t nth);

  const decltype(tokval_t::id) &peek_nth_id_val(std::size_t nth)
    { return peek_nth_tokval(nth).id; }

  const decltype(tokval_t::kw) &peek_nth_kw_val(std::size_t nth)
    { return peek_nth_tokval(nth).kw; }

  const decltype(tokval_t::int_) &peek_nth_int_val(std::size_t nth)
    { return peek_nth_tokval(nth).int_; }

  const decltype(tokval_t::op) &peek_nth_op_val(std::size_t nth)
    { return peek_nth_tokval(nth).op; }

  const decltype(tokval_t::sep) &peek_nth_sep_val(std::size_t nth)
    { return peek_nth_tokval(nth).sep; }

private:
  bool next_char();

  bool is_eof() const;
  bool is_eol() const;

  Token handle_integer();
  Token handle_id_or_keyword();
  Token handle_operator();
  Token handle_comment();
  Token handle_separator();
  Token handle_unknown();

  Token handle_next();
  Token handle_error(const std::string &errmsg);

  void prefetch_n_token(std::size_t n);

  const tokval_t &peek_nth_tokval(std::size_t nth);

private:
  struct token_s
  {
    Token token;
    tokval_t val;
    std::size_t lineno;
    std::size_t colno;
  };

  std::istream &in;
  misc::Logger &logger;
  char last_char;
  bool next_char_line_break;

  tokval_t tokval;
  std::size_t lineno;
  std::size_t colno;

  std::vector<token_s> buf;
  std::size_t ix;
};

} // namespace frontend

} // namespace sysyc

#endif // __SYSYC_FRONTEND_LEXER_H__
