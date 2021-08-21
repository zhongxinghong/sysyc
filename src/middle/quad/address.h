//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: address.h
//  Created Date: 2021-05-25
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_MIDDLE_QUAD_ADDRESS_H__
#define __SYSYC_MIDDLE_QUAD_ADDRESS_H__

namespace sysyc
{
namespace middle
{
namespace quad
{
using addr_t = std::size_t;
using label_t = std::size_t;

enum class AddressType : addr_t
{
  r = 0,    // REG
  T, p, t,  // SYMBOL
  f,        // FUNCTION
  v,        // VARIABLE
};

namespace addrinfo
{
inline AddressType get_type(const addr_t &addr)
{
  return static_cast<AddressType>((addr & 0x70000) >> 16);
}

inline addr_t embed_type(const addr_t &addr, const AddressType &type)
{
  return addr | (static_cast<addr_t>(type) << 16);
}

inline addr_t erase_type(const addr_t &addr)
{
  return addr & 0xffff;
}

inline bool is_r(const addr_t &addr)
{
  return get_type(addr) == AddressType::r;
}

inline bool is_T(const addr_t &addr)
{
  return get_type(addr) == AddressType::T;
}

inline bool is_p(const addr_t &addr)
{
  return get_type(addr) == AddressType::p;
}

inline bool is_t(const addr_t &addr)
{
  return get_type(addr) == AddressType::t;
}

inline bool is_f(const addr_t &addr)
{
  return get_type(addr) == AddressType::f;
}

inline bool is_v(const addr_t &addr)
{
  return get_type(addr) == AddressType::v;
}

} // namespace addrinfo

} // namespace quad

} // namespace middle

} // namespace sysyc

#endif // __SYSYC_MIDDLE_QUAD_ADDRESS_H__
