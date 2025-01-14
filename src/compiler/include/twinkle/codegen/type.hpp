/**
 * These codes are licensed under MIT License
 * See the LICENSE for details
 *
 * Copyright (c) 2022 Hiramoto Ittou
 */

#ifndef _c4aa2bde_b6dc_11ec_b909_0242ac120002
#define _c4aa2bde_b6dc_11ec_b909_0242ac120002

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <twinkle/pch/pch.hpp>
#include <twinkle/support/utils.hpp>
#include <twinkle/support/kind.hpp>
#include <twinkle/support/typedef.hpp>
#include <twinkle/unicode/unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <twinkle/ast/ast.hpp>

namespace twinkle::codegen
{

enum class BuiltinTypeKind {
  void_,
  // Integer types.
  i8,
  i16,
  i32,
  i64,
  u8,
  u16,
  u32,
  u64,
  bool_,
  char_,
  // Floating-point types.
  f64,
  f32,
  isize,
  usize,
};

// Forward declaration
struct CGContext;
struct Type;

struct UnionVariant {
  UnionVariant(std::string&&                tag,
               const std::uint8_t           offset,
               llvm::StructType* const      type,
               const std::shared_ptr<Type>& element_type)
    : tag{std::move(tag)}
    , offset{offset}
    , type{type}
    , element_type{element_type}
  {
  }

  UnionVariant(const std::string&           tag,
               const std::uint8_t           offset,
               llvm::StructType* const      type,
               const std::shared_ptr<Type>& element_type)
    : tag{tag}
    , offset{offset}
    , type{type}
    , element_type{element_type}
  {
  }

  const std::string           tag;
  const std::uint8_t          offset;
  llvm::StructType* const     type;
  const std::shared_ptr<Type> element_type;
};

using UnionVariants = std::vector<UnionVariant>;

struct Type {
  explicit Type(const bool is_mutable) noexcept
    : is_mutable{is_mutable}
  {
  }

  virtual ~Type() = default;

  [[nodiscard]] virtual std::shared_ptr<Type> clone() const = 0;

  [[nodiscard]] virtual SignKind getSignKind(CGContext&) const = 0;

  [[nodiscard]] virtual llvm::Type* getLLVMType(CGContext&) const = 0;

  [[nodiscard]] virtual std::string getMangledName(CGContext&) const = 0;

  [[nodiscard]] virtual std::shared_ptr<Type> getPointeeType(CGContext&) const
  {
    unreachable();
  }

  [[nodiscard]] virtual std::shared_ptr<Type> getRefeeType(CGContext&) const
  {
    unreachable();
  }

  [[nodiscard]] virtual std::shared_ptr<Type>
  getArrayElementType(CGContext&) const
  {
    unreachable();
  }

  [[nodiscard]] virtual std::uint64_t getArraySize(CGContext&) const
  {
    unreachable();
  }

  [[nodiscard]] virtual std::string getClassName(CGContext&) const
  {
    unreachable();
  }

  [[nodiscard]] virtual std::string getUserDefinedTyName(CGContext&) const
  {
    unreachable();
  }

  [[nodiscard]] virtual bool isVoidTy(CGContext&) const
  {
    return false;
  }

  [[nodiscard]] virtual bool isIntegerTy(CGContext&) const
  {
    return false;
  }

  [[nodiscard]] virtual bool isFloatingPointTy(CGContext&) const
  {
    return false;
  }

  [[nodiscard]] virtual bool isPointerTy(CGContext&) const
  {
    return false;
  }

  [[nodiscard]] virtual bool isClassTy(CGContext&) const
  {
    return false;
  }

  [[nodiscard]] virtual bool isUnionTy(CGContext&) const
  {
    return false;
  }

  [[nodiscard]] virtual bool isOpaque(CGContext&) const
  {
    return false;
  }

  [[nodiscard]] virtual bool isArrayTy(CGContext&) const
  {
    return false;
  }

  [[nodiscard]] virtual bool isRefTy(CGContext&) const
  {
    return false;
  }

  [[nodiscard]] virtual bool isUserDefinedType() const
  {
    return false;
  }

  [[nodiscard]] virtual const UnionVariants& getUnionVariants(CGContext&) const
  {
    unreachable();
  }

  [[nodiscard]] bool isSigned(CGContext& ctx) const
  {
    return getSignKind(ctx) == SignKind::signed_;
  }

  [[nodiscard]] bool isUnsigned(CGContext& ctx) const
  {
    return getSignKind(ctx) == SignKind::unsigned_;
  }

  [[nodiscard]] virtual bool isMutable() const
  {
    return is_mutable;
  }

  virtual void setMutable(CGContext&, const bool is_mutable)
  {
    this->is_mutable = is_mutable;
  }

protected:
  bool is_mutable;
};

struct BuiltinType : public Type {
  explicit BuiltinType(const BuiltinTypeKind kind,
                       const bool            is_mutable) noexcept
    : Type{is_mutable}
    , kind{kind}
  {
  }

  [[nodiscard]] std::shared_ptr<Type> clone() const override
  {
    return std::make_shared<BuiltinType>(*this);
  }

  [[nodiscard]] SignKind getSignKind(CGContext&) const override;

  [[nodiscard]] bool isVoidTy(CGContext&) const override
  {
    return kind == BuiltinTypeKind::void_;
  }

  [[nodiscard]] bool isFloatingPointTy(CGContext&) const override
  {
    return kind == BuiltinTypeKind::f64 || kind == BuiltinTypeKind::f32;
  }

  [[nodiscard]] bool isIntegerTy(CGContext&) const override;

  [[nodiscard]] llvm::Type* getLLVMType(CGContext& ctx) const override;

  [[nodiscard]] std::string getMangledName(CGContext&) const override;

private:
  const BuiltinTypeKind kind;
};

struct UserDefinedType : public Type {
  explicit UserDefinedType(const std::u32string& ident, const bool is_mutable)
    : Type{is_mutable}
    , ident{unicode::utf32toUtf8(ident)}
  {
  }

  explicit UserDefinedType(const std::string& ident, const bool is_mutable)
    : Type{is_mutable}
    , ident{ident}
  {
  }

  [[nodiscard]] std::shared_ptr<Type> clone() const override
  {
    return std::make_shared<UserDefinedType>(*this);
  }

  [[nodiscard]] SignKind getSignKind(CGContext& ctx) const override
  {
    return getRealType(ctx)->getSignKind(ctx);
  }

  // Return nullptr if the type does not exist
  [[nodiscard]] std::shared_ptr<Type> getRealType(CGContext& ctx) const;

  // Return nullptr if the type does not exist
  [[nodiscard]] llvm::Type* getLLVMType(CGContext& ctx) const override;

  [[nodiscard]] std::shared_ptr<Type>
  getPointeeType(CGContext& ctx) const override
  {
    assert(isPointerTy(ctx));
    return getRealType(ctx)->getPointeeType(ctx);
  }

  [[nodiscard]] std::shared_ptr<Type>
  getArrayElementType(CGContext& ctx) const override
  {
    return getRealType(ctx)->getArrayElementType(ctx);
  }

  [[nodiscard]] std::shared_ptr<Type>
  getRefeeType(CGContext& ctx) const override
  {
    return getRealType(ctx)->getRefeeType(ctx);
  }

  [[nodiscard]] bool isVoidTy(CGContext& ctx) const override
  {
    return getRealType(ctx)->isVoidTy(ctx);
  }

  [[nodiscard]] bool isIntegerTy(CGContext& ctx) const override
  {
    return getRealType(ctx)->isIntegerTy(ctx);
  }

  [[nodiscard]] bool isFloatingPointTy(CGContext& ctx) const override
  {
    return getRealType(ctx)->isFloatingPointTy(ctx);
  }

  [[nodiscard]] bool isOpaque(CGContext& ctx) const override
  {
    return getRealType(ctx)->isOpaque(ctx);
  }

  [[nodiscard]] bool isUnionTy(CGContext& ctx) const override
  {
    return getRealType(ctx)->isUnionTy(ctx);
  }

  [[nodiscard]] bool isClassTy(CGContext& ctx) const override
  {
    return getRealType(ctx)->isClassTy(ctx);
  }

  [[nodiscard]] bool isPointerTy(CGContext& ctx) const override
  {
    return getRealType(ctx)->isPointerTy(ctx);
  }

  [[nodiscard]] bool isArrayTy(CGContext& ctx) const override
  {
    return getRealType(ctx)->isArrayTy(ctx);
  }

  [[nodiscard]] bool isRefTy(CGContext& ctx) const override
  {
    return getRealType(ctx)->isRefTy(ctx);
  }

  [[nodiscard]] const UnionVariants&
  getUnionVariants(CGContext& ctx) const override
  {
    return getRealType(ctx)->getUnionVariants(ctx);
  }

  [[nodiscard]] bool isUserDefinedType() const override
  {
    return true;
  }

  [[nodiscard]] std::uint64_t getArraySize(CGContext& ctx) const override
  {
    return getRealType(ctx)->getArraySize(ctx);
  }

  [[nodiscard]] std::string getClassName(CGContext& ctx) const override
  {
    return getRealType(ctx)->getClassName(ctx);
  }

  [[nodiscard]] std::string getUserDefinedTyName(CGContext&) const override
  {
    return ident;
  }

  [[nodiscard]] std::string getMangledName(CGContext& ctx) const override;

  void setMutable(CGContext& ctx, const bool is_mutable) override
  {
    getRealType(ctx)->setMutable(ctx, is_mutable);
  }

private:
  const std::string ident;
};

struct ClassType : public Type {
  struct MemberVariable {
    std::string           name;
    std::shared_ptr<Type> type;
    Accessibility         accessibility;
  };

  ClassType(CGContext&                    ctx,
            std::vector<MemberVariable>&& members,
            const std::u32string&         name,
            const bool                    is_mutable);

  ClassType(CGContext&                    ctx,
            std::vector<MemberVariable>&& members,
            const std::string&            name,
            const bool                    is_mutable);

  ClassType(CGContext&                    ctx,
            std::vector<MemberVariable>&& members,
            const std::string&            name,
            llvm::StructType* const       type,
            const bool                    is_mutable);

  [[nodiscard]] std::shared_ptr<Type> clone() const override
  {
    return std::make_shared<ClassType>(*this);
  }

  static std::shared_ptr<ClassType> createOpaqueClass(CGContext&         ctx,
                                                      const std::string& ident);

  static std::vector<llvm::Type*>
  extractTypes(CGContext& ctx, const std::vector<MemberVariable>& members);

  void setIsOpaque(const bool val) noexcept
  {
    is_opaque = val;
  }

  // Used to set members to Opaque classes
  void setBody(CGContext&                    ctx,
               std::vector<MemberVariable>&& members_arg) noexcept;

  // Calculate the offset of a member variable
  // Return std::nullopt if there is no matching member
  [[nodiscard]] std::optional<std::size_t>
  offsetByName(const std::string_view member_name) const;

  [[nodiscard]] const MemberVariable&
  getMemberVar(const std::size_t offset) const
  {
    return members.at(offset);
  }

  [[nodiscard]] SignKind getSignKind(CGContext&) const override
  {
    return SignKind::no_sign;
  }

  [[nodiscard]] llvm::Type* getLLVMType(CGContext&) const override
  {
    return type;
  }

  [[nodiscard]] bool isOpaque(CGContext&) const override
  {
    return is_opaque;
  }

  [[nodiscard]] bool isClassTy(CGContext&) const override
  {
    return true;
  }

  [[nodiscard]] std::string getMangledName(CGContext&) const override
  {
    return boost::lexical_cast<std::string>(name.length()) + name;
  }

  [[nodiscard]] std::string getClassName(CGContext&) const override
  {
    return name;
  }

  [[nodiscard]] std::string getUserDefinedTyName(CGContext&) const override
  {
    return name;
  }

private:
  bool                        is_opaque;
  std::vector<MemberVariable> members;
  const std::string           name;

  // If struct is created multiple times, the name will be duplicated, so create
  // it only once and store it in this variable
  llvm::StructType* type;
};

struct UnionType : public Type {
  struct TagWithType {
    TagWithType(std::string&& tag, const std::shared_ptr<Type>& type)
      : tag{std::move(tag)}
      , type{type}
    {
    }

    TagWithType(const std::string& tag, const std::shared_ptr<Type>& type)
      : tag{tag}
      , type{type}
    {
    }

    std::string           tag;
    std::shared_ptr<Type> type;
  };

  using Tags = std::vector<TagWithType>;

  struct Actual {
    Actual(llvm::StructType* const basic_type, UnionVariants&& variants)
      : basic_type{basic_type}
      , variants{std::move(variants)}
    {
    }

    // If struct is created multiple times, the name will be duplicated, so
    // create it only once and store it in this variables
    llvm::StructType* basic_type;
    UnionVariants     variants;
  };

  UnionType(CGContext&         ctx,
            const std::string& name,
            Tags&&             members,
            const bool         is_mutable);

  [[nodiscard]] std::shared_ptr<Type> clone() const override
  {
    return std::make_shared<UnionType>(*this);
  }

  [[nodiscard]] SignKind getSignKind(CGContext&) const override
  {
    return SignKind::no_sign;
  }

  [[nodiscard]] llvm::Type* getLLVMType(CGContext&) const override
  {
    return actual.basic_type;
  }

  [[nodiscard]] bool isUnionTy(CGContext&) const override
  {
    return true;
  }

  [[nodiscard]] std::string getMangledName(CGContext&) const override
  {
    return boost::lexical_cast<std::string>(name.length()) + name;
  }

  [[nodiscard]] std::string getUserDefinedTyName(CGContext&) const override
  {
    return name;
  }

  [[nodiscard]] const UnionVariants& getUnionVariants(CGContext&) const override
  {
    return actual.variants;
  }

  [[nodiscard]] std::optional<const std::reference_wrapper<const UnionVariant>>
  getUnionVariantType(const std::string& tag) const;

private:
  [[nodiscard]] static llvm::StructType*
  createBasicType(CGContext& ctx, const Tags& members, const std::string& name);

  [[nodiscard]] static UnionVariants
  createVariants(CGContext& ctx, const Tags& members, const std::string& name);

  [[nodiscard]] static Actual
  createActual(CGContext& ctx, const Tags& members, const std::string& name);

  const std::string name;

  Actual actual;
};

struct PointerType : public Type {
  explicit PointerType(const std::shared_ptr<Type>& pointee_type,
                       const bool                   is_mutable)
    : Type{is_mutable}
    , pointee_type{pointee_type}
  {
  }

  [[nodiscard]] std::shared_ptr<Type> clone() const override
  {
    return std::make_shared<PointerType>(*this);
  }

  [[nodiscard]] bool isPointerTy(CGContext&) const override
  {
    return true;
  }

  [[nodiscard]] llvm::Type* getLLVMType(CGContext& ctx) const override
  {
    assert(pointee_type);
    return llvm::PointerType::getUnqual(pointee_type->getLLVMType(ctx));
  }

  [[nodiscard]] std::shared_ptr<Type> getPointeeType(CGContext&) const override
  {
    return pointee_type;
  }

  [[nodiscard]] std::string getMangledName(CGContext& ctx) const override;

  [[nodiscard]] SignKind getSignKind(CGContext&) const override
  {
    return SignKind::unsigned_;
  }

  void setMutable(CGContext& ctx, const bool is_mutable) override
  {
    this->is_mutable = is_mutable;
    pointee_type->setMutable(ctx, is_mutable);
  }

private:
  const std::shared_ptr<Type> pointee_type;
};

struct ArrayType : public Type {
  ArrayType(const std::shared_ptr<Type>& element_type,
            const std::uint64_t          array_size,
            const bool                   is_mutable)
    : Type{is_mutable}
    , element_type{element_type}
    , array_size{array_size}
  {
  }

  [[nodiscard]] std::shared_ptr<Type> clone() const override
  {
    return std::make_shared<ArrayType>(*this);
  }

  [[nodiscard]] std::string getMangledName(CGContext& ctx) const override;

  [[nodiscard]] llvm::Type* getLLVMType(CGContext& ctx) const override
  {
    return llvm::ArrayType::get(element_type->getLLVMType(ctx), array_size);
  }

  [[nodiscard]] std::shared_ptr<Type>
  getArrayElementType(CGContext&) const override
  {
    return element_type;
  }

  [[nodiscard]] bool isArrayTy(CGContext&) const override
  {
    return true;
  }

  [[nodiscard]] std::uint64_t getArraySize(CGContext&) const override
  {
    return array_size;
  }

  [[nodiscard]] SignKind getSignKind(CGContext&) const override
  {
    return SignKind::no_sign;
  }

  void setMutable(CGContext& ctx, const bool is_mutable) override
  {
    this->is_mutable = is_mutable;
    element_type->setMutable(ctx, is_mutable);
  }

private:
  const std::shared_ptr<Type> element_type;
  const std::uint64_t         array_size;
};

// Hold pointer type
// However, implement so that dereferences are not required when referencing
struct ReferenceType : public Type {
  explicit ReferenceType(const std::shared_ptr<Type>& refee_type,
                         const bool                   is_mutable)
    : Type{is_mutable}
    , refee_type{refee_type}
  {
  }

  [[nodiscard]] std::shared_ptr<Type> clone() const override
  {
    return std::make_shared<ReferenceType>(*this);
  }

  [[nodiscard]] bool isRefTy(CGContext&) const override
  {
    return true;
  }

  [[nodiscard]] llvm::Type* getLLVMType(CGContext& ctx) const override
  {
    return llvm::PointerType::getUnqual(refee_type->getLLVMType(ctx));
  }

  [[nodiscard]] std::shared_ptr<Type> getRefeeType(CGContext&) const override
  {
    return refee_type;
  }

  [[nodiscard]] std::string getMangledName(CGContext& ctx) const override
  {
    return "R" + refee_type->getMangledName(ctx);
  }

  [[nodiscard]] SignKind getSignKind(CGContext& ctx) const override
  {
    return refee_type->getSignKind(ctx);
  }

  void setMutable(CGContext& ctx, const bool is_mutable) override
  {
    this->is_mutable = is_mutable;
    refee_type->setMutable(ctx, is_mutable);
  }

private:
  std::shared_ptr<Type> refee_type;
};

[[nodiscard]] std::shared_ptr<Type>
createType(CGContext& ctx, const ast::Type& ast, const PositionRange& pos);

} // namespace twinkle::codegen

#endif
