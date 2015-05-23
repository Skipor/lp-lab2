//
// Created by  Vladimir Skipor on 07/05/15.
//

#ifndef LP_LAB2_NODE_H
#define LP_LAB2_NODE_H

#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <unordered_map>
#include "parser.hpp"

#define YYDEBUG 1

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;
class NBaseType;
class NPattern;

typedef std::vector<std::unique_ptr<NStatement>> StatementList;
typedef std::vector<std::unique_ptr<NExpression>> ExpressionList;
typedef std::vector<std::unique_ptr<NBaseType>> TypeList;
typedef std::vector<std::unique_ptr<NPattern>> PatternList;

typedef std::pair<std::unique_ptr<NExpression>, std::unique_ptr<NExpression>> CaseToExpr;
typedef std::vector<CaseToExpr> CaseToExprList;


//typedef std::vector<NVariableDeclaration*> VariableList;

class Node {
public:
  virtual ~Node() { }

  std::string to_string() const {
    std::ostringstream ss;
    TranslateTo(ss);
    return ss.str();
  }

protected:
  virtual void TranslateTo(std::ostream& os) const {
    throw std::logic_error(std::string("TranslateTo not imlemented for ") + typeid(*this).name());
  }

private:
  friend std::ostream& operator<<(std::ostream&, const Node&);
};

inline std::ostream& operator<<(std::ostream& os, const Node& node) {
  node.TranslateTo(os);
  return os;
}

class NPattern : public Node {
//  virtual  bool is_blank() {
//    return false;
//  }

};

class NBlank : public NPattern {
//  virtual  bool is_blank() override {
//    return true;
//  }

};

class NExpression : public Node {
public:
  typedef std::unordered_map<std::string, std::string> NameToNameMap;
  void RenameIds(NameToNameMap const & map) = 0;
};

class NStatement : public Node {
};

class NConstantBase : public NExpression, public NPattern {
};

template<typename T>
class NConstant : public NConstantBase {
public:

  NConstant(T const& value) : value_(value) { }

  NConstant(T&& value) : value_(value) { }

  NConstant& operator=(const NConstant&) = default;

  NConstant& operator=(NConstant&&) = default;

  T& value() const {
    return value_;
  }


  void set_value(T value) {
    value_ = value;
  }

  void RenameIds(NameToNameMap const& map) {
  }


protected:
  T value_;

  virtual void TranslateTo(std::ostream& os) const override {
    os << value();
  }
};

typedef NConstant<long long> NInteger;
typedef NConstant<double> NDouble;
typedef NConstant<bool> NBool;

class NIdentifier : public NExpression, public NPattern {
public:
  std::string const & name() const {
    return name_;
  }

  NIdentifier(const std::string& name) : name_(name) { }

  NIdentifier(std::string&& name) : name_(name) { }

  void RenameIds(NameToNameMap const& map) {
    if(map.count(name_)) {
      name_ = map.at(name_);
    }
  }

protected:
  std::string name_;

  virtual void TranslateTo(std::ostream& os) const override {
    os << name();
  }
};


class NBaseType : public Node {
public:
  virtual size_t args_num() const = 0;

  virtual bool is_functional() const {
    return args_num() == 0;
  }
};

class NPrimitiveType : public NBaseType {
public:
  NPrimitiveType(const std::string& name) : name_(name) { }

  NPrimitiveType(std::string&& name) : name_(name) { }

  virtual bool is_functional() const override {
    return false;
  }

  virtual size_t args_num() const override {
    return 0;
  }

  std::string& name() {
    return name_;
  }


protected:
  std::string name_;

  virtual void TranslateTo(std::ostream& os) const override {
    static const std::unordered_map<std::string, std::string> primitive_type_map =
        {
            {"Integer", "long long"},
            {"Double", "double"},
            {"Float", "float"},
            {"Bool", "bool"}
        };

    if(primitive_type_map.count(name_)) {
       os << primitive_type_map[name_];
    } else {
      throw std::logic_error("Invalid primitive type: " + name_);
    }

  }
};


class NType : public NBaseType {
public:

  virtual size_t args_num() const override {
    return types_.size() - 1;
  }

  NPrimitiveType* return_type() {
    return dynamic_cast<NPrimitiveType*>(types_.back().get());
  };

  TypeList& types() {
    return types_;
  }

protected:
  TypeList types_;
  virtual void TranslateTo(std::ostream& os) const override {
    if (is_functional()) {
      os << "std::function<"
      << *types_.back()
      << "(";

      for (auto it = types_.begin(), end = --types_.end(); it != end;) {
        os << **it;
        ++it;
        if (it != end) {
          os << ", ";
        }
      }
      os << ")>";
    } else {
      os << *types_.back();
    }
  }



};

class NTypeDeclaration : public NStatement {

public:
  NTypeDeclaration(NIdentifier* id, NType* type) : id_(id), type_(type) {
  }

protected:
  std::unique_ptr<NIdentifier> id_;
  std::unique_ptr<NType> type_;

  virtual void TranslateTo(std::ostream& os) const override {
    os << "std::function<";
    os << *type_;
    os << "> " << *id_ << ";\n";
  }

};


class NFunctionCall : public NExpression {
public:
  NFunctionCall(NIdentifier* id, ExpressionList * args) : id_(id), arguments_(std::move(*args)) {
    delete args;
  }

  NIdentifier* id() const {
    return id_.get();
  }

  void set_id(NIdentifier* id) {
    id_.reset(id);
  }

  ExpressionList& arguments() {
    return arguments_;
  }

  void RenameIds(NameToNameMap const& map) {
    for(auto expr_ptr : arguments_) {
      expr_ptr->RenameIds(map);
    }
  }

protected:
  virtual void TranslateTo(std::ostream& os) const override {
    os << *id_ << "(";
    for (auto it = arguments_.begin(), end = arguments_.end(); it != end;) {
      os << **it;
      ++it;
      if (it != end) {
        os << ", ";
      }
    }
  }

  std::unique_ptr<NIdentifier> id_;
  ExpressionList arguments_;
};

class NBinaryOperator : public NExpression {
public:
  const int op;
  std::unique_ptr<NExpression> lhs;
  std::unique_ptr<NExpression> rhs;

  NBinaryOperator(NExpression* lhs, int op, NExpression* rhs) :
      lhs(lhs), rhs(rhs), op(op) { }
  void RenameIds(NameToNameMap const& map) {
    lhs->RenameIds(map);
    rhs->RenameIds(map);
  }
protected:
  virtual void TranslateTo(std::ostream& os) const override {
    static const std::unordered_map<int, std::string> token_to_operator =
        {
            {TCEQ  ,"==" },
            {TCNE  ,"!=" },
            {TCLT  ,"<" },
            {TCLE  ,"<=" },
            {TCGT  ,">" },
            {TCGE  ,">=" },
            {TDIV  ,"/" },
            {TMUL  ,"*" },
            {TMINUS,"-" },
            {TPLUS ,"+" }
        };
    if(token_to_operator.count(op)) {
      os << "(" << *lhs << ") " << token_to_operator.at(op) << " (" << *rhs << ")";
    } else {
      throw std::logic_error("Unknown operator: " + std::to_string(op));
    }

  }
};

class NDefinition : public NStatement {

public:

  NDefinition() = default;

  NDefinition(NDefinition&&) = default;

  NDefinition(NIdentifier* id, PatternList* patterns) : id_(id), patterns_(std::move(*patterns)) {
    delete patterns;
  }

  NIdentifier* id() const {
    return id_.get();
  }

  void set_id(NIdentifier* id) {
    id_.reset(id);
  }

  PatternList& patterns() {
    return patterns_;
  }

protected:
  std::unique_ptr<NIdentifier> id_;
  PatternList patterns_;
  static const std::string intend("  ");
  static std::string GetArgByNum(size_t arg_num) {
    return "arg" + std::to_string(arg_num);
  }
};

//class NConstDef : public NDefinition {
//protected:
//  std::unique_ptr<NConstantBase> value_;
//
//};

class NFunctionPatternDef : public NDefinition {

public:
  NFunctionPatternDef(NDefinition* base, NExpression* expr)
      : NDefinition(std::move(*base)), expression_(expr) {
    delete base;
  }

  NFunctionPatternDef(NIdentifier* id, PatternList* patterns, NExpression* expr)
      : NDefinition(id, patterns), expression_(expr) {
  }

  NExpression* expression() {
    return expression_.get();
  }

  void set_expression(NExpression* expr) {
    expression_.reset(expr);
  }


protected:
  std::unique_ptr<NExpression> expression_;


};

class NFunctionCaseDef : public NDefinition {

public:

  NFunctionCaseDef(NDefinition* base, CaseToExprList* case_to_expr_list)
      : NDefinition(std::move(*base)), case_to_expr_(std::move(*case_to_expr_list)) {
    delete base;
    delete case_to_expr_list;
  }

  NFunctionCaseDef(NIdentifier* id, PatternList* patterns, CaseToExprList* case_to_expr_list)
      : NDefinition(id, patterns), case_to_expr_(std::move(*case_to_expr_list)) {
    delete case_to_expr_list;
  }

  CaseToExprList& case_to_expr() {
    return case_to_expr_;
  };

protected:
  CaseToExprList case_to_expr_;

  virtual void TranslateTo(std::ostream& os) const override {
    for(auto const & case_to_expr: case_to_expr_) {
      TranslateCaseToExpr(os, case_to_expr);
    }
  }

  virtual void TranslateCaseToExpr(std::ostream & os, CaseToExpr const & case_to_expr) const  {
    os << intend << "if (" << *case_to_expr.first << ") {"
    << intend << intend << "return " << *case_to_expr.second << ";"
    << intend << "}\n";
  }


};


#endif //LP_LAB2_NODE_H
