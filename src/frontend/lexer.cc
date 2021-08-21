//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: lexer.cc
//  Created Date: 2021-04-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cctype>
#include <sstream>
#include <unordered_set>
#include <cassert>

#include "frontend/lexer.h"

#define __sysyc_op_e2_2_c0(a1, a2)    ((a2)[0]),
#define __sysyc_op_e2_2_call(a1, a2)  (sizeof(a2) > 1 ? (a2)[0] : (a2)[0]), \
                                      (sizeof(a2) > 2 ? (a2)[1] : (a2)[0]), \
                                      (sizeof(a2) > 3 ? (a2)[2] : (a2)[0]),

namespace sysyc
{
namespace frontend
{
namespace __impl
{
inline bool is_op_first_char(const char &c)
{
  static const std::unordered_set<char> table = \
    { SYSYC_SYSY_OPERATOR_TABLE(__sysyc_op_e2_2_c0) };

  return table.find(c) != table.end();
}

inline bool is_op_char(const char &c)
{
  static const std::unordered_set<char> table = \
    { SYSYC_SYSY_OPERATOR_TABLE(__sysyc_op_e2_2_call) };

  return table.find(c) != table.end();
}

inline bool is_sep_char(const char &c)
{
  static const std::string table = SYSYC_SYSY_SEPARATOR;

  for (const char &sep : table)
  {
    if (c == sep)
      return true;
  }
  return false;
}

inline bool isodigit(const char &c)
{
  return c >= '0' && c <= '7';
}

inline int xdigit2int(const char &c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  assert(false);
  return -1;
}

} // namespace __impl


#undef __sysyc_op_e2_2_c0
#undef __sysyc_op_e2_2_call

Lexer::Lexer(std::istream &in, misc::Logger &logger) :
  in(in), logger(logger), last_char(' '), next_char_line_break(false),
  lineno(1), colno(0), ix(0)
{
  in >> std::noskipws;
}

inline bool Lexer::is_eof() const
{
  return !in || in.eof();
}

inline bool Lexer::is_eol() const
{
  return is_eof() || last_char == '\r' || last_char == '\n';
}

bool Lexer::next_char()
{
  if (is_eof())
    return false;

  if (next_char_line_break)
  {
    next_char_line_break = false;
    lineno++;
    colno = 0;
  }

  in >> last_char;
  colno++;

  if (last_char == '\n')
    next_char_line_break = true;

  return true;
}

Token Lexer::handle_error(const std::string &errmsg)
{
  std::stringstream sst;
  sst << "[Lexer] At Line " << lineno << ", Col " << colno << ": " << errmsg;
  logger.error(sst.str());
  return Token::ERROR;
}

Token Lexer::handle_integer()
{
  if (last_char == '0')
  {
    if (!next_char())
    {
      tokval.int_ = 0;
    }
    else if (last_char == 'x' || last_char == 'X')
    {
      if (!next_char() || !std::isxdigit(last_char))
        return handle_error("expect [0-9a-fA-F] after '0x' or '0X'");

      tokval.int_ = __impl::xdigit2int(last_char);

      while (next_char() && std::isxdigit(last_char))
        tokval.int_ = (tokval.int_ << 4) + __impl::xdigit2int(last_char);
    }
    else if (__impl::isodigit(last_char))
    {
      tokval.int_ = (last_char - '0');

      while (next_char() && __impl::isodigit(last_char))
        tokval.int_ = (tokval.int_ << 3) + (last_char - '0');
    }
    else if (std::isdigit(last_char))
    {
      return handle_error("expect [0-7] after first '0'");
    }
    else
    {
      tokval.int_ = 0;
    }
  }
  else if (std::isdigit(last_char))
  {
    tokval.int_ = (last_char - '0');

    while (next_char() && std::isdigit(last_char))
      tokval.int_ = tokval.int_ * 10 + (last_char - '0');
  }
  else
  {
    assert(false);
  }
  return Token::INTEGER;
}

Token Lexer::handle_id_or_keyword()
{
  tokval.id = std::string(1, last_char);

  while (next_char() && (std::isalnum(last_char) || last_char == '_'))
    tokval.id.push_back(last_char);

  if (__impl::kw_str2enum(tokval.id, &tokval.kw))
    return Token::KEYWORD;

  return Token::ID;
}

Token Lexer::handle_operator()
{
  std::string sbuf = std::string(1, last_char);

  while (next_char() && __impl::is_op_char(last_char))
  {
    sbuf.push_back(last_char);

    if (sbuf == "//" || sbuf == "/*")
      return handle_comment();

    if (!__impl::op_str2enum(sbuf, &tokval.op))
    {
      sbuf.pop_back();
      break;
    }
  }

  if (!__impl::op_str2enum(sbuf, &tokval.op))
    return handle_error("unknown operator '" + sbuf + "'");

  return Token::OPERATOR;
}

Token Lexer::handle_comment()
{
  if (last_char == '/')
  {
    while (next_char() && !is_eol())
      ;
  }
  else if (last_char == '*')
  {
    int state = 0;

    while (next_char() && state != 2)
    {
      if (state == 0 && last_char == '*')
      {
        state = 1;
      }
      else if (state == 1)
      {
        if (last_char == '/')
          state = 2;
        else if (last_char != '*')
          state = 0;
      }
    }

    if (state != 2)
      return handle_error("expect \"*/\" after \"/*\"");
  }
  else
  {
    assert(false);
  }
  return next_token();
}

Token Lexer::handle_separator()
{
  tokval.sep = last_char;
  next_char();
  return Token::SEPARATOR;
}

Token Lexer::handle_unknown()
{
  return handle_error("unknown char '" + std::string(1, last_char) + "'");
}

Token Lexer::handle_next()
{
  while (std::isspace(last_char) && next_char())
    ;

  if (is_eof())
    return Token::END;

  if (std::isdigit(last_char))
    return handle_integer();

  if (std::isalnum(last_char) || last_char == '_')
    return handle_id_or_keyword();

  if (__impl::is_op_first_char(last_char))
    return handle_operator();

  if (__impl::is_sep_char(last_char))
    return handle_separator();

  return handle_unknown();
}

void Lexer::prefetch_n_token(std::size_t n)
{
  for (std::size_t i = 0; i < n; ++i)
  {
    auto token = handle_next();
    buf.push_back({token, tokval, lineno, colno});
  }
}

Token Lexer::next_token()
{
  assert(ix <= buf.size());

  if (ix == buf.size())
    prefetch_n_token(1);

  return buf[ix++].token;
}

Token Lexer::peek_nth_token(std::size_t nth)
{
  assert(nth >= 1 && ix + nth >= 2);

  if (nth == 1)   // optimize for nth == 1
    return buf[ix - 1].token;

  if (ix + nth - 1 > buf.size())
    prefetch_n_token(ix + nth - 1 - buf.size());

  return buf[ix + nth - 2].token;
}

const Lexer::tokval_t &Lexer::peek_nth_tokval(std::size_t nth)
{
  assert(nth >= 1 && ix + nth >= 2);

  if (nth == 1)   // optimize for nth == 1
    return buf[ix - 1].val;

  if (ix + nth - 1 > buf.size())
    prefetch_n_token(ix + nth - 1 - buf.size());

  return buf[ix + nth - 2].val;
}

} // namespace frontend

} // namespace sysyc
