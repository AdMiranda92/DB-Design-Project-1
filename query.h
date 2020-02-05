//Author:  William Hendrix
//Developed for Database Design project
//Spring 2020

#ifndef __QUERY_H
#define __QUERY_H

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <exception>
#include <cctype>
using namespace std;

#include "table.h"

enum optype_t { OR = 0, AND, EQUALS, NOT_EQUALS, LESS_THAN_EQUALS, GREATER_THAN_EQUALS, LESS_THAN, GREATER_THAN, PLUS, MINUS, TIMES, DIVIDE };

const string OP[] = {"||", "&&", "==", "!=", "<=", ">=", "<", ">", "+", "-", "*", "/"};
const int NUM_OPS = 12;

class Table;
class Row;
class Query;
class ConditionNode;
class ValueNode;
class ConstantNode;
class BoolConstantNode;
class IntConstantNode;
class StringConstantNode;
class AttributeNode;
class OperatorNode;

//Single, abstract node in the query condition
//May represent a value or operation in the expression
//Main useful methods are getBoolValue, getIntValue, getStringValue, and getType
//Call "get value" function that's appropriate to the node's type to evaluate the expression rooted at this node
//Should return false, 0, or "" if the tree has an error
class ConditionNode
{
protected:
  const Table* table;
public:
  ConditionNode(const Table* t) { table = t; }
  virtual ~ConditionNode() {}
  virtual bool isOperator() const = 0;
  virtual bool isAttribute() const = 0;
  virtual datatype_t getType() const = 0;
  virtual const void* evaluate(const Row& row) = 0;
  virtual ConditionNode* left() { return nullptr; }
  virtual ConditionNode* right() { return nullptr; }
  virtual const ConditionNode* left() const { return nullptr; }
  virtual const ConditionNode* right() const { return nullptr; }
  bool getBoolValue(const Row& row);
  int32_t getIntValue(const Row& row);
  string getStringValue(const Row& row);
  static ConditionNode* parseCondition(const string&, const Table*);
};

//ConditionNode containing a value
//Stores an explicit type, but will not have children
class ValueNode : public ConditionNode
{
protected:
  datatype_t type;
public:
  ValueNode(datatype_t typ, const Table* tab) : ConditionNode(tab) { type = typ; }
  bool isOperator() const { return false; }
  datatype_t getType() const { return type; }
  void setType(datatype_t t) { type = t; }
};

//Node representing an attribute
//Determines its type and value based on Table::getAttributeType and Row::getValue
class AttributeNode : public ValueNode
{
private:
  string name;
  void* value;
public:
  AttributeNode(string n, const Table* tab) : ValueNode(UNKNOWN, tab), name(n) { if (table != nullptr) setType(table->getAttributeType(n)); value = nullptr; }
  bool isAttribute() const override { return true; }
  const string& getName() const { return name; }
  const void* evaluate(const Row& row) override { setValue(row.getValue(name)); return value; }
  void setValue(const void* v);
};

//Node representing a constant value in the expression
class ConstantNode : public ValueNode
{
public:
  ConstantNode(datatype_t typ, const Table* t) : ValueNode(typ, t) {}
  bool isAttribute() const override { return false; } 
};

//Boolean constant
class BoolConstantNode : public ConstantNode
{
private:
  bool value;
public:
  BoolConstantNode(bool b, const Table* tab) : ConstantNode(BOOL, tab) { value = b; } 
  datatype_t getType() const override { return BOOL; }
  const void* evaluate(const Row& row) override { return &value; }
};

//Integer constant
class IntConstantNode : public ConstantNode
{
private:
  int32_t value;
public:
  IntConstantNode(int32_t i, const Table* tab) : ConstantNode(INT, tab) { value = i; }
  datatype_t getType() const override { return INT; }
  const void* evaluate(const Row& row) override { return &value; }
};

//String constant
class StringConstantNode : public ConstantNode
{
private:
  string value;
public:
  StringConstantNode(string s, const Table* tab) : ConstantNode(STRING, tab) { value = s;}
  datatype_t getType() const override { return STRING; }
  const void* evaluate(const Row& row) override { return &value; }
};

//Node representing an operation
//Will have two children (all supported ops are binary)
class OperatorNode : public ConditionNode
{
private:
  optype_t optype;
  ConditionNode* lhs;
  ConditionNode* rhs;
  const void* value;
  const void* boolEval(const Row& row);
  const void* intEval(const Row& row);
  const void* stringEval(const Row& row);
public:
  OperatorNode(const string&, const Table*);
  ~OperatorNode();
  bool isAttribute() const override { return false; }
  bool isOperator() const override { return true; }
  optype_t getOpType() const { return optype; }
  ConditionNode* left() override { return lhs; }
  ConditionNode* right() override { return rhs; }
  const ConditionNode* left() const override { return lhs; }
  const ConditionNode* right() const override { return rhs; }
  datatype_t getType() const override;
  const void* evaluate(const Row& row) override;
};

//Class representing a database query
class Query
{
private:
  vector<string> attribs;
  Table* table;
  ConditionNode* condition;
public:
  //Constructs query based off of a string
  //Expected format:  SELECT attributes FROM table WHERE condition
  //Attributes can be comma separated list or * for all
  //Condition uses mostly C syntax, with operators ||, &&, ==, !=, <=, >=, <, >, +, -, *, and /
  //String constants in condition should be enclosed in quotes ("")
  //Uses table name to construct a Table using Table::getTableByName
  //Uses "quick and dirty" parser
  //Should be whitespace agnostic
  //Some restrictions:
  //Key words (SELECT, FROM, WHERE, TRUE, FALSE) must be capitalized
  //Negative numbers not recognized (can use 0 - # instead)
  //Order of operations is unconventional, but should result in the same answer as expected
  //Malformed queries were not tested extensively--type it right!
  Query(const string&);

  //Explicit value constructor
  Query(const char** att, int numatts, const char* tname, const char* cond);

  //Destructor
  ~Query() { delete condition; delete table; }

  //Return vector of attributes this query should output
  const vector<string>& getAttributesToReturn() const { return attribs; }

  //Returns a pointer to the table returned by Table::getTableByName(table_name)
  Table* getTable() { return table; }
  const Table* getTable() const { return table; }

  //Returns a pointer to the root of the condition tree
  ConditionNode* getCondition() { return condition; }
  const ConditionNode* getCondition() const { return condition; }
};

//Print functions
//Conditions are printed in reverse Polish notation (RPN), with attribute names prefixed by "A:"
//Query prints very similar to string constructor, except with condition in RPN
ostream& operator<<(ostream&, Query&);
ostream& operator<<(ostream&, ConditionNode*);
ostream& operator<<(ostream&, BoolConstantNode*);
ostream& operator<<(ostream&, StringConstantNode*);
ostream& operator<<(ostream&, IntConstantNode*);
ostream& operator<<(ostream&, AttributeNode*);
ostream& operator<<(ostream&, OperatorNode*);

#endif