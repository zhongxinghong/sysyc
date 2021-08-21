//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: codegen.cc
//  Created Date: 2021-05-25
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "middle/quad/register.h"
#include "backend/tigger/codegen.h"

namespace sysyc
{
namespace backend
{
namespace tigger
{
using namespace middle::quad;

std::ostream &TiggerCodeGenerator::add_indent()
{
  auto &os = *p_os;

  for (std::size_t i = 0; i < indent; ++i)
    os << ' ';
  return os;
}

std::string TiggerCodeGenerator::addr2str(const Operand *addr)
{
  assert(addr != nullptr);

  switch (addr->type)
  {
  case Operand::Type::NUM:
    return std::to_string(addr->val);

  case Operand::Type::ADDR:
  {
    assert(addr->val >= 0);
    auto uaddr = static_cast<addr_t>(addr->val);
    auto id = addrinfo::erase_type(uaddr);

    switch (addrinfo::get_type(uaddr))
    {
    case AddressType::T:
    case AddressType::t:
    case AddressType::p:
      assert(false);
      break;

    case AddressType::v:
      return "v" + std::to_string(id);

    case AddressType::f:
    {
      auto it = p_instseq->funcaddr_map.find(uaddr);
      assert(it != p_instseq->funcaddr_map.end());
      return it->second;
    }

    default:
      return std::to_string(static_cast<Register>(uaddr));
    }
  }
  default:
    assert(false);
  }
  return "";
}

std::string TiggerCodeGenerator::num2str(const Operand *addr)
{
  assert(addr && addr->is_num());
  return std::to_string(addr->val);
}

std::string TiggerCodeGenerator::label2str(const Operand *addr)
{
  assert(addr && addr->is_addr());
  return "l" + std::to_string(addr->val);
}

std::string TiggerCodeGenerator::op2str(const Opcode &op)
{
  switch (op)
  {
  case Opcode::LNOT:  return "!";
  case Opcode::NEG:   return "-";
  case Opcode::NE:    return "!=";
  case Opcode::EQ:    return "==";
  case Opcode::GT:    return ">";
  case Opcode::LT:    return "<";
  case Opcode::GE:    return ">=";
  case Opcode::LE:    return "<=";
  case Opcode::LAND:  return "&&";
  case Opcode::LOR:   return "||";
  case Opcode::ADD:   return "+";
  case Opcode::SUB:   return "-";
  case Opcode::MUL:   return "*";
  case Opcode::DIV:   return "/";
  case Opcode::MOD:   return "%";
  case Opcode::JNE:   return "!=";
  case Opcode::JEQ:   return "==";
  case Opcode::JGT:   return ">";
  case Opcode::JLT:   return "<";
  case Opcode::JGE:   return ">=";
  case Opcode::JLE:   return "<=";
  case Opcode::JEQZ:  return "==";
  case Opcode::JNEZ:  return "!=";
  default:
    assert(false);
  }
  return "";
}

bool TiggerCodeGenerator::generate(const InstructionList &instseq, std::ostream &os)
{
  indent = 0;
  p_instseq = &instseq;
  p_os = &os;

  for (const auto &inst : instseq.list)
  {
    switch (inst->op)
    {
    case Opcode::LNOT:
    case Opcode::NEG:
    {
      add_indent()
        << addr2str(inst->dest)
        << " = "
        << op2str(inst->op)
        << addr2str(inst->arg1)
        << std::endl;
      break;
    }
    case Opcode::NE:
    case Opcode::EQ:
    case Opcode::GT:
    case Opcode::LT:
    case Opcode::GE:
    case Opcode::LE:
    case Opcode::LAND:
    case Opcode::LOR:
    case Opcode::ADD:
    case Opcode::SUB:
    case Opcode::MUL:
    case Opcode::DIV:
    case Opcode::MOD:
    {
      add_indent()
        << addr2str(inst->dest)
        << " = "
        << addr2str(inst->arg1)
        << ' '
        << op2str(inst->op)
        << ' '
        << addr2str(inst->arg2)
        << std::endl;
      break;
    }
    case Opcode::JNE:
    case Opcode::JEQ:
    case Opcode::JGT:
    case Opcode::JLT:
    case Opcode::JGE:
    case Opcode::JLE:
    {
      add_indent()
        << "if "
        << addr2str(inst->arg1)
        << ' '
        << op2str(inst->op)
        << ' '
        << addr2str(inst->arg2)
        << " goto "
        << label2str(inst->dest)
        << std::endl;
      break;
    }
    case Opcode::JEQZ:
    case Opcode::JNEZ:
    {
      add_indent()
        << "if "
        << addr2str(inst->arg1)
        << ' '
        << op2str(inst->op)
        << " x0 goto "
        << label2str(inst->dest)
        << std::endl;
      break;
    }
    case Opcode::JMP:
    {
      add_indent()
        << "goto "
        << label2str(inst->dest)
        << std::endl;
      break;
    }
    case Opcode::SWORD:
    {
      add_indent()
        << addr2str(inst->dest)
        << (!inst->arg2 ? "" : " [" + num2str(inst->arg2)  + "]")
        << " = "
        << addr2str(inst->arg1)
        << std::endl;
      break;
    }
    case Opcode::LWORD:
    {
      add_indent()
        << addr2str(inst->dest)
        << " = "
        << addr2str(inst->arg1)
        << " ["
        << num2str(inst->arg2)
        << ']'
        << std::endl;
      break;
    }
    case Opcode::VDEF:
    {
      add_indent()
        << addr2str(inst->dest)
        << " = "
        << (!inst->arg1 ? "0" : "malloc " + num2str(inst->arg1))
        << std::endl;
      break;
    }
    case Opcode::FDEF:
    {
      indent += tabsize;
      os << "f_"
        << addr2str(inst->dest)
        << " ["
        << num2str(inst->arg1)
        << "] ["
        << num2str(inst->arg2)
        << ']'
        << std::endl;
      break;
    }
    case Opcode::FEND:
    {
      assert(indent >= tabsize);
      indent -= tabsize;
      os << "end f_"
        << addr2str(inst->dest)
        << std::endl;
      break;
    }
    case Opcode::LABEL:
    {
      os << label2str(inst->arg1)
        << ':'
        << std::endl;
      break;
    }
    case Opcode::CALL:
    {
      add_indent()
        << "call f_"
        << addr2str(inst->arg1)
        << std::endl;
      break;
    }
    case Opcode::RET:
    {
      add_indent()
        << "return"
        << std::endl;
      break;
    }
    case Opcode::STORE:
    {
      add_indent()
        << "store "
        << addr2str(inst->arg1)
        << ' '
        << num2str(inst->dest)
        << std::endl;
      break;
    }
    case Opcode::LOAD:
    {
      add_indent()
        << "load "
        << addr2str(inst->arg1)
        << ' '
        << addr2str(inst->dest)
        << std::endl;
      break;
    }
    case Opcode::LOADADDR:
    {
      add_indent()
        << "loadaddr "
        << addr2str(inst->arg1)
        << ' '
        << addr2str(inst->dest)
        << std::endl;
      break;
    }
    default:
      assert(false);
    }
  }
  return true;
}

} // namespace tigger

} // namespace backend

} // namespace sysyc
