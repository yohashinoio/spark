/**
 * These codes are licensed under Apache-2.0 License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#ifndef _18de40e3_5ff0_4b6c_833a_5f79d5726fcc
#define _18de40e3_5ff0_4b6c_833a_5f79d5726fcc

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <maple/pch/pch.hpp>
#include <maple/ast/ast.hpp>

namespace maple
{

namespace codegen
{

struct NamespaceHierarchy;
struct CGContext;
struct Value;

} // namespace codegen

namespace mangle
{

struct Mangler : private boost::noncopyable {
  [[nodiscard]] std::string mangleFunction(codegen::CGContext&      ctx,
                                           const ast::FunctionDecl& ast) const;

  [[nodiscard]] std::string
  mangleFunctionCall(codegen::CGContext&               ctx,
                     const std::string_view            callee,
                     const std::deque<codegen::Value>& args) const;

  [[nodiscard]] std::string mangleMemberFunctionCall(
    codegen::CGContext&    ctx,
    const std::string_view callee,
    const std::string_view type_name_of_this, // If the function is in a Sample
                                              // structure, set "Sample".
    const std::deque<codegen::Value>& args) const;

private:
  [[nodiscard]] std::string mangleFunctionName(const std::string& name) const;

  [[nodiscard]] std::string
  mangleNamespace(const codegen::NamespaceHierarchy& namespaces) const;
};

} // namespace mangle

} // namespace maple

#endif
