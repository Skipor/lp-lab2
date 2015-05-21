//
// Created by  Vladimir Skipor on 07/05/15.
//

#ifndef LP_LAB2_NODE_H
#define LP_LAB2_NODE_H

#include <iostream>
#include <vector>
#include <memory>
#include <sstream>

class CodeGenContext;

class NStatement;

class NExpression;

class NVariableDeclaration;

class NBaseType;

typedef std::vector<std::unique_ptr<NStatement>> StatementList;
typedef std::vector<std::unique_ptr<NExpression>> ExpressionList;
typedef std::vector<std::unique_ptr<NBaseType>> TypeList;
typedef std::vector<std::unique_ptr<NPattern>> PatternList;
typedef std::vector<std::pair<std::unique_ptr<NExpression>, std::unique_ptr<NExpression>>> CaseToExprList;


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

std::ostream& operator<<(std::ostream& os, const Node& node) {
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


protected:
  T value_;
};

typedef NConstant<long long> NInteger;
typedef NConstant<double> NDouble;
typedef NConstant<bool> NBool;

class NIdentifier : public NExpression, public NPattern {
public:
  std::string& name() {
    return name_;
  }

  NIdentifier(const std::string& name) : name_(name) { }

  NIdentifier(std::string&& name) : name_(name) { }

protected:
  std::string name_;
};


class NBaseType : public Node {
  virtual size_t args_num() const = 0;

  virtual bool is_functional() const {
    return args_num() == 0;
  }
};

class NPrimitiveType : public NBaseType {
public:
  virtual bool is_functional() const override {
    return false;
  }

  virtual size_t args_num() const override {
    return 0;
  }

  std::string& name() {
    return name_;
  }

  NPrimitiveType(const std::string& name) : name_(name) { }

  NPrimitiveType(std::string&& name) : name_(name) { }

protected:
  std::string name_;
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

};

class NTypeDeclaration : public NExpression {

public:
  NTypeDeclaration(NPrimitiveType* id, NType* type) : id_(id), type_(type) {
  }

protected:
  std::unique_ptr<NPrimitiveType> id_;
  std::unique_ptr<NType> type_;

};


class NFunctionCall : public NExpression {
public:
  NFunctionCall(NIdentifier* id) : id_(id) {
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

protected:
  std::unique_ptr<NIdentifier> id_;
  ExpressionList arguments_;
};

class NBinaryOperator : public NExpression {
public:
  int op;
  std::unique_ptr<NExpression> lhs;
  std::unique_ptr<NExpression> rhs;

  NBinaryOperator(NExpression* lhs, int op, NExpression* rhs) :
      lhs(lhs), rhs(rhs), op(op) { }
};


class NVariableDeclaration : public NStatement {
public:
  const NIdentifier& type;
  NIdentifier& id;
  NExpression* assignmentExpr;

  NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
      type(type), id(id) { }

  NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression* assignmentExpr) :
      type(type), id(id), assignmentExpr(assignmentExpr) { }

};

class NDefenition : public NStatement {


public:
  NDefenition() = default;

  NDefenition(NIdentifier* id) : id_(id) {
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
};

//class NConstDef : public NDefenition {
//protected:
//  std::unique_ptr<NConstantBase> value_;
//
//};

class NFunctionPatternDef : public NDefenition {

public:
  NExpression* expression() {
    return expression_.get();
  }

  void set_expression(NExpression * expr) {
    expression_.reset(expr);
  }


protected:
  std::unique_ptr<NExpression> expression_;

};

class NFunctionCaseDef : public NDefenition {

public:
  CaseToExprList & case_to_expr() {
    return case_to_expr_;
  };

protected:
  CaseToExprList case_to_expr_;


};


#endif //LP_LAB2_NODE_H
