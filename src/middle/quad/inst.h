//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: inst.h
//  Created Date: 2021-05-17
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_MIDDLE_QUAD_INST_H__
#define __SYSYC_MIDDLE_QUAD_INST_H__

#include "frontend/sysystd.h"
#include "middle/quad/opcode.h"
#include "middle/quad/register.h"

namespace sysyc
{
namespace middle
{
namespace quad
{
struct Operand
{
  enum class Type { NUM, ADDR };

  Type type;
  int val;

  Operand(Type type, int val) : type(type), val(val) {}
  Operand(const Operand &opnd) : Operand(opnd.type, opnd.val) {}
  Operand(const Operand *opnd) : Operand(*opnd) {}
  Operand(const Register &reg) : Operand(Type::ADDR, static_cast<addr_t>(reg)) {}

  bool is_num() const { return type == Type::NUM; }
  bool is_addr() const { return type == Type::ADDR; }
};

struct Instruction
{
  Opcode op;
  Operand *dest;
  Operand *arg1;
  Operand *arg2;

  Instruction(Opcode op, Operand *dest, Operand *arg1, Operand *arg2)
    : op(op), dest(dest), arg1(arg1), arg2(arg2) {}

  Instruction(const Instruction &inst)
    : Instruction(inst.op, new Operand(inst.dest), new Operand(inst.arg1),
                  new Operand(inst.arg2)) {}

  Instruction(const Instruction *inst) : Instruction(*inst) {}

  ~Instruction() { delete dest; delete arg1; delete arg2; }

  static Instruction *new_label(std::size_t label);
  static Instruction *new_jeqz(Operand *addr, std::size_t label);
  static Instruction *new_jnez(Operand *addr, std::size_t label);
  static Instruction *new_jmp(std::size_t label);
  static Instruction *new_ret(Operand *addr);
  static Instruction *new_call(Operand *dest, std::size_t func);
  static Instruction *new_param(Operand *addr);
  static Instruction *new_vdef(std::size_t addr, Operand *size);
  static Instruction *new_fdef(std::size_t func);
  static Instruction *new_fend(std::size_t func);
};

} // namespace quad

} // namespace middle

} // namespace sysyc

#endif // __SYSYC_MIDDLE_QUAD_INST_H__
