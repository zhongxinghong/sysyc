//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: codegen.cc
//  Created Date: 2021-05-27
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "middle/quad/register.h"
#include "backend/riscv/codegen.h"

namespace sysyc
{
namespace backend
{
namespace riscv
{
using namespace middle::quad;

namespace __impl
{
inline bool is_num_int12(const Operand *addr)
{
  return addr->is_num() && sysystd::is_int12(addr->val);
}

inline bool is_num_int10(const Operand *addr)
{
  return addr->is_num() && sysystd::is_int10(addr->val);
}

} // namespace __impl

std::ostream &RISCVCodeGenerator::add_inst(const std::string &inst)
{
  static const std::size_t inst_max_width = 10;

  auto &os = *p_os;

  for (std::size_t i = 0; i < tabsize; ++i)
    os << ' ';

  os << inst;

  assert(inst.size() <= inst_max_width);

  for (std::size_t i = 0, n = inst_max_width - inst.size(); i < n; ++i)
    os << ' ';

  return os;
}

std::ostream &RISCVCodeGenerator::add_inst(const std::string &inst,
  const std::string &arg1)
{
  return add_inst(inst) << arg1 << std::endl;
}

std::ostream &RISCVCodeGenerator::add_inst(const std::string &inst,
  const std::string &arg1, const std::string &arg2)
{
  return add_inst(inst) << arg1 << ", " << arg2 << std::endl;
}

std::ostream &RISCVCodeGenerator::add_inst(const std::string &inst,
  const std::string &arg1, const std::string &arg2, const std::string &arg3)
{
  return add_inst(inst) << arg1 << ", " << arg2 << ", " << arg3 << std::endl;
}

std::ostream &RISCVCodeGenerator::add_label(const std::string &label)
{
  auto &os = *p_os;
  return os << label << ':' << std::endl;
}

std::string RISCVCodeGenerator::reg2str(const Operand *addr)
{
  assert(addr->is_addr());
  assert(addrinfo::is_r(addr->val));
  return std::to_string(static_cast<Register>(addr->val));
}

std::string RISCVCodeGenerator::num2str(const Operand *addr)
{
  assert(addr->is_num());
  return std::to_string(addr->val);
}

std::string RISCVCodeGenerator::regnum2str(const Operand *addr)
{
  if (addr->is_num())
  {
    if (addr->val == 0)
      return "x0";

    add_inst("li", "t6", std::to_string(addr->val));
    return "t6";
  }
  return reg2str(addr);
}

std::string RISCVCodeGenerator::label2str(const Operand *addr)
{
  assert(addr->is_addr());
  return ".l" + std::to_string(addr->val);
}

std::string RISCVCodeGenerator::addr2str(const Operand *addr)
{
  assert(addr->is_addr());
  auto uaddr = static_cast<addr_t>(addr->val);

  switch (addrinfo::get_type(uaddr))
  {
  case AddressType::v:
    return "v" + std::to_string(addrinfo::erase_type(uaddr));

  case AddressType::f:
  {
    const auto &instseq = *p_instseq;
    auto it = instseq.funcaddr_map.find(uaddr);
    assert(it != instseq.funcaddr_map.end());
    return it->second;
  }
  default:
    assert(false);
  }
  return "";
}

bool RISCVCodeGenerator::generate(const InstructionList &instseq, std::ostream &os)
{
  p_instseq = &instseq;
  p_os = &os;

  const auto &list = instseq.list;
  int STK = 0;

  const std::string t6 = "t6";
  const std::string sp = "sp";
  const std::string ra = "ra";

  for (auto it = list.begin(), e = list.end(); it != e; ++it)
  {
    const auto &inst = *it;

    switch (inst->op)
    {
    case Opcode::LNOT:
    {
      // reg1 = !reg2

      // seqz reg1, reg2

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);

      add_inst("seqz", reg1, reg2);
      break;
    }
    case Opcode::NEG:
    {
      // reg1 = -reg2

      // neg reg1, reg2

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);

      add_inst("neg", reg1, reg2);
      break;
    }
    case Opcode::NE:
    {
      // reg1 = reg2 != reg3

      // xor reg1, reg2, reg3
      // snez reg1, reg1

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("xor", reg1, reg2, reg3);
      add_inst("snez", reg1, reg1);
      break;
    }
    case Opcode::EQ:
    {
      // reg1 = reg2 == reg3

      // xor reg1, reg2, reg3
      // seqz reg1, reg1

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("xor", reg1, reg2, reg3);
      add_inst("seqz", reg1, reg1);
      break;
    }
    case Opcode::GT:
    {
      // reg1 = reg2 > reg3

      // sgt reg1, reg2, reg3

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("sgt", reg1, reg2, reg3);
      break;
    }
    case Opcode::LT:
    {
      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);

      if (__impl::is_num_int12(inst->arg2))
      {
        // reg1 = reg2 < int12

        // slti reg1, reg2, int12

        auto int12 = num2str(inst->arg2);

        add_inst("slti", reg1, reg2, int12);
      }
      else
      {
        // reg1 = reg2 < reg3

        // slt reg1, reg2, reg3

        auto reg3 = regnum2str(inst->arg2);

        add_inst("slt", reg1, reg2, reg3);
      }
      break;
    }
    case Opcode::GE:
    {
      // reg1 = reg2 >= reg3

      // slt reg1, reg2, reg3
      // seqz reg1, reg1

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("slt", reg1, reg2, reg3);
      add_inst("seqz", reg1, reg1);
      break;
    }
    case Opcode::LE:
    {
      // reg1 = reg2 <= reg3

      // sgt reg1, reg2, reg3
      // seqz reg1, reg1

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("sgt", reg1, reg2, reg3);
      add_inst("seqz", reg1, reg1);
      break;
    }
    case Opcode::LAND:
    {
      // reg1 = reg2 && reg3

      // snez reg1, reg2
      // snez t6, reg3
      // and reg1, reg1, t6

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("snez", reg1, reg2);
      add_inst("snez", t6, reg3);
      add_inst("and", reg1, reg1, t6);
      break;
    }
    case Opcode::LOR:
    {
      // reg1 = reg2 || reg3

      // or reg1, reg2, reg3
      // snez reg1, reg1

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("or", reg1, reg2, reg3);
      add_inst("snez", reg1, reg1);
      break;
    }
    case Opcode::ADD:
    {
      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);

      if (__impl::is_num_int12(inst->arg2))
      {
        // reg1 = reg2 + int12

        // addi reg1, reg2, int12

        auto int12 = num2str(inst->arg2);

        add_inst("addi", reg1, reg2, int12);
      }
      else
      {
        // reg1 = reg2 + reg3

        // add reg1, reg2, reg3

        auto reg3 = regnum2str(inst->arg2);

        add_inst("add", reg1, reg2, reg3);
      }
      break;
    }
    case Opcode::SUB:
    {
      // reg1 = reg2 - reg3

      // sub reg1, reg2, reg3

      assert(!(inst->arg1->is_num() && inst->arg2->is_num()));

      auto reg1 = reg2str(inst->dest);
      auto reg2 = regnum2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("sub", reg1, reg2, reg3);
      break;
    }
    case Opcode::MUL:
    {
      // reg1 = reg2 * reg3

      // mul reg1, reg2, reg3

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("mul", reg1, reg2, reg3);
      break;
    }
    case Opcode::DIV:
    {
      // reg1 = reg2 / reg3

      // div reg1, reg2, reg3

      assert(!(inst->arg1->is_num() && inst->arg2->is_num()));

      auto reg1 = reg2str(inst->dest);
      auto reg2 = regnum2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("div", reg1, reg2, reg3);
      break;
    }
    case Opcode::MOD:
    {
      // reg1 = reg2 % reg3

      // rem reg1, reg2, reg3

      assert(!(inst->arg1->is_num() && inst->arg2->is_num()));

      auto reg1 = reg2str(inst->dest);
      auto reg2 = regnum2str(inst->arg1);
      auto reg3 = regnum2str(inst->arg2);

      add_inst("rem", reg1, reg2, reg3);
      break;
    }
    case Opcode::SLL:
    {
      // slli rd, rs1, shamt

      assert(inst->arg1->is_addr() && __impl::is_num_int12(inst->arg2));

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = num2str(inst->arg2);

      add_inst("slli", reg1, reg2, reg3);
      break;
    }
    case Opcode::SRL:
    {
      // srli rd, rs1, shamt

      assert(inst->arg1->is_addr() && __impl::is_num_int12(inst->arg2));

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = num2str(inst->arg2);

      add_inst("srli", reg1, reg2, reg3);
      break;
    }
    case Opcode::SRA:
    {
      // srai rd, rs1, shamt

      assert(inst->arg1->is_addr() && __impl::is_num_int12(inst->arg2));

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);
      auto reg3 = num2str(inst->arg2);

      add_inst("srai", reg1, reg2, reg3);
      break;
    }
    case Opcode::AND:
    {
      assert(inst->arg1->is_addr() && inst->arg2->is_num());

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);

      if (__impl::is_num_int12(inst->arg2))
      {
        // andi rd, rs1, immediate

        auto int12 = num2str(inst->arg2);

        add_inst("addi", reg1, reg2, int12);
      }
      else
      {
        // and rd, rs1, rs2

        auto reg3 = regnum2str(inst->arg2);

        add_inst("add", reg1, reg2, reg3);
      }
      break;
    }
    case Opcode::JNE:
    {
      // if reg1 != reg2 goto label

      // bne reg1, reg2, .label

      auto reg1 = reg2str(inst->arg1);
      auto reg2 = regnum2str(inst->arg2);
      auto label = label2str(inst->dest);

      add_inst("bne", reg1, reg2, label);
      break;
    }
    case Opcode::JEQ:
    {
      // if reg1 == reg2 goto label

      // beq reg1, reg2, .label

      auto reg1 = reg2str(inst->arg1);
      auto reg2 = regnum2str(inst->arg2);
      auto label = label2str(inst->dest);

      add_inst("beq", reg1, reg2, label);
      break;
    }
    case Opcode::JGT:
    {
      // if reg1 > reg2 goto label

      // bgt reg1, reg2, .label

      auto reg1 = reg2str(inst->arg1);
      auto reg2 = regnum2str(inst->arg2);
      auto label = label2str(inst->dest);

      add_inst("bgt", reg1, reg2, label);
      break;
    }
    case Opcode::JLT:
    {
      // if reg1 < reg2 goto label

      // blt reg1, reg2, .label

      auto reg1 = reg2str(inst->arg1);
      auto reg2 = regnum2str(inst->arg2);
      auto label = label2str(inst->dest);

      add_inst("blt", reg1, reg2, label);
      break;
    }
    case Opcode::JGE:
    {
      // if reg1 >= reg2 goto label

      // bge reg1, reg2, .label

      auto reg1 = reg2str(inst->arg1);
      auto reg2 = regnum2str(inst->arg2);
      auto label = label2str(inst->dest);

      add_inst("bge", reg1, reg2, label);
      break;
    }
    case Opcode::JLE:
    {
      // if reg1 <= reg2 goto label

      // ble reg1, reg2, .label

      auto reg1 = reg2str(inst->arg1);
      auto reg2 = regnum2str(inst->arg2);
      auto label = label2str(inst->dest);

      add_inst("ble", reg1, reg2, label);
      break;
    }
    case Opcode::JNEZ:
    {
      // if reg1 != x0 goto label

      // bnez reg1, .label

      auto reg1 = reg2str(inst->arg1);
      auto label = label2str(inst->dest);

      add_inst("bnez", reg1, label);
      break;
    }
    case Opcode::JEQZ:
    {
      // if reg1 == x0 goto label

      // beqz reg1, .label

      auto reg1 = reg2str(inst->arg1);
      auto label = label2str(inst->dest);

      add_inst("beqz", reg1, label);
      break;
    }
    case Opcode::JMP:
    {
      // goto label

      // j .label

      auto label = label2str(inst->dest);

      add_inst("j", label);
      break;
    }
    case Opcode::SWORD:
    {
      auto reg1 = reg2str(inst->dest);

      if (inst->arg2 == nullptr)
      {
        if (inst->arg1->is_num())
        {
          // reg = int

          // li reg, int

          auto int32 = num2str(inst->arg1);

          add_inst("li", reg1, int32);
        }
        else
        {
          // reg1 = reg2

          // mv reg1, reg2

          auto reg2 = reg2str(inst->arg1);

          add_inst("mv", reg1, reg2);
        }
      }
      else
      {
        // reg1[int12] = reg2

        assert(inst->arg1->is_addr());
        assert(inst->arg2->is_num());

        auto reg2 = reg2str(inst->arg1);

        if (sysystd::is_int12(inst->arg2->val))
        {
          // sw reg2, int12(reg1)

          auto int12 = num2str(inst->arg2);

          add_inst("sw", reg2, int12 + "(" + reg1 + ")");
        }
        else
        {
          // li t6, int
          // add t6, t6, reg1
          // sw reg2, 0(t6)

          auto int32 = num2str(inst->arg2);

          add_inst("li", t6, int32);
          add_inst("add", t6, t6, reg1);
          add_inst("sw", reg2, "0(" + t6 + ")");
        }
      }
      break;
    }
    case Opcode::LWORD:
    {
      // reg1 = reg2[int12]

      auto reg1 = reg2str(inst->dest);
      auto reg2 = reg2str(inst->arg1);

      assert(inst->arg2->is_num());

      if (sysystd::is_int12(inst->arg2->val))
      {
        // lw reg1, int12(reg2)

        auto int12 = num2str(inst->arg2);

        add_inst("lw", reg1, int12 + "(" + reg2 + ")");
      }
      else
      {
        // li t6, int
        // add t6, t6, reg2
        // lw reg1, 0(t6)

        auto int32 = num2str(inst->arg2);

        add_inst("li", t6, int32);
        add_inst("add", t6, t6, reg2);
        add_inst("lw", reg1, "0(" + t6 + ")");
      }
      break;
    }
    case Opcode::VDEF:
    {
      assert(addrinfo::is_v(inst->dest->val));

      if (inst->arg1 == nullptr)
      {
        //   .global   global_var
        //   .section  .sdata
        //   .align    2
        //   .type     global_var, @object
        //   .size     global_var, 4
        // global_var:
        //   .word     int

        auto global_var = addr2str(inst->dest);

        add_inst(".global", global_var);
        add_inst(".section", ".sdata");
        add_inst(".align", "2");
        add_inst(".type", global_var, "@object");
        add_inst(".size", global_var, "4");
        add_label(global_var);
        add_inst(".word", "0");
      }
      else
      {
        // .comm global_var, int, 4

        auto global_var = addr2str(inst->dest);
        auto int32 = num2str(inst->arg1);

        add_inst(".comm", global_var, int32, "4");
      }
      break;
    }
    case Opcode::FDEF:
    {
      //   .text
      //   .align  2
      //   .global func
      //   .type   func, @function
      // func:
      //   addi    sp, sp, -STK
      //   sw      ra, STK-4(sp)

      auto func = addr2str(inst->dest);
      // auto int1 = inst->arg1->val;
      auto int2 = inst->arg2->val;

      STK = (int2 / 4 + 1) * 16;

      add_inst(".text") << std::endl;
      add_inst(".align", "2");
      add_inst(".global", func);
      add_inst(".type", func, "@function");
      add_label(func);

      if (sysystd::is_int12(-STK))
      {
        add_inst("addi", sp, sp, std::to_string(-STK));
      }
      else
      {
        // li t6, -STK
        // add sp, sp, t6

        add_inst("li", t6, std::to_string(-STK));
        add_inst("add", sp, sp, t6);
      }

      if (sysystd::is_int12(STK - 4))
      {
        // sw      ra, STK-4(sp)

        add_inst("sw", ra, std::to_string(STK - 4) + "(" + sp + ")");
      }
      else
      {
        // li      t6, STK-4
        // add     t6, t6, sp
        // sw      ra, 0(t6)

        add_inst("li", t6, std::to_string(STK - 4));
        add_inst("add", t6, t6, sp);
        add_inst("sw", ra, "0(" + t6 + ")");
      }
      break;
    }
    case Opcode::FEND:
    {
      //   .size   func, .-func

      auto func = addr2str(inst->dest);

      add_inst(".size", func, ".-" + func);
      break;
    }
    case Opcode::LABEL:
    {
      // label:

      // .label:

      auto label = label2str(inst->arg1);

      add_label(label);
      break;
    }
    case Opcode::CALL:
    {
      // call f_func

      // call func

      auto func = addr2str(inst->arg1);

      add_inst("call", func);
      break;
    }
    case Opcode::RET:
    {
      // return

      // lw ra, STK-4(sp)
      // addi sp, sp, STK
      // ret

      if (sysystd::is_int12(STK - 4))
      {
        add_inst("lw", ra, std::to_string(STK - 4) + "(" + sp + ")");
      }
      else
      {
        // li t6, STK-4
        // add t6, t6, sp
        // lw ra, 0(t6)

        add_inst("li", t6, std::to_string(STK - 4));
        add_inst("add", t6, t6, sp);
        add_inst("lw", ra, "0(" + t6 + ")");
      }

      if (sysystd::is_int12(STK))
      {
        add_inst("addi", sp, sp, std::to_string(STK));
      }
      else
      {
        // li t6, STK
        // add sp, sp, t6

        add_inst("li", t6, std::to_string(STK));
        add_inst("add", sp, sp, t6);
      }

      add_inst("ret") << std::endl;
      break;
    }
    case Opcode::STORE:
    {
      // store reg int10

      assert(inst->dest->is_num());

      auto reg = reg2str(inst->arg1);

      if (sysystd::is_int10(inst->dest->val))
      {
        // sw reg, int10*4(sp)

        auto int10 = inst->dest->val;

        add_inst("sw", reg, std::to_string(int10 * 4) + "(" + sp + ")");
      }
      else
      {
        // sw reg, int*4(sp)

        // li t6, int*4
        // add t6, t6, sp
        // sw reg, 0(t6)

        auto int32 = inst->dest->val;

        add_inst("li", t6, std::to_string(int32 * 4));
        add_inst("add", t6, t6, sp);
        add_inst("sw", reg, "0(" + t6 + ")");
        break;
      }
      break;
    }
    case Opcode::LOAD:
    {
      auto reg = reg2str(inst->dest);

      if (inst->arg1->is_addr())
      {
        // load global_var reg

        // lui reg, %hi(global_var)
        // lw reg, %lo(global_var)(reg)

        auto global_var = addr2str(inst->arg1);

        add_inst("lui", reg, "%hi(" + global_var + ")");
        add_inst("lw", reg, "%lo(" + global_var + ")(" + reg + ")");
      }
      else if (sysystd::is_int10(inst->arg1->val))
      {
        // load int10 reg

        // lw reg, int10*4(sp)

        auto int10 = inst->arg1->val;

        add_inst("lw", reg, std::to_string(int10 * 4) + "(" + sp + ")");
      }
      else
      {
        // load int reg

        // li t6, int*4
        // add t6, t6, sp
        // lw reg, 0(t6)

        auto int32 = inst->arg1->val;

        add_inst("li", t6, std::to_string(int32 * 4));
        add_inst("add", t6, t6, sp);
        add_inst("lw", reg, "0(" + t6 + ")");
      }
      break;
    }
    case Opcode::LOADADDR:
    {
      auto reg = reg2str(inst->dest);

      if (inst->arg1->is_addr())
      {
        // loadaddr global_var reg

        // la reg, global_var

        auto global_var = addr2str(inst->arg1);

        add_inst("la", reg, global_var);
      }
      else if (sysystd::is_int10(inst->arg1->val))
      {
        // loadaddr int10 reg

        // addi reg, sp, int10*4

        auto int10 = inst->arg1->val;

        add_inst("addi", reg, sp, std::to_string(int10 * 4));
      }
      else
      {
        // loadaddr int reg

        // li t6, int*4
        // addi reg, sp, t6

        auto int32 = inst->arg1->val;

        add_inst("li", t6, std::to_string(int32 * 4));
        add_inst("add", reg, sp, t6);
      }
      break;
    }
    default:
      assert(false);
    }
  }

  return true;
}

} // namespace riscv

} // namespace backend

} // namespace sysyc
