/**
 * ast.hxx
 *
 * These codes are licensed under Apache-2.0 License.
 * See the LICENSE for details.
 *
 * Copyright (c) 2022 Hiramoto Ittou.
 */

#ifndef _1d3d3a84_9536_11ec_b909_0242ac120002
#define _1d3d3a84_9536_11ec_b909_0242ac120002

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <pch/pch.hxx>
#include <parse/id.hxx>

namespace x3 = boost::spirit::x3;

namespace miko
{

//===----------------------------------------------------------------------===//
// Abstract syntax tree
//===----------------------------------------------------------------------===//

namespace ast
{

struct type_info {
  bool          is_ptr;
  id::type_name id;
};

struct nil {};

//===----------------------------------------------------------------------===//
// Expression abstract syntax tree
//===----------------------------------------------------------------------===//

struct string_literal : x3::position_tagged {
  std::string str;

  explicit string_literal(const std::string& str);

  string_literal() noexcept;
};

struct variable_expr : x3::position_tagged {
  std::string name;

  explicit variable_expr(const std::string& name);

  variable_expr() noexcept;
};

struct unary_op_expr;
struct binary_op_expr;
struct function_call_expr;
struct cast_expr;
struct address_of_expr;

using expression = boost::variant<nil,
                                  std::uint32_t, /* unsigned integer literals */
                                  std::int32_t,  /* signed integer literals */
                                  bool,          /* boolean literals */
                                  string_literal,
                                  variable_expr,
                                  boost::recursive_wrapper<unary_op_expr>,
                                  boost::recursive_wrapper<binary_op_expr>,
                                  boost::recursive_wrapper<function_call_expr>,
                                  boost::recursive_wrapper<cast_expr>,
                                  boost::recursive_wrapper<address_of_expr>>;

struct unary_op_expr : x3::position_tagged {
  std::string op;
  expression  rhs;

  unary_op_expr(const std::string& op, const expression& rhs);

  unary_op_expr() noexcept;
};

struct binary_op_expr : x3::position_tagged {
  expression  lhs;
  std::string op;
  expression  rhs;

  binary_op_expr(const expression&  lhs,
                 const std::string& op,
                 const expression&  rhs);

  binary_op_expr() noexcept;
};

struct function_call_expr : x3::position_tagged {
  std::string             callee;
  std::vector<expression> args;

  function_call_expr(const std::string&             callee,
                     const std::vector<expression>& args);

  function_call_expr() noexcept;
};

struct cast_expr : x3::position_tagged {
  expression lhs;
  type_info  as;

  cast_expr(const expression& lhs, const type_info& as);

  cast_expr() noexcept;
};

struct address_of_expr : x3::position_tagged {
  expression lhs;

  address_of_expr(const expression& lhs);

  address_of_expr() noexcept;
};

//===----------------------------------------------------------------------===//
// Statement abstract syntax tree
//===----------------------------------------------------------------------===//

struct return_statement : x3::position_tagged {
  std::optional<expression> rhs;

  explicit return_statement(const std::optional<expression>& rhs);

  return_statement() noexcept;
};

struct variable_def_statement : x3::position_tagged {
  std::optional<id::variable_qualifier> qualifier;
  std::string                           name;
  type_info                             type;
  std::optional<expression>             initializer;

  variable_def_statement(const std::optional<id::variable_qualifier>& qualifier,
                         const std::string&                           name,
                         const type_info&                             type,
                         const std::optional<expression>& initializer);

  variable_def_statement() noexcept;
};

struct break_statement : x3::position_tagged {
  std::string tmp;
};

struct continue_statement : x3::position_tagged {
  std::string tmp;
};

struct if_statement;
struct loop_statement;
struct while_statement;
struct for_statement;

using statement = boost::make_recursive_variant<
  nil,
  std::vector<boost::recursive_variant_>, // compound statement
  expression,
  return_statement,
  variable_def_statement,
  break_statement,
  continue_statement,
  boost::recursive_wrapper<if_statement>,
  boost::recursive_wrapper<loop_statement>,
  boost::recursive_wrapper<while_statement>,
  boost::recursive_wrapper<for_statement>>::type;

using compound_statement = std::vector<statement>;

struct if_statement : x3::position_tagged {
  expression               condition;
  statement                then_statement;
  std::optional<statement> else_statement;

  if_statement(const expression&               condition,
               const statement&                then_statement,
               const std::optional<statement>& else_statement);

  if_statement() noexcept;
};

struct loop_statement : x3::position_tagged {
  std::string tmp;
  statement   body;

  explicit loop_statement(const std::string& tmp, const statement& body);

  loop_statement() noexcept;
};

struct while_statement : x3::position_tagged {
  expression cond_expr;
  statement  body;

  while_statement(const expression& cond_expr, const statement& body);

  while_statement() noexcept;
};

struct for_statement : x3::position_tagged {
  std::optional<expression> init_expr;
  std::optional<expression> cond_expr;
  std::optional<expression> loop_expr;
  statement                 body;

  for_statement(const std::optional<expression>& init_expr,
                const std::optional<expression>& cond_expr,
                const std::optional<expression>& loop_expr,
                const statement&                 body);

  for_statement() noexcept;
};

//===----------------------------------------------------------------------===//
// Top level abstract syntax tree
//===----------------------------------------------------------------------===//

struct parameter : x3::position_tagged {
  std::optional<id::variable_qualifier> qualifier;
  std::string                           name;
  type_info                             type;
  bool                                  is_vararg;

  parameter(const std::optional<id::variable_qualifier>& qualifier,
            const std::string&                           name,
            const type_info&                             type,
            bool                                         is_vararg);

  parameter() noexcept;
};

struct parameter_list : x3::position_tagged {
  std::vector<parameter> params;

  explicit parameter_list(const std::vector<parameter>& params);

  parameter_list() noexcept;

  const parameter& operator[](const std::size_t idx) const;

  const std::vector<parameter>& operator*() const noexcept;

  std::size_t length() const noexcept;
};

struct function_declare : x3::position_tagged {
  std::optional<id::function_linkage> linkage;
  std::string                         name;
  parameter_list                      params;
  type_info                           return_type;

  function_declare(const std::optional<id::function_linkage>& linkage,
                   const std::string&                         name,
                   const parameter_list&                      params,
                   const type_info&                           return_type);

  function_declare() noexcept;
};

struct function_define : x3::position_tagged {
  function_declare decl;
  statement        body;

  function_define(const function_declare& decl, const statement& body);

  function_define() noexcept;
};

using top_level_stmt = boost::variant<nil, function_declare, function_define>;

using program = std::vector<top_level_stmt>;

} // namespace ast
} // namespace miko

#endif
