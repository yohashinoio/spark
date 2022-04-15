/**
 * common.hpp
 *
 * These codes are licensed under Apache-2.0 License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#ifndef _4e85d9d0_bc81_11ec_8422_0242ac120002
#define _4e85d9d0_bc81_11ec_8422_0242ac120002

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <pch/pch.hpp>

namespace maple::codegen
{

struct Variable {
  Variable(llvm::AllocaInst* pointer,
           const bool        is_mutable,
           const bool        is_signed) noexcept;

  llvm::AllocaInst* getAllocaInst() const noexcept
  {
    return pointer;
  }

  bool isMutable() const noexcept
  {
    return is_mutable;
  }

  bool isSigned() const noexcept
  {
    return is_signed;
  }

private:
  llvm::AllocaInst* pointer;
  bool              is_mutable;
  bool              is_signed;
};

struct SymbolTable {
  [[nodiscard]] std::optional<Variable>
  operator[](const std::string& name) const noexcept;

  // Regist stands for register.
  void regist(const std::string& name, Variable info)
  {
    named_values.insert({name, info});
  }

  // Returns true if the variable is already registered, false otherwise.
  bool exists(const std::string& name) const
  {
    return named_values.contains(name);
  }

private:
  std::unordered_map<std::string, Variable> named_values;
};

// Class that wraps llvm::Value.
// Made to handle signs, etc.
struct Value {
  Value(llvm::Value* value, const bool is_signed) noexcept;

  explicit Value(llvm::Value* value) noexcept;

  Value() noexcept = default;

  [[nodiscard]] llvm::Value* getValue() const noexcept
  {
    return value;
  }

  [[nodiscard]] bool isSigned() const noexcept
  {
    return is_signed;
  }

  [[nodiscard]] bool isInteger() const
  {
    return value->getType()->isIntegerTy();
  }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return value;
  }

private:
  llvm::Value* value;
  bool         is_signed;
};

// Create an alloca instruction in the entry block of
// the function.
[[nodiscard]] llvm::AllocaInst*
create_entry_block_alloca(llvm::Function*    func,
                          const std::string& var_name,
                          llvm::Type*        type);

} // namespace maple::codegen

#endif
