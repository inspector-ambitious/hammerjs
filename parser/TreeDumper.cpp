/*
  Copyright (C) 2010 Ariya Hidayat <ariya.hidayat@gmail.com>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "TreeDumper.h"

#include <Nodes.h>
#include <SyntaxTree.h>

namespace JSC {

static const char* operatorAsText(SyntaxTree::Node::OperatorType op)
{
    switch (op) {
    case SyntaxTree::Node::TypeofOperator: return "typeof"; break;
    case SyntaxTree::Node::DeleteOperator: return "delete"; break;
    case SyntaxTree::Node::LogicalNotOperator: return "!"; break;
    case SyntaxTree::Node::LogicalOrOperator: return "||"; break;
    case SyntaxTree::Node::LogicalAndOperator: return "&&"; break;
    case SyntaxTree::Node::BitwiseNotOperator: return "!"; break;
    case SyntaxTree::Node::BitwiseOrOperator: return "|"; break;
    case SyntaxTree::Node::BitwiseXorOperator: return "^"; break;
    case SyntaxTree::Node::BitwiseAndOperator: return "&"; break;
    case SyntaxTree::Node::EqualOperator: return "=="; break;
    case SyntaxTree::Node::NotEqualOperator: return "!="; break;
    case SyntaxTree::Node::StringEqualOperator: return "==="; break;
    case SyntaxTree::Node::StringNotEqualOperator: return "!=="; break;
    case SyntaxTree::Node::LessThanOperator: return "<"; break;
    case SyntaxTree::Node::GreaterThanOperator: return ">"; break;
    case SyntaxTree::Node::LessThanOrEqualOperator: return ">="; break;
    case SyntaxTree::Node::GreaterThanOrEqualOperator: return "<="; break;
    case SyntaxTree::Node::InstanceofOperator: return "instance of";
    case SyntaxTree::Node::IntokenOperator: return "in";
    case SyntaxTree::Node::LeftShiftOperator: return "<<";
    case SyntaxTree::Node::RightShiftOperator: return ">>";
    case SyntaxTree::Node::ZeroFillRightShiftOperator: return ">>>";
    case SyntaxTree::Node::AddOperator: return "+";
    case SyntaxTree::Node::SubtractOperator: return "-";
    case SyntaxTree::Node::MultiplyOperator: return "*";
    case SyntaxTree::Node::DivideOperator: return "/";
    case SyntaxTree::Node::ModulusOperator: return "%";
    case SyntaxTree::Node::PlusPlusOperator: return "++";
    case SyntaxTree::Node::MinusMinusOperator: return "--";
    case SyntaxTree::Node::AssignEqual: return "=";
    case SyntaxTree::Node::AssignAdd: return "+=";
    case SyntaxTree::Node::AssignSubtract: return "-=";
    case SyntaxTree::Node::AssignMultiply: return "*=";
    case SyntaxTree::Node::AssignDivide: return "/=";
    case SyntaxTree::Node::AssignModulus: return "%=";
    case SyntaxTree::Node::AssignLeftShift: return ">>=";
    case SyntaxTree::Node::AssignRightShift: return "<<=";
    case SyntaxTree::Node::AssignZeroFillRightShift: return ">>>=";
    case SyntaxTree::Node::AssignAnd: return "&=";
    case SyntaxTree::Node::AssignXor: return "^=";
    case SyntaxTree::Node::AssignOr: return "|=";
    default: CRASH(); break;
    }
    return 0;
}

JSONTreeDumper::JSONTreeDumper()
    : indent(0)
{
}

void JSONTreeDumper::start()
{
    tmpnam(fileName);
    buffer = fopen(fileName, "wt");
}

void JSONTreeDumper::finish()
{
    fclose(buffer);

    buffer  = fopen(fileName, "rt");
    fseek(buffer, 0, SEEK_END);
    int len = ftell(buffer);
    rewind(buffer);
    char* data = new char[len + 1];
    fread(data, 1, len, buffer);
    data[len] = '\0';
    fclose(buffer);

    tree = UString(data, len);
    delete [] data;
}

void JSONTreeDumper::printSpaces(int indent)
{
    for (int i = 0; i < indent; ++i)
        fprintf(buffer, "    ");
}

void JSONTreeDumper::printString(const UString &str)
{
    for (unsigned c = 0; c < str.length(); ++c) {
        switch (str[c]) {
        case '"':
            fprintf(buffer, "\\\"");
            break;
        case '\\':
            fprintf(buffer, "\\\\");
            break;
        case '\b':
            fprintf(buffer, "\\b");
            break;
        case '\f':
            fprintf(buffer, "\\f");
            break;
        case '\n':
            fprintf(buffer, "\\n");
            break;
        case '\r':
            fprintf(buffer, "\\r");
            break;
        case '\v':
            fprintf(buffer, "\\\\v");
            break;
        case '\t':
            fprintf(buffer, "\\t");
            break;
        default:
            fprintf(buffer, "%c", str[c]);
        }
    }
}

void JSONTreeDumper::visitChild(SyntaxTree::Node* n, int index, const char* name)
{
    printSpaces(indent);
    SyntaxTree::Node* node = (index < n->childCount()) ? n->childAt(index) : 0;
    if (!node) {
        fprintf(buffer, "\"%s\": null", name);
    } else {
        if (name)
            fprintf(buffer, "\"%s\": {\n", name);
        ++indent;
        node->apply(this);
        --indent;
        if (name) {
            printSpaces(indent);
            fprintf(buffer, "}");
        }
    }
}

void JSONTreeDumper::visitAllChildren(SyntaxTree::Node* n, bool showIndex)
{
    fprintf(buffer, "[");
    for (int index = 0; index < n->childCount(); ++index) {
        if (index > 0) {
            fprintf(buffer, "\n");
            printSpaces(indent);
        }
        fprintf(buffer, "{\n");
        ++indent;
        n->childAt(index)->apply(this);
        --indent;
        printSpaces(indent);
        fprintf(buffer, "}");
        if (index < n->childCount() - 1)
            fprintf(buffer, ",");
    }
    fprintf(buffer, "]");
}

void JSONTreeDumper::process(SyntaxTree::Node* n)
{
    if (n->type() == SyntaxTree::Node::ArgumentsType) {
        if (n->childCount() && n->childAt(0))
            n->childAt(0)->apply(this);
        else
            fprintf(buffer, "[]");
        return;
    }

    if (n->type() == SyntaxTree::Node::ArgumentsListType) {
        visitAllChildren(n);
        return;
    }

    if (n->type() == SyntaxTree::Node::ArrayType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ArrayExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"elements\": ");
        if (n->childCount() && n->childAt(0))
            n->childAt(0)->apply(this);
        else
            fprintf(buffer, "[]");
        fprintf(buffer, "\n");
        return;
    }

    // FIXME: should be variable init inside declaration?
    if (n->type() == SyntaxTree::Node::AssignmentExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"AssignmentExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"operator\": \"%s\",\n", operatorAsText(n->op()));
        visitChild(n, 0, "left");
        fprintf(buffer, ",\n");
        visitChild(n, 1, "right");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::BinaryExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"BinaryExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"operator\": \"%s\",\n", operatorAsText(n->op()));
        visitChild(n, 0, "left");
        fprintf(buffer, ",\n");
        visitChild(n, 1, "right");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::BlockStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"BlockStatement\",\n");
        printSpaces(indent);
        if (n->childCount() == 1)
            if (n->childAt(0))
                if (n->childAt(0)->type() == SyntaxTree::Node::SourceElementsType) {
                    fprintf(buffer, "\"body\": ");
                    visitAllChildren(n->childAt(0));
                    fprintf(buffer, "\n");
                    return;
                }
        fprintf(buffer, "\"body\": []\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::BooleanExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"Literal\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"objtype\": \"Boolean\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"value\": %s\n", n->boolean() ? "true" : "false");
        return;
    }

    if (n->type() == SyntaxTree::Node::BracketAccessType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"MemberExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"accesstype\": \"Bracket\",\n");
        visitChild(n, 0, "object");
        fprintf(buffer, ",\n");
        visitChild(n, 1, "property");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::BreakStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"BreakStatement\",\n");
        printSpaces(indent);
        if (n->identifier().ustring().isEmpty()) {
            fprintf(buffer, "\"label\": null\n");
        } else {
            fprintf(buffer, "\"label\": ");
            printString(n->identifier().ustring());
            fprintf(buffer, "\n");
        }
        return;
    }

    if (n->type() == SyntaxTree::Node::ClauseListType) {
        if (n->childCount())
            visitAllChildren(n);
        else
            fprintf(buffer, "[]");
        return;
    }

    if (n->type() == SyntaxTree::Node::ClauseType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"SwitchCase\",\n");
        visitChild(n, 0, "test");
        fprintf(buffer, ",\n");
        printSpaces(indent);
        if (n->childAt(1)) {
            fprintf(buffer, "\"consequent\": [{\n");
            n->childAt(1)->apply(this);
            printSpaces(indent);
            fprintf(buffer, "}]");
        } else {
            fprintf(buffer, "\"consequent\": []");
        }
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::CommaType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"SequenceExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"expressions\": ");
        visitAllChildren(n);
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::ContinueStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ContinueStatement\",\n");
        printSpaces(indent);
        if (n->identifier().ustring().isEmpty()) {
            fprintf(buffer, "\"label\": null\n");
        } else {
            fprintf(buffer, "\"label\": ");
            printString(n->identifier().ustring());
            fprintf(buffer, "\n");
        }
        return;
    }

    if (n->type() == SyntaxTree::Node::ConditionalExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ConditionalExpression\",\n");
        visitChild(n, 0, "test");
        fprintf(buffer, ",\n");
        visitChild(n, 1, "consequent");
        fprintf(buffer, ",\n");
        visitChild(n, 2, "alternate");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::DotAccessType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"MemberExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"accesstype\": \"Dot\",\n");
        visitChild(n, 0, "object");
        fprintf(buffer, ",\n");
        printSpaces(indent);
        fprintf(buffer, "\"property\": {\n");
        printSpaces(indent + 1);
        fprintf(buffer, "\"type\": \"Identifier\",\n");
        printSpaces(indent + 1);
        fprintf(buffer, "\"name\": \"");
        printString(n->identifier().ustring());
        fprintf(buffer, "\"\n");
        printSpaces(indent);
        fprintf(buffer, "}\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::DoWhileStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"DoWhileStatement\",\n");
        visitChild(n, 0, "body");
        fprintf(buffer, ",\n");
        visitChild(n, 1, "test");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::ElementListType) {
        visitAllChildren(n);
        return;
    }

    if (n->type() == SyntaxTree::Node::EmptyStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"EmptyStatement\"\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::ExpressionStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ExpressionStatement\",\n");
        visitChild(n, 0, "expression");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::ExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ExpressionStatement\",\n");
        visitChild(n, 0, "expression");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::FunctionCallType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"CallExpression\",\n");
        visitChild(n, 0, "callee");
        fprintf(buffer, ",\n");
        printSpaces(indent);
        fprintf(buffer, "\"arguments\": ");
        n->childAt(1)->apply(this);
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::ForLoopType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ForStatement\",\n");
        visitChild(n, 0, "init");
        fprintf(buffer, ",\n");
        visitChild(n, 1, "test");
        fprintf(buffer, ",\n");
        visitChild(n, 2, "update");
        fprintf(buffer, ",\n");
        visitChild(n, 3, "body");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::ForInLoopType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ForInStatement\",\n");
        if (n->childAt(0)) {
            // FIXME
        } else {
            printSpaces(indent);
            fprintf(buffer, "\"left\": {\n");
            printSpaces(indent + 1);
            fprintf(buffer, "\"type\": \"VariableDeclaration\",\n");
            printSpaces(indent + 1);
            fprintf(buffer, "\"declarations\": [{\n");
            printSpaces(indent + 2);
            fprintf(buffer, "\"type\": \"VariableDeclarator\",\n");
            printSpaces(indent + 2);
            fprintf(buffer, "\"id\": {\n");
            printSpaces(indent + 3);
            fprintf(buffer, "\"type\": \"Identifier\",\n");
            printSpaces(indent + 3);
            fprintf(buffer, "\"name\": \"");
            printString(n->identifier().ustring());
            fprintf(buffer, "\"\n");
            printSpaces(indent + 2);
            fprintf(buffer, "},\n");
            printSpaces(indent + 2);
            fprintf(buffer, "\"init\": null\n");
            printSpaces(indent + 1);
            fprintf(buffer, "}]\n");
            printSpaces(indent);
            fprintf(buffer, "},\n");
        }
        visitChild(n, 1, "right");
        fprintf(buffer, ",\n");
        visitChild(n, 2, "body");
        fprintf(buffer, ",\n");
        printSpaces(indent);
        fprintf(buffer, "\"each\": false\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::FormalParameterListType) {
        fprintf(buffer, "{\n");
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"Identifier\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"name\": \"");
        printString(n->identifier().ustring());
        fprintf(buffer, "\"\n");
        printSpaces(indent - 1);
        if (n->childCount()) {
            fprintf(buffer, "},\n");
            printSpaces(indent - 1);
            for (int i = 0; i < n->childCount(); ++i)
                n->childAt(i)->apply(this);
        } else {
            fprintf(buffer, "}");
        }
        return;
    }

    if (n->type() == SyntaxTree::Node::FunctionDeclStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"FunctionExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"id\": \"");
        printString(n->identifier().ustring());
        fprintf(buffer, "\",\n");
        printSpaces(indent);
        if (n->childAt(0)) {
            fprintf(buffer, "\"params\": [");
            indent++;
            n->childAt(0)->apply(this);
            indent--;
            fprintf(buffer, "],\n");
        } else {
            fprintf(buffer, "\"params\": [],\n");
        }
        printSpaces(indent);
        visitChild(n, 1, "body");
        return;
    }

    if (n->type() == SyntaxTree::Node::FunctionBodyType) {
        if (n->childCount() && n->childAt(0)) {
            n->childAt(0)->apply(this);
        } else {
            printSpaces(indent + 1);
            fprintf(buffer, "\"type\": \"BlockStatement\",\n");
            printSpaces(indent + 1);
            fprintf(buffer, "\"body\": []\n");
        }
        return;
    }

    if (n->type() == SyntaxTree::Node::FunctionExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"FunctionExpression\",\n");
        printSpaces(indent);
        if (n->identifier().ustring().isEmpty()) {
            fprintf(buffer, "\"id\": null,\n");
        } else {
            fprintf(buffer, "\"id\": \"");
            printString(n->identifier().ustring());
            fprintf(buffer, "\",\n");
        }
        if (n->childCount() == 0) {
            printSpaces(indent);
            fprintf(buffer, "\"params\": [],\n");
            fprintf(buffer, "\"body\": {}\n");
        } else if (n->childCount() == 1) {
            printSpaces(indent);
            fprintf(buffer, "\"params\": [],\n");
            visitChild(n, 0, "body");
            fprintf(buffer, "\n");
        } else {
            printSpaces(indent);
            if (n->childAt(0)) {
                fprintf(buffer, "\"params\": [");
                indent++;
                n->childAt(0)->apply(this);
                indent--;
                fprintf(buffer, "],\n");
            } else {
                fprintf(buffer, "\"params\": [],\n");
            }
            visitChild(n, 1, "body");
            fprintf(buffer, "\n");
        }
        return;
    }

    if (n->type() == SyntaxTree::Node::IdentifierExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"Identifier\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"name\": \"");
        printString(n->identifier().ustring());
        fprintf(buffer, "\"\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::IfStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"IfStatement\",\n");
        visitChild(n, 0, "test");
        fprintf(buffer, ",\n");
        visitChild(n, 1, "consequent");
        fprintf(buffer, ",\n");
        visitChild(n, 2, "alternate");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::NewExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"NewExpression\",\n");
        visitChild(n, 0, "callee");
        fprintf(buffer, ",\n");
        printSpaces(indent);
        fprintf(buffer, "\"arguments\": ");
        n->childAt(1)->apply(this);
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::NullType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"Literal\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"objtype\": \"Null\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"value\": null\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::NumberExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"Literal\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"objtype\": \"Number\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"value\": \"%g\"\n", n->number());
        return;
    }

    if (n->type() == SyntaxTree::Node::ObjectLiteralType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ObjectExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"properties\": ");
        if (n->childCount() && n->childAt(0)->type() == SyntaxTree::Node::PropertyListType) {
            visitAllChildren(n->childAt(0));
        } else {
            visitAllChildren(n);
        }
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::PropertyType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"Property\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"key\": {\n");
        printSpaces(indent + 1);
        fprintf(buffer, "\"type\": \"Identifier\",\n");
        printSpaces(indent + 1);
        fprintf(buffer, "\"name\": \"");
        printString(n->identifier().ustring());
        fprintf(buffer, "\"\n");
        printSpaces(indent);
        fprintf(buffer, "},\n");
        visitChild(n, 0, "value");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::SourceElementsType) {
        if (indent == 0)
            fprintf(buffer, "{\n");
        indent++;
        printSpaces(indent);
        if (indent == 1)
            fprintf(buffer, "\"type\": \"Program\",\n");
        else
            fprintf(buffer, "\"type\": \"BlockStatement\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"body\": ");
        visitAllChildren(n);
        indent--;
        fprintf(buffer, "\n");
        if (indent == 0)
            fprintf(buffer, "}\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::StringExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"Literal\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"objtype\": \"String\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"value\": \"");
        printString(n->string());
        fprintf(buffer, "\"\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::ThisType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ThisExpression\"\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::UnaryExpressionType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"UnaryExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"operator\": \"%s\",\n", operatorAsText(n->op()));
        visitChild(n, 0, "argument");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::PostfixType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"UpdateExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"operator\": \"%s\",\n", operatorAsText(n->op()));
        visitChild(n, 0, "argument");
        fprintf(buffer, ",\n");
        printSpaces(indent);
        fprintf(buffer, "\"prefix\": false\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::PrefixType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"UpdateExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"operator\": \"%s\",\n", operatorAsText(n->op()));
        visitChild(n, 0, "argument");
        fprintf(buffer, ",\n");
        printSpaces(indent);
        fprintf(buffer, "\"prefix\": true\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::RegexType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"Literal\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"objtype\": \"RegEx\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"value\": ");
        fprintf(buffer, "\"/");
        printString(n->identifier().ustring());
        fprintf(buffer, "/");
        printString(n->string());
        fprintf(buffer, "\"\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::ResolveType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"Identifier\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"name\": \"");
        printString(n->identifier().ustring());
        fprintf(buffer, "\"\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::ReturnStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ReturnStatement\",\n");
        if (n->childCount() && n->childAt(0)) {
            visitChild(n, 0, "argument");
        } else {
            printSpaces(indent);
            fprintf(buffer, "\"argument\": null\n");
        }
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::SwitchStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"SwitchStatement\",\n");
        visitChild(n, 0, "discriminant");
        fprintf(buffer, ",\n");
        printSpaces(indent);

        if (n->childAt(1)) {
            // fold 'default' clause into another
            SyntaxTree::Node clauses(*n->childAt(1));
            if (n->childAt(2))
                clauses.append(n->childAt(2));
            fprintf(buffer, "\"cases\": ");
            clauses.apply(this);
            fprintf(buffer, "\n");
        } else {
            if (n->childAt(2)) {
                fprintf(buffer, "\"cases\": [{\n");
                ++indent;
                n->childAt(2)->apply(this);
                --indent;
                printSpaces(indent);
                fprintf(buffer, "}]\n");
            }
        }
        return;
    }

    if (n->type() == SyntaxTree::Node::ThrowStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"ThrowStatement\",\n");
        if (n->childCount() && n->childAt(0)) {
            visitChild(n, 0, "argument");
        } else {
            printSpaces(indent);
            fprintf(buffer, "\"argument\": null\n");
        }
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::TryStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"TryStatement\",\n");
        visitChild(n, 0, "block");
        fprintf(buffer, ",\n");
        visitChild(n, 1, "handler");
        fprintf(buffer, ",\n");
        visitChild(n, 2, "finalizer");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::VoidType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"UnaryExpression\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"operator\": \"void\",\n");
        visitChild(n, 0, "argument");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::WhileStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"WhileStatement\",\n");
        visitChild(n, 0, "test");
        fprintf(buffer, ",\n");
        visitChild(n, 1, "body");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::WithStatementType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"WithStatement\",\n");
        visitChild(n, 0, "object");
        fprintf(buffer, ",\n");
        visitChild(n, 1, "body");
        fprintf(buffer, "\n");
        return;
    }

    if (n->type() == SyntaxTree::Node::VariableDeclarationType) {
        printSpaces(indent);
        fprintf(buffer, "\"type\": \"VariableDeclaration\",\n");
        printSpaces(indent);
        fprintf(buffer, "\"declarations\": ");
        visitAllChildren(n);
        fprintf(buffer, "\n");
        return;
    }

    printSpaces(indent);
    fprintf(buffer, "\"type\": \"Unknown %d\"\n", n->type());
    exit(0);
    return;
}

} // namespace JSC
