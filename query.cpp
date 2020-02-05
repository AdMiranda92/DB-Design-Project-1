//Author:  William Hendrix
//Developed for Database Design project
//Spring 2020

#include "query.h"
#include <cctype>
#include <sstream>
#include <cstring>
using namespace std;

Query::Query(const char** att, int natts, const char* tname, const char* cond)
{
  for (int i = 0; i < natts; i++)
    attribs.emplace_back(att[i]);
  table = Table::getTableByName(tname);
  condition = ConditionNode::parseCondition(string(cond), table);
}

void trim(const string& str, int& left, int& right)
{
  while (left < right && str[left] == ' ')
    left++;
  if (left >= right)
    return;
  while (left < right && str[right - 1] == ' ')
    right--;
}

Query::Query(const string& str)
{
  static const string PART[] = {"SELECT", "FROM", "WHERE"};
  static const int NPARTS = 3;
  
  int loc[NPARTS], left, right;
  for (int i = 0; i < NPARTS; i++)
  {
    loc[i] = str.find(PART[i]);
    if (loc[i] == string::npos)
    {
      table = nullptr;
      condition = nullptr;
      return;
    }
  }
  
  left = loc[0] + PART[0].size();
  right = loc[1];
  trim(str, left, right);
  int comma = str.find(',', left);
  while (comma != string::npos && comma < loc[1])
  {
    trim(str, left, comma);
    attribs.emplace_back(str.substr(left, comma - left));
    left = comma + 1;
    comma = str.find(',', left);
  }
  trim(str, left, right);
  attribs.emplace_back(str.substr(left, right - left));

  left = loc[1] + PART[1].size();
  right = loc[2];
  trim(str, left, right);
  table = Table::getTableByName(str.substr(left, right - left));

  left = loc[2] + PART[2].size();
  right = str.size();
  trim(str, left, right);
  condition = ConditionNode::parseCondition(str.substr(left, right - left), table);
}

bool ConditionNode::getBoolValue(const Row& row)
{
  const void* temp = evaluate(row);
  if (temp == nullptr || getType() != BOOL)
    return false;
  else
    return *reinterpret_cast<const bool*>(temp);
}

int32_t ConditionNode::getIntValue(const Row& row)
{
  const void* temp = evaluate(row);
  if (temp == nullptr || getType() != INT)
    return 0;
  else
    return *reinterpret_cast<const int32_t*>(temp);
}

string ConditionNode::getStringValue(const Row& row)
{
  const void* temp = evaluate(row);
  if (temp == nullptr || getType() != STRING)
    return "";
  else
    return *reinterpret_cast<const string*>(temp);
}

void AttributeNode::setValue(const void* val)
{
  switch (type)
  {
  case BOOL:
    delete reinterpret_cast<const bool*>(value);
    value = new bool(*reinterpret_cast<const bool*>(val));
//cout << "Retrieved " << name << "=" << (*reinterpret_cast<const bool*>(value)? "TRUE":"FALSE") << endl;
    return;
  case INT:
    delete reinterpret_cast<const int32_t*>(value);
    value = new int32_t;
    memcpy(value, val, sizeof(int32_t));
//cout << "Retrieved " << name << "=" << *reinterpret_cast<const int32_t*>(value) << endl;
    return;
  case STRING:
    delete reinterpret_cast<const string*>(value);
    value = new string(reinterpret_cast<const char*>(val));
//cout << "Retrieved " << name << "=" << *reinterpret_cast<const string*>(value) << endl;
    return;
  default:
    return;
  }
}

datatype_t OperatorNode::getType() const
{
  switch (optype)
  {
  case EQUALS:
  case NOT_EQUALS:
  case LESS_THAN:
  case GREATER_THAN:
  case LESS_THAN_EQUALS:
  case GREATER_THAN_EQUALS:
    return BOOL;
  default:
    if (rhs != nullptr)
      return rhs->getType();
    else
      return UNKNOWN; //Should never happen
  }
  return UNKNOWN; //Should never happen
}

const void* OperatorNode::evaluate(const Row& row)
{
  if (value != nullptr)
    switch (getType())
    {
    case BOOL:
      delete reinterpret_cast<const bool*>(value);
      break;
    case INT:
      delete reinterpret_cast<const int32_t*>(value);
      break;
    case STRING:
      delete reinterpret_cast<const string*>(value);
      break;
    default:
      break;
    }
  value = nullptr;
  
  if (rhs == nullptr || lhs == nullptr)
    return nullptr;

  switch (rhs->getType())
  {
  case BOOL:
    value = boolEval(row);
    //cout << lhs << ' ' << rhs << ' ' << OP[otype] << ":  " << (*reinterpret_cast<const bool*>(value)?"TRUE":"FALSE") << endl;
    return value;
  case INT:
    value = intEval(row);
    //cout << lhs << ' ' << rhs << ' ' << OP[otype] << ":  " << *reinterpret_cast<const int32_t*>(value) << endl;
    return value;
  case STRING:
    value = stringEval(row);
    //cout << lhs << ' ' << rhs << ' ' << OP[otype] << ":  " << *reinterpret_cast<const char*>(value) << endl;
    return value;
  default: //Should never happen
    return nullptr;
  }
}

const void* OperatorNode::boolEval(const Row& row)
{
  const bool* left;
  const bool* right;

  try
  {
    left = reinterpret_cast<const bool*>(lhs->evaluate(row));
    right = reinterpret_cast<const bool*>(rhs->evaluate(row));
    switch (optype)
    {
    case EQUALS:
      return new bool(*left == *right);
    case NOT_EQUALS:
      return new bool(*left != *right);
    case LESS_THAN:
      return new bool(*left < *right);
    case GREATER_THAN:
      return new bool(*left > *right);
    case LESS_THAN_EQUALS:
      return new bool(*left <= *right);
    case GREATER_THAN_EQUALS:
      return new bool(*left >= *right);
    case AND:
      return new bool(*left && *right);
    case OR:
      return new bool(*left || *right);
    default:
      return nullptr;
    }
  }
  catch (...)
  {
    return nullptr;
  }
  return nullptr;
}

const void* OperatorNode::intEval(const Row& row)
{
  const int32_t* left;
  const int32_t* right;
  
  try
  {
    left = reinterpret_cast<const int32_t*>(lhs->evaluate(row));
    right = reinterpret_cast<const int32_t*>(rhs->evaluate(row));
    switch (optype)
    {
    case EQUALS:
      return new bool(*left == *right);
    case NOT_EQUALS:
      return new bool(*left != *right);
    case LESS_THAN:
      return new bool(*left < *right);
    case GREATER_THAN:
      return new bool(*left > *right);
    case LESS_THAN_EQUALS:
      return new bool(*left <= *right);
    case GREATER_THAN_EQUALS:
      return new bool(*left >= *right);
    case AND:
      return new int32_t(*left & *right);
    case OR:
      return new int32_t(*left | *right);
    case PLUS:
      return new int32_t(*left + *right);
    case MINUS:
      return new int32_t(*left - *right);
    case TIMES:
      return new int32_t(*left * *right);
    case DIVIDE:
      return new int32_t(*left / *right);
    default:
      return nullptr;
    }
  }
  catch (...)
  {
    return nullptr;
  }
  return nullptr;
}

const void* OperatorNode::stringEval(const Row& row)
{
  const string* left;
  const string* right;
  
  try
  {
    left = reinterpret_cast<const string*>(lhs->evaluate(row));
    right = reinterpret_cast<const string*>(rhs->evaluate(row));
    switch (optype)
    {
    case EQUALS:
      return new bool(*left == *right);
    case NOT_EQUALS:
      return new bool(*left != *right);
    case LESS_THAN:
      return new bool(*left < *right);
    case GREATER_THAN:
      return new bool(*left > *right);
    case LESS_THAN_EQUALS:
      return new bool(*left <= *right);
    case GREATER_THAN_EQUALS:
      return new bool(*left >= *right);
    case PLUS:
      return new string(*left + *right);
    default:
      return nullptr;
    }
  }
  catch (...)
  {
    return nullptr;
  }
  return nullptr;
}

ConditionNode* ConditionNode::parseCondition(const string& str, const Table* table)
{
  for (int i = 0; i < NUM_OPS; i++)
    if (str.find(OP[i]) != string::npos)
      return new OperatorNode(str, table);
  if (str == "TRUE" || str == "FALSE")
    return new BoolConstantNode(str == "TRUE", table);
  else if (str[0] == '"' && str[str.size() - 1] == '"')
    return new StringConstantNode(str.substr(1, str.size() - 2), table);
  else if (isdigit(str[0]))
  {
    istringstream iss(str);
    int32_t temp;
    iss >> temp;
    return new IntConstantNode(temp, table);
  }
  else if (str != "")
    return new AttributeNode(str, table);
  else
    return new StringConstantNode("PARSING ERROR", table);
}

OperatorNode::OperatorNode(const string& str, const Table* t) : ConditionNode(t)
{
  int minPriority = NUM_OPS;
  int minLoc = 0;
  int level = 0;
  int loc = 0, end = str.size() - 1;
  while (str[loc] == '(' && str[end] == ')')
  {
    loc++;
    end--;
  }
  for (; loc <= end; loc++)
    if (str[loc] == '(')
      level++;
    else if (str[loc] == ')')
      level--;
    else if (level == 0)
      for (int op = 0; op < minPriority; op++)
        if (loc + OP[op].size() <= end && str.substr(loc, OP[op].size()) == OP[op])
        {
          minPriority = op;
          minLoc = loc;
        }
  
  loc = minLoc - 1;
  while (str[loc] == ' ' )
    loc--;
  end = minLoc + OP[minPriority].size();
  while (str[end] == ' ')
    end++;

  optype = static_cast<optype_t>(minPriority);
  lhs = ConditionNode::parseCondition(str.substr(0, loc+1), table);
  rhs = ConditionNode::parseCondition(str.substr(end), table);
  value = nullptr;
}

OperatorNode::~OperatorNode()
{
  if (value != nullptr)
    switch (getType())
    {
    case BOOL:
      delete reinterpret_cast<const bool*>(value);
      break;
    case INT:
      delete reinterpret_cast<const int32_t*>(value);
      break;
    case STRING:
      delete reinterpret_cast<const string*>(value);
      break;
    default:
      break;
    }
  delete lhs;
  delete rhs;
}

ostream& operator<<(ostream& out, Query& q)
{
  out << "SELECT ";
  for (string s : q.getAttributesToReturn())
    out << s << ' ';
  out << "FROM " << q.getTable()->getName() << " WHERE ";
  if (q.getCondition() == nullptr)
    return out << "ERROR";
  else
    return out << q.getCondition();
}

ostream& operator<<(ostream& out, ConditionNode* c)
{
  if (c == nullptr)
    return out;
  if (c->isOperator())
    return out << reinterpret_cast<OperatorNode*>(c);
  else if (c->isAttribute())
    return out << reinterpret_cast<AttributeNode*>(c);
  else
    switch (c->getType())
    {
    case BOOL:
      return out << reinterpret_cast<BoolConstantNode*>(c);
    case INT:
      return out << reinterpret_cast<IntConstantNode*>(c);
    case STRING:
      return out << reinterpret_cast<StringConstantNode*>(c);
    default:
      return out << "ERROR";
    }
}

ostream& operator<<(ostream& out, BoolConstantNode* bc)
{
  if (bc == nullptr)
    return out;
  if (bc->getBoolValue(Row()))
    return out << "TRUE";
  else
    return out << "FALSE";
}

ostream& operator<<(ostream& out, IntConstantNode* ic)
{
  if (ic == nullptr)
    return out;
  return out << ic->getIntValue(Row());
}

ostream& operator<<(ostream& out, StringConstantNode* sc)
{
  if (sc == nullptr)
    return out;
  return out << sc->getStringValue(Row());
}

ostream& operator<<(ostream& out, AttributeNode* a)
{
  if (a == nullptr)
    return out;
  return out << "A:" << a->getName();
}

ostream& operator<<(ostream& out, OperatorNode* op)
{
  if (op == nullptr)
    return out;
  return out << op->left() << ' ' << op->right() << ' ' << OP[op->getOpType()];
}