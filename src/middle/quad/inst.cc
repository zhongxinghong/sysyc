//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: inst.cc
//  Created Date: 2021-05-19
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include "middle/quad/inst.h"

namespace sysyc
{
namespace middle
{
namespace quad
{
Instruction *Instruction::new_label(std::size_t label)
{
  return new Instruction(
    Opcode::LABEL,
    nullptr,
    new Operand(Operand::Type::ADDR, label),
    nullptr
  );
}

Instruction *Instruction::new_jeqz(Operand *addr, std::size_t label)
{
  return new Instruction(
    Opcode::JEQZ,
    new Operand(Operand::Type::ADDR, label),
    addr,
    nullptr
  );
}

Instruction *Instruction::new_jnez(Operand *addr, std::size_t label)
{
  return new Instruction(
    Opcode::JNEZ,
    new Operand(Operand::Type::ADDR, label),
    addr,
    nullptr
  );
}

Instruction *Instruction::new_jmp(std::size_t label)
{
  return new Instruction(
    Opcode::JMP,
    new Operand(Operand::Type::ADDR, label),
    nullptr,
    nullptr
  );
}

Instruction *Instruction::new_ret(Operand *addr)
{
  return new Instruction(
    Opcode::RET,
    nullptr,
    addr,
    nullptr
  );
}

Instruction *Instruction::new_call(Operand *dest, std::size_t func)
{
  return new Instruction(
    Opcode::CALL,
    dest,
    new Operand(Operand::Type::ADDR, func),
    nullptr
  );
}

Instruction *Instruction::new_param(Operand *addr)
{
  return new Instruction(
    Opcode::PARAM,
    nullptr,
    addr,
    nullptr
  );
}

Instruction *Instruction::new_vdef(std::size_t addr, Operand *size)
{
  return new Instruction(
    Opcode::VDEF,
    new Operand(Operand::Type::ADDR, addr),
    size,
    nullptr
  );
}

Instruction *Instruction::new_fdef(std::size_t func)
{
  return new Instruction(
    Opcode::FDEF,
    new Operand(Operand::Type::ADDR, func),
    nullptr,
    nullptr
  );
}

Instruction *Instruction::new_fend(std::size_t func)
{
  return new Instruction(
    Opcode::FEND,
    new Operand(Operand::Type::ADDR, func),
    nullptr,
    nullptr
  );
}

} // namespace quad

} // namespace middle

} // namespace sysyc
