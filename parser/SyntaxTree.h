/*
 * Copyright (C) 2010 Sencha, Inc.
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SyntaxTree_h
#define SyntaxTree_h

#include <JSGlobalData.h>
#include <Nodes.h>
#include <wtf/Vector.h>

namespace JSC {

class Identifier;
class JSGlobalData;
class Lexer;

namespace SyntaxTree {

class Node;

class Visitor
{
public:
    virtual void process(Node* n) = 0;
};

class Node
{
public:

    inline void* operator new(size_t size, JSGlobalData* globalData)
    {
        return globalData->parser->arena().allocateFreeable(size);
    }

    typedef enum {
        ArgumentsListType = 0,
        ArgumentsType,
        ArrayType,
        AssignmentExpressionType,
        AssignType,
        BinaryExpressionType,
        BlockStatementType,
        BooleanExpressionType,
        BracketAccessType,
        BreakStatementType,
        CommaType = 10,
        ConditionalExpressionType,
        ConstDeclarationType,
        ConstStatementType,
        ContinueStatementType,
        ClauseType,
        ClauseListType,
        DebuggerType,
        DeclarationType,
        DoWhileStatementType,
        DotAccessType = 20,
        ElementListType,
        EmptyStatementType,
        ExpressionStatementType,
        ExpressionType,
        ForInLoopType,
        ForLoopType,
        FormalParameterListType,
        FunctionBodyType,
        FunctionCallType,
        FunctionDeclStatementType = 30,
        FunctionExpressionType,
        IdentifierExpressionType,
        IfStatementType,
        LabelStatementType,
        NewExpressionType,
        NullType,
        NumberExpressionType,
        ObjectLiteralType,
        PostfixType,
        PrefixType = 40,
        PropertyType,
        PropertyListType,
        RegexType,
        ResolveType,
        ReturnStatementType,
        SourceElementsType,
        StatementType,
        StringExpressionType,
        SwitchStatementType,
        ThisType = 50,
        ThrowStatementType,
        TryStatementType,
        UnaryExpressionType,
        VariableDeclarationType,
        VoidType,
        WhileStatementType,
        WithStatementType
    } Type;

    typedef enum {
        NoOperator,
        TypeofOperator,
        DeleteOperator,
        LogicalNotOperator,
        LogicalOrOperator,
        LogicalAndOperator,
        BitwiseNotOperator,
        BitwiseOrOperator,
        BitwiseXorOperator,
        BitwiseAndOperator,
        EqualOperator,
        NotEqualOperator,
        StringEqualOperator,
        StringNotEqualOperator,
        LessThanOperator,
        GreaterThanOperator,
        LessThanOrEqualOperator,
        GreaterThanOrEqualOperator,
        InstanceofOperator,
        IntokenOperator,
        LeftShiftOperator,
        RightShiftOperator,
        ZeroFillRightShiftOperator,
        AddOperator,
        SubtractOperator,
        MultiplyOperator,
        DivideOperator,
        ModulusOperator,
        PlusPlusOperator,
        MinusMinusOperator,
        AssignEqual,
        AssignAdd,
        AssignSubtract,
        AssignMultiply,
        AssignDivide,
        AssignModulus,
        AssignLeftShift,
        AssignRightShift,
        AssignZeroFillRightShift,
        AssignAnd,
        AssignXor,
        AssignOr
    } OperatorType;


    static OperatorType convertOperator(Operator o) {
        OperatorType op = NoOperator;
        switch (o) {
        case OpEqual:       op = AssignEqual; break;
        case OpPlusEq:      op = AssignAdd; break;
        case OpMinusEq:     op = AssignSubtract; break;
        case OpMultEq:      op = AssignMultiply; break;
        case OpDivEq:       op = AssignDivide; break;
        case OpPlusPlus:    op = PlusPlusOperator; break;
        case OpMinusMinus:  op = MinusMinusOperator; break;
        case OpAndEq:       op = AssignAnd; break;
        case OpXOrEq:       op = AssignXor; break;
        case OpOrEq:        op = AssignOr; break;
        case OpModEq:       op = AssignModulus; break;
        case OpLShift:      op = AssignLeftShift; break;
        case OpRShift:      op = AssignRightShift; break;
        case OpURShift:     op = AssignZeroFillRightShift; break;
        default: CRASH(); break;
        }
        return op;
    }

    static OperatorType convertOperator(int tokenType) {
        OperatorType op = NoOperator;
        switch (tokenType) {
        case OR:           op = LogicalOrOperator; break;
        case AND:          op = LogicalAndOperator; break;
        case BITOR:        op = BitwiseOrOperator; break;
        case BITXOR:       op = BitwiseXorOperator; break;
        case BITAND:       op = BitwiseAndOperator; break;
        case EQEQ:         op = EqualOperator; break;
        case NE:           op = NotEqualOperator; break;
        case STREQ:        op = StringEqualOperator; break;
        case STRNEQ:       op = StringNotEqualOperator; break;
        case LT:           op = LessThanOperator; break;
        case GT:           op = GreaterThanOperator; break;
        case LE:           op = LessThanOrEqualOperator; break;
        case GE:           op = GreaterThanOrEqualOperator; break;
        case INSTANCEOF:   op = InstanceofOperator; break;
        case INTOKEN:      op = IntokenOperator; break;
        case LSHIFT:       op = LeftShiftOperator; break;
        case RSHIFT:       op = RightShiftOperator; break;
        case URSHIFT:      op = ZeroFillRightShiftOperator; break;
        case PLUS:         op = AddOperator; break;
        case MINUS:        op = SubtractOperator; break;
        case TIMES:        op = MultiplyOperator; break;
        case DIVIDE:       op = DivideOperator; break;
        case MOD:          op = ModulusOperator; break;
        case PLUSPLUS:     op = AddOperator; break;
        case MINUSMINUS:   op = SubtractOperator; break;
        case EQUAL:        op = AssignEqual; break;
        case PLUSEQUAL:    op = AssignAdd; break;
        case MINUSEQUAL:   op = AssignSubtract; break;
        case MULTEQUAL:    op = AssignMultiply; break;
        case DIVEQUAL:     op = AssignDivide; break;
        case LSHIFTEQUAL:  op = AssignLeftShift; break;
        case RSHIFTEQUAL:  op = AssignRightShift; break;
        case URSHIFTEQUAL: op = AssignZeroFillRightShift; break;
        case ANDEQUAL:     op = AssignAnd; break;
        case XOREQUAL:     op = AssignXor; break;
        case OREQUAL:      op = AssignOr; break;
        case MODEQUAL:     op = AssignModulus; break;
        default: CRASH(); break;
        }
        return op;
    }

    int type() const { return m_type; }

    void append(Node* n) { m_children.append(n); }

    int childCount() const { return m_children.size(); }

    Node* childAt(int i) const { return m_children.at(i); }

    void apply(Visitor* visitor) { visitor->process(this); }

    Node(Type type)
        : m_type(type)
        , m_operator(NoOperator)
        , m_boolean(false)
        , m_number(0)
        , m_propertyType(PropertyNode::Constant)
        , start(0)
        , end(0)
    {
    }

    explicit Node(bool b)
        : m_type(BooleanExpressionType)
        , m_operator(NoOperator)
        , m_boolean(b)
        , m_number(0)
        , m_propertyType(PropertyNode::Constant)
        , start(0)
        , end(0)
    {
    }

    explicit Node(double d)
        : m_type(NumberExpressionType)
        , m_operator(NoOperator)
        , m_boolean(false)
        , m_number(d)
        , m_propertyType(PropertyNode::Constant)
        , start(0)
        , end(0)
    {
    }

    explicit Node(const UString &str)
        : m_type(StringExpressionType)
        , m_operator(NoOperator)
        , m_boolean(false)
        , m_number(0)
        , m_string(str)
        , m_propertyType(PropertyNode::Constant)
        , start(0)
        , end(0)
    {
    }

    explicit Node(Type type, OperatorType op)
        : m_type(type)
        , m_operator(op)
        , m_boolean(false)
        , m_number(0)
        , m_propertyType(PropertyNode::Constant)
        , start(0)
        , end(0)
    {
    }

    explicit Node(Type type, const Identifier& id)
        : m_type(type)
        , m_boolean(false)
        , m_number(0)
        , m_identifier(id)
        , m_propertyType(PropertyNode::Constant)
        , start(0)
        , end(0)
    {
    }

    explicit Node(Type type, const Identifier& pattern, const Identifier& flags)
        : m_type(type)
        , m_boolean(false)
        , m_number(0)
        , m_string(flags.ustring())
        , m_identifier(pattern)
        , m_propertyType(PropertyNode::Constant)
        , start(0)
        , end(0)
    {
    }

    explicit Node(Type type, Node* expr)
        : m_type(type)
        , m_boolean(false)
        , m_number(0)
        , m_propertyType(PropertyNode::Constant)
        , start(0)
        , end(0)
    {
        m_children.append(expr);
    }

    const Identifier& identifier() const { return m_identifier; }

    bool boolean() const { return m_boolean; }

    double number() const { return m_number; }

    UString string() const { return m_string; }

    OperatorType op() const { return m_operator; }

    PropertyNode::Type propertyType() const { return m_propertyType; }

    void setPropertyType(PropertyNode::Type type) { m_propertyType = type; }

private:
    Type m_type;
    OperatorType m_operator;
    bool m_boolean;
    double m_number;
    UString m_string;
    Identifier m_identifier;
    PropertyNode::Type m_propertyType;
    int start;
    int end;
    WTF::Vector<Node*> m_children;
};

class Builder {
public:
    Builder(JSGlobalData* globalData, Lexer*)
        : m_globalData(globalData)
    {
    }

    static const bool CreatesAST = true;
    static const bool NeedsFreeVariableInfo = false;

    typedef Builder FunctionBodyBuilder;

    typedef Node* Arguments;
    typedef Node* ArgumentsList;
    typedef Node* Clause;
    typedef Node* ClauseList;
    typedef Node* Comma;
    typedef Node* ConstDeclList;
    typedef Node* ElementList;
    typedef Node* Expression;
    typedef Node* FormalParameterList;
    typedef Node* FunctionBody;
    typedef Node* SourceElements;
    typedef Node* Property;
    typedef Node* PropertyList;
    typedef Node* Statement;

    struct BinaryOpInfo {
        BinaryOpInfo() {}
        BinaryOpInfo(int s, int d, int e, bool r)
            : start(s)
            , divot(d)
            , end(e)
            , hasAssignment(r)
        {
        }
        BinaryOpInfo(const BinaryOpInfo& lhs, const BinaryOpInfo& rhs)
            : start(lhs.start)
            , divot(rhs.start)
            , end(rhs.end)
            , hasAssignment(lhs.hasAssignment || rhs.hasAssignment)
        {
        }
        int start;
        int divot;
        int end;
        bool hasAssignment;
    };
    typedef std::pair<Expression, BinaryOpInfo> BinaryOperand;

    struct AssignmentInfo {
        AssignmentInfo() {}
        AssignmentInfo(Expression node, int start, int divot, int initAssignments, Operator op)
            : m_node(node)
            , m_start(start)
            , m_divot(divot)
            , m_initAssignments(initAssignments)
            , m_op(op)
        {
        }
        Expression m_node;
        int m_start;
        int m_divot;
        int m_initAssignments;
        Operator m_op;
    };

    void addVar(const Identifier* ident, int attrs)
    {
        // TODO: use this to hoist the variables
    }

    void appendBinaryExpressionInfo(int& operandStackDepth, Expression current, int exprStart, int lhs, int rhs, bool hasAssignments)
    {
        operandStackDepth++;
        m_binaryOperandStack.append(std::make_pair(current, BinaryOpInfo(exprStart, lhs, rhs, hasAssignments)));
    }

    void appendBinaryOperation(int& operandStackDepth, int&, const BinaryOperand& lhs, const BinaryOperand& rhs)
    {
        operandStackDepth++;
        m_binaryOperandStack.append(std::make_pair(makeBinaryNode(m_binaryOperatorStack.last().first, lhs, rhs), BinaryOpInfo(lhs.second, rhs.second)));
    }

    void appendUnaryToken(int& tokenStackDepth, int type, int start)
    {
        tokenStackDepth++;
        m_unaryTokenStack.append(std::make_pair(type, start));
    }

    ConstDeclList appendConstDecl(ConstDeclList tail, const Identifier* name, Expression initializer)
    {
        Node* node = new (m_globalData) Node(Node::ConstDeclarationType, *name);
        node->append(initializer);
        tail->append(node);
        return tail;
    }

    void appendStatement(SourceElements sourceElements, Statement statement)
    {
        sourceElements->append(statement);
    }

    void appendToComma(Comma comma, Expression expr)
    {
        comma->append(expr);
    }

    void assignmentStackAppend(int& assignmentStackDepth, Expression node, int start, int divot, int assignmentCount, Operator op)
    {
        assignmentStackDepth++;
        m_assignmentInfoStack.append(AssignmentInfo(node, start, divot, assignmentCount, op));
    }

    Expression combineCommaNodes(Expression list, Expression init)
    {
        if (!list)
            return init;
        if (list->type() == Node::CommaType) {
            list->append(init);
            return list;
        }
        Node* node = new (m_globalData) Node(Node::CommaType);
        node->append(list);
        node->append(init);
        return node;
    }

    Arguments createArguments()
    {
        return new (m_globalData) Node(Node::ArgumentsType);
    }

    Arguments createArguments(ArgumentsList argumentsList)
    {
        return new (m_globalData) Node(Node::ArgumentsType, argumentsList);
    }

    ArgumentsList createArgumentsList(Expression expression)
    {
        return new (m_globalData) Node(Node::ArgumentsListType, expression);
    }

    ArgumentsList createArgumentsList(ArgumentsList tail, Expression expression)
    {
        tail->append(expression);
        return tail;
    }

    Expression createArray(int elisions)
    {
        // TODO: honor the elisions
        Node* node = new (m_globalData) Node(Node::ArrayType);
        return node;
    }

    Expression createArray(int elisions, ElementList elements)
    {
        // TODO: honor the elisions
        Node* node = new (m_globalData) Node(Node::ArrayType);
        node->append(elements);
        return node;
    }

    Expression createArray(ElementList elements)
    {
        Node* node = new (m_globalData) Node(Node::ArrayType);
        node->append(elements);
        return node;
    }

    Expression createAssignment(int& assignmentStackDepth, Expression rhs, int initialAssignmentCount, int currentAssignmentCount, int lastTokenEnd)
    {
        Node* node = new (m_globalData) Node(Node::AssignmentExpressionType, Node::convertOperator(m_assignmentInfoStack.last().m_op));
        node->append(m_assignmentInfoStack.last().m_node);
        node->append(rhs);
        m_assignmentInfoStack.removeLast();
        assignmentStackDepth--;
        return node;
    }

    Expression createAssignResolve(const Identifier& ident, Expression rhs, bool rhsHasAssignment, int start, int divot, int end)
    {
        Expression lhs = new (m_globalData) Node(Node::IdentifierExpressionType, ident);
        Node* node = new (m_globalData) Node(Node::AssignmentExpressionType, Node::AssignEqual);
        node->append(lhs);
        node->append(rhs);
        return node;
    }

    Statement createBlockStatement(SourceElements elements, int startLine, int endLine)
    {
        return new (m_globalData) Node(Node::BlockStatementType, elements);
    }

    Expression createBoolean(bool b)
    {
        return new (m_globalData) Node(b);
    }

    Expression createBracketAccess(Expression base, Expression property, bool propertyHasAssignments, int start, int divot, int end)
    {
        Node* node = new (m_globalData) Node(Node::BracketAccessType);
        node->append(base);
        node->append(property);
        return node;
    }

    Statement createBreakStatement(int eStart, int eEnd, int startLine, int endLine)
    {
        return new (m_globalData) Node(Node::BreakStatementType);
    }

    Statement createBreakStatement(const Identifier* ident, int eStart, int eEnd, int startLine, int endLine)
    {
        return new (m_globalData) Node(Node::BreakStatementType, *ident);
    }

    Clause createClause(Expression expr, SourceElements elements)
    {
        Node* node = new (m_globalData) Node(Node::ClauseType, expr);
        node->append(elements);
        return node;
    }

    ClauseList createClauseList(Clause clause)
    {
        return new (m_globalData) Node(Node::ClauseListType, clause);
    }

    ClauseList createClauseList(ClauseList tail, Clause clause)
    {
        tail->append(clause);
        return tail;
    }

    Comma createCommaExpr(Expression lhs, Expression rhs)
    {
        Node* node = new (m_globalData) Node(Node::CommaType);
        node->append(lhs);
        node->append(rhs);
        return node;
    }

    Expression createConditionalExpr(Expression condition, Expression lhs, Expression rhs)
    {
        Node* node = new (m_globalData) Node(Node::ConditionalExpressionType);
        node->append(condition);
        node->append(lhs);
        node->append(rhs);
        return node;
    }

    Statement createConstStatement(ConstDeclList decls, int startLine, int endLine)
    {
        return new (m_globalData) Node(Node::ConstStatementType, decls);
    }

    Statement createContinueStatement(int eStart, int eEnd, int startLine, int endLine)
    {
        return new (m_globalData) Node(Node::ContinueStatementType);
    }

    Statement createContinueStatement(const Identifier* ident, int eStart, int eEnd, int startLine, int endLine)
    {
        return new (m_globalData) Node(Node::ContinueStatementType, *ident);
    }

    Statement createDebugger(int startLine, int endLine)
    {
        return new (m_globalData) Node(Node::DebuggerType);
    }

    Expression createDotAccess(Expression base, const Identifier& property, int start, int divot, int end)
    {
        Node* node = new (m_globalData) Node(Node::DotAccessType, property);
        node->append(base);
        return node;
    }

    Statement createDoWhileStatement(Statement statement, Expression expr, int startLine, int endLine)
    {
        Node* node = new (m_globalData) Node(Node::DoWhileStatementType);
        node->append(statement);
        node->append(expr);
        return node;
    }

    ElementList createElementList(int elisions, Expression expression)
    {
        // FIXME: honor the elision
        return new (m_globalData) Node(Node::ElementListType, expression);
    }

    ElementList createElementList(ElementList tail, int elisions, Expression expression)
    {
        // FIXME: honor the elision
        tail->append(expression);
        return tail;
    }

    Statement createEmptyStatement()
    {
        return new (m_globalData) Node(Node::EmptyStatementType);
    }

    Statement createExprStatement(Expression expr, int start, int end)
    {
        Node* node = new (m_globalData) Node(Node::ExpressionStatementType);
        node->append(expr);
        return node;
    }

    Statement createForLoop(Expression initializer, Expression condition, Expression iter, Statement statements, bool b, int start, int end)
    {
        Node *node = new (m_globalData) Node(Node::ForLoopType);
        node->append(initializer);
        node->append(condition);
        node->append(iter);
        node->append(statements);
        return node;
    }

    Statement createForInLoop(const Identifier* ident, Expression initializer, Expression iter, Statement statements, int start, int divot, int end, int initStart, int initEnd, int startLine, int endLine)
    {
        Node *node = new (m_globalData) Node(Node::ForInLoopType, *ident);
        node->append(initializer);
        node->append(iter);
        node->append(statements);
        return node;
    }

    Statement createForInLoop(Expression lhs, Expression iter, Statement statements, int eStart, int eDivot, int eEnd, int start, int end)
    {
        Node *node = new (m_globalData) Node(Node::ForInLoopType);
        node->append(lhs);
        node->append(iter);
        node->append(statements);
        return node;
    }

    FormalParameterList createFormalParameterList(const Identifier& identifier)
    {
        return new (m_globalData) Node(Node::FormalParameterListType, identifier);
    }

    FormalParameterList createFormalParameterList(FormalParameterList tail, const Identifier& identifier)
    {
        Node* node = new (m_globalData) Node(Node::FormalParameterListType, identifier);
        tail->append(node);
        return node;
    }

    Statement createFuncDeclStatement(const Identifier* name, FunctionBody body, FormalParameterList parameters, int openBracePos, int closeBracePos, int bodyStartLine, int bodyEndLine)
    {
        Node* node = new (m_globalData) Node(Node::FunctionDeclStatementType, *name);
        node->append(parameters);
        node->append(body);
        return node;
    }

    FunctionBody createFunctionBody(SourceElements elements = 0)
    {
        return new (m_globalData) Node(Node::FunctionBodyType, elements);
    }

    Expression createFunctionExpr(const Identifier* name, FunctionBody body, FormalParameterList parameters, int openBracePos, int closeBracePos, int bodyStartLine, int bodyEndLine)
    {
        Node *node = new (m_globalData) Node(Node::FunctionExpressionType, *name);
        node->append(parameters);
        node->append(body);
        return node;
    }

    template <bool strict>
    Property createGetterOrSetterProperty(PropertyNode::Type type, const Identifier* name, FormalParameterList parameterList, FunctionBody, int, int, int, int)
    {
        CRASH();
        return 0;
    }

    Statement createIfStatement(Expression condition, Statement trueBlock, int start, int end)
    {
        Node *node = new (m_globalData) Node(Node::IfStatementType, condition);
        node->append(trueBlock);
        return node;
    }

    Statement createIfStatement(Expression condition, Statement trueBlock, Statement falseBlock, int start, int end)
    {
        Node *node = new (m_globalData) Node(Node::IfStatementType, condition);
        node->append(trueBlock);
        node->append(falseBlock);
        return node;
    }

    Statement createLabelStatement(const Identifier* ident, Statement statement, int start, int end)
    {
        Node* node = new (m_globalData) Node(Node::LabelStatementType, *ident);
        node->append(statement);
        return node;
    }

    Expression createLogicalNot(Expression expr)
    {
        Node* node = new (m_globalData) Node(Node::UnaryExpressionType, Node::LogicalNotOperator);
        node->append(expr);
        return node;
    }

    Expression createObjectLiteral()
    {
        return new (m_globalData) Node(Node::ObjectLiteralType);
    }

    Expression createObjectLiteral(PropertyList properties)
    {
        Node* node = new (m_globalData) Node(Node::ObjectLiteralType);
        node->append(properties);
        return node;
    }

    Expression createNewExpr(Expression expr, Arguments arguments, int start, int divot, int end)
    {
        Node* node = new (m_globalData) Node(Node::NewExpressionType, expr);
        node->append(arguments);
        return node;
    }

    Expression createNewExpr(Expression expr, int start, int end)
    {
        return new (m_globalData) Node(Node::ExpressionType, expr);
    }

    Expression createNull()
    {
        return new (m_globalData) Node(Node::NullType);
    }

    Expression createNumberExpr(double d)
    {
        return new (m_globalData) Node(d);
    }

    template <bool complete>
    Property createProperty(const Identifier* name, Expression expr, PropertyNode::Type type)
    {
        Node* node = new (m_globalData) Node(Node::PropertyType, *name);
        node->setPropertyType(type);
        node->append(expr);
        return node;
    }

    template <bool complete>
    Property createProperty(JSGlobalData* globalData, double name, Expression expr, PropertyNode::Type type)
    {
        Identifier id(m_globalData->parser->arena().identifierArena().makeNumericIdentifier(m_globalData, name));
        Node* node = new (m_globalData) Node(Node::PropertyType, id);
        node->setPropertyType(type);
        node->append(expr);
        return node;
    }

    PropertyList createPropertyList(Property property)
    {
        return new (m_globalData) Node(Node::PropertyListType, property);
    }

    PropertyList createPropertyList(Property property, PropertyList tail)
    {
        tail->append(property);
        return tail;
    }

    Expression createRegex(const Identifier& pattern, const Identifier& flags, int start)
    {
        return new (m_globalData) Node(Node::RegexType, pattern, flags);
    }

    Expression createResolve(const Identifier* ident, int start)
    {
        // TODO: check for global arguments
        return new (m_globalData) Node(Node::ResolveType, *ident);
    }

    Statement createReturnStatement(Expression expression, int eStart, int eEnd, int startLine, int endLine)
    {
        return new (m_globalData) Node(Node::ReturnStatementType, expression);
    }

    SourceElements createSourceElements()
    {
        return new (m_globalData) Node(Node::SourceElementsType);
    }

    Expression createString(const Identifier* string)
    {
        return new (m_globalData) Node(string->ustring());
    }

    Statement createSwitchStatement(Expression expr, ClauseList firstClauses, Clause defaultClause, ClauseList secondClauses, int startLine, int endLine)
    {
        Node* node = new (m_globalData) Node(Node::SwitchStatementType, expr);
        node->append(firstClauses);
        node->append(defaultClause);
        node->append(secondClauses);
        return node;
    }

    Statement createThrowStatement(Expression expr, int start, int end, int startLine, int endLine)
    {
        return new (m_globalData) Node(Node::ThrowStatementType, expr);
    }

    Statement createTryStatement(Statement tryBlock, const Identifier* ident, bool catchHasEval, Statement catchBlock, Statement finallyBlock, int startLine, int endLine)
    {
        Node* node = new (m_globalData) Node(Node::TryStatementType, *ident);
        node->append(tryBlock);
        node->append(catchBlock);
        node->append(finallyBlock);
        return node;
    }

    Expression createUnaryPlus(Expression expr)
    {
        Node* node = new (m_globalData) Node(Node::UnaryExpressionType, Node::AddOperator);
        node->append(expr);
        return node;
    }

    Statement createVarStatement(Expression expr, int start, int end)
    {
        return new (m_globalData) Node(Node::VariableDeclarationType, expr);
    }

    Statement createWhileStatement(Expression expr, Statement statement, int startLine, int endLine)
    {
        Node* node = new (m_globalData) Node(Node::WhileStatementType, expr);
        node->append(statement);
        return node;
    }

    Statement createWithStatement(Expression expr, Statement statement, int start, int end, int startLine, int endLine)
    {
        Node* node = new (m_globalData) Node(Node::WithStatementType, expr);
        node->append(statement);
        return node;
    }

    Expression createVarIdentifier(const Identifier* ident)
    {
        return new (m_globalData) Node(Node::IdentifierExpressionType, *ident);
    }

    Expression createVoid(Expression expr)
    {
        return new (m_globalData) Node(Node::VoidType);
    }

    // JSParser uses this to find out if there is eval inside a try statement.
    // We don't care for the optimization so we just return a constant.
    int evalCount() const
    {
        return 1;
    }

    const BinaryOperand& getFromOperandStack(int i)
    {
        return m_binaryOperandStack[m_binaryOperandStack.size() + i];
    }

    const Identifier& getName(const Property& property)
    {
        return property->identifier();
    }

    PropertyNode::Type getType(const Property& property)
    {
        return property->propertyType();
    }

    Expression makeBinaryNode(int token, pair<Expression, BinaryOpInfo> lhs, pair<Expression, BinaryOpInfo> rhs)
    {
        Node* node = new (m_globalData) Node(Node::BinaryExpressionType, Node::convertOperator(token));
        node->append(lhs.first);
        node->append(rhs.first);
        return node;
    }

    Expression makeBitwiseNotNode(Expression expr)
    {
        Node* node = new (m_globalData) Node(Node::UnaryExpressionType, Node::BitwiseNotOperator);
        node->append(expr);
        return node;
    }

    Expression makeDeleteNode(Expression expr, int start, int divot, int end)
    {
        Node* node = new (m_globalData) Node(Node::UnaryExpressionType, Node::DeleteOperator);
        node->append(expr);
        return node;
    }

    Expression makeFunctionCallNode(Expression func, Arguments args, int start, int divot, int end)
    {
        Node* node = new (m_globalData) Node(Node::FunctionCallType);
        node->append(func);
        node->append(args);
        return node;
    }

    Expression makeNegateNode(Expression expr)
    {
        Node* node = new (m_globalData) Node(Node::UnaryExpressionType, Node::SubtractOperator);
        node->append(expr);
        return node;
    }

    Expression makePostfixNode(Expression expr, Operator op, int start, int divot, int end)
    {
        Node* node = new (m_globalData) Node(Node::PostfixType, Node::convertOperator(op));
        node->append(expr);
        return node;
    }

    Expression makePrefixNode(Expression expr, Operator op, int start, int divot, int end)
    {
        Node* node = new (m_globalData) Node(Node::PrefixType, Node::convertOperator(op));
        node->append(expr);
        return node;
    }

    Expression makeTypeOfNode(Expression expr)
    {
        Node* node = new (m_globalData) Node(Node::UnaryExpressionType, Node::TypeofOperator);
        node->append(expr);
        return node;
    }

    bool operatorStackHasHigherPrecedence(int&, int precedence)
    {
        return precedence <= m_binaryOperatorStack.last().second;
    }

    void operatorStackAppend(int& operatorStackDepth, int op, int precedence)
    {
        operatorStackDepth++;
        m_binaryOperatorStack.append(std::make_pair(op, precedence));
    }

    void operatorStackPop(int& operatorStackDepth)
    {
        operatorStackDepth--;
        m_binaryOperatorStack.removeLast();
    }

    Expression popOperandStack(int&)
    {
        Expression result = m_binaryOperandStack.last().first;
        m_binaryOperandStack.removeLast();
        return result;
    }

    void setUsesArguments(FunctionBody function)
    {
    }

    void shrinkOperandStackBy(int& operandStackDepth, int amount)
    {
        operandStackDepth -= amount;
        ASSERT(operandStackDepth >= 0);
        m_binaryOperandStack.resize(m_binaryOperandStack.size() - amount);
    }

    Expression thisExpr()
    {
        return new (m_globalData) Node(Node::ThisType);
    }

    int unaryTokenStackLastStart(int&)
    {
        return m_unaryTokenStack.last().second;
    }

    int unaryTokenStackLastType(int&)
    {
        return m_unaryTokenStack.last().first;
    }

    void unaryTokenStackRemoveLast(int& tokenStackDepth)
    {
        tokenStackDepth--;
        m_unaryTokenStack.removeLast();
    }

private:
    JSGlobalData* m_globalData;
    Vector<AssignmentInfo, 10> m_assignmentInfoStack;
    Vector<BinaryOperand, 10> m_binaryOperandStack;
    Vector<pair<int, int>, 10> m_binaryOperatorStack;
    Vector<pair<int, int>, 10> m_unaryTokenStack;
};

} // namespace SyntaxTree

}

#endif // SyntaxTree_h
