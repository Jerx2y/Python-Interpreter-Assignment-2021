#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include <vector>
#include "Python3BaseVisitor.h"
#include "Scope.h"
#include "Exception.h"
#include "utils.h"
#include "BaseType.h"

#include <iostream>
#include <stack>
#include <unordered_map>

using std::cin;
using std::cout;
using std::endl;

class EvalVisitor: public Python3BaseVisitor {

public:

    struct Func {
        Scope scope;
        Python3Parser::SuiteContext *suite;
        std::vector<std::string> testlist;
        Func() : scope() { suite = nullptr; }
    };

    std::stack<Scope> Stack;
    std::unordered_map<std::string, Func> Function;

    EvalVisitor() : Python3BaseVisitor() {
        while (!Stack.empty())
            Stack.pop();
        Stack.push(Scope());
    }

    virtual antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
        auto funcName = ctx->NAME()->getText();
        auto varList = visitParameters(ctx->parameters()).as<std::pair<std::vector<std::string>, std::vector<BaseType> > >();
        const auto &varName = varList.first;
        const auto &varData = varList.second;
        Func now;
        now.testlist = varName;
        for (int i = varName.size() - 1, j = varData.size() - 1; j >= 0; --i, --j)
            now.scope.varRegister(varName[i], varData[j]);
        now.suite = ctx->suite(); // TODO
        Function[funcName] = now;
        return 0;
    } //funcdef: 'def' NAME parameters ':' suite;

    virtual antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx) override {
        if (ctx->typedargslist())
            return visitTypedargslist(ctx->typedargslist());
        return std::make_pair(std::vector<std::string>(), std::vector<BaseType>());
    } // parameters: '(' typedargslist? ')';

    virtual antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
        std::vector<std::string> name;
        std::vector<BaseType> test;
        auto tfpdef = ctx->tfpdef();
        for (auto x : tfpdef)
            name.push_back(visitTfpdef(x).as<std::string>());
        auto testArray = ctx->test();
        for (auto x : testArray)
            test.push_back(visitTest(x).as<BaseType>());
        return std::make_pair(name, test);
    } // typedargslist: (tfpdef ('=' test)? (',' tfpdef ('=' test)?)*);

    virtual antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
        return ctx->NAME()->getText();
    }

    virtual antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx) override {
        if (ctx->simple_stmt())
            return visitSimple_stmt(ctx->simple_stmt());
        else return visitCompound_stmt(ctx->compound_stmt());
    }

    virtual antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override {
        return visitSmall_stmt(ctx->small_stmt());
    }

    virtual antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override {
        if (ctx->flow_stmt()) return visitFlow_stmt(ctx->flow_stmt());
        else return visitExpr_stmt(ctx->expr_stmt());
    }

    void getAugassign(BaseType &lhs, const BaseType &rhs, const std::string &opt) {
        if (opt == "+=") lhs = lhs + rhs;
        if (opt == "-=") lhs = lhs - rhs;
        if (opt == "*=") lhs = mul(lhs, rhs);
        if (opt == "/=") lhs = ddiv(lhs, rhs);
        if (opt == "//=") lhs = idiv(lhs, rhs);
        if (opt == "%=") lhs = mod(lhs, rhs);
    }

    virtual antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {

        auto testlistArray = ctx->testlist();
        int arraySize = testlistArray.size();
        auto varData = visitTestlist(testlistArray[arraySize - 1]).as<std::vector<BaseType> >();

        if (ctx->augassign()) {
            auto varName = testlistArray[0]->getText();
            varName += ',';
            string name; name.clear();
            for (int j = 0, k = 0, nameSize = varName.size(); j < nameSize; ++j) {
                if (varName[j] == ',') {
                    BaseType tmp = Stack.top().varQuery(name).second;
                    getAugassign(tmp, varData[k++], visitAugassign(ctx->augassign()).as<std::string>());
                    Stack.top().varRegister(name, tmp);
                    name.clear();
                } else name += varName[j];
            }
            return 0;
        }

        for (int i = 0; i < arraySize - 1; ++i) {
            auto varName = testlistArray[i]->getText();
            varName += ',';
            string name; name.clear();
            for (int j = 0, k = 0, nameSize = varName.size(); j < nameSize; ++j) {
                if (varName[j] == ',') {
                    Stack.top().varRegister(name, varData[k++]);
                    name.clear();
                } else name += varName[j];
            }
        }

        if (arraySize > 1) return 0;
        else return varData;
    }

    virtual antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx) override {
        return ctx->getText();
    }

    virtual antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override {
        if (ctx->break_stmt()) return -1;
        if (ctx->continue_stmt()) return -2;
        if (ctx->return_stmt()) return visitReturn_stmt(ctx->return_stmt());
    }

    virtual antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
        if (ctx->testlist()) return visitTestlist(ctx->testlist());
        else return BaseType(bool(0));
    }

    virtual antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
        auto test = ctx->test();
        auto suite = ctx->suite();
        auto testSize = test.size();
        for (int i = 0; i < testSize; ++i)
            if ((bool) visitTest(test[i]).as<BaseType>()) {
                auto tmp = visitSuite(suite[i]);
                if (tmp.is<int>()) return tmp;
                else if (tmp.is<std::vector<BaseType> >())
                    return tmp;
                else return tmp;
            }
        if (testSize != suite.size()) { 
            auto tmp = visitSuite(suite[testSize]);
            if (tmp.is<int>()) return tmp;
            else if (tmp.is<std::vector<BaseType> >())
                return tmp;
            else return tmp;
        }
        return 0;
    }

    virtual antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
        while ((bool)visitTest(ctx->test()).as<BaseType>()) {
            auto tmp = visitSuite(ctx->suite());
            if (tmp.is<int>()) {
                if (tmp.as<int>()) return tmp;
            } else if (tmp.is<std::vector<BaseType> >())
                return tmp;
            else if (tmp.as<BaseType>() != BaseType()) return tmp;
        }
        return 0;
    }

    virtual antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx) override {
        if (ctx->simple_stmt())
            return visitSimple_stmt(ctx->simple_stmt());
        auto stmt = ctx->stmt();
        for (auto x : stmt) {
            auto tmp = visitStmt(x);
            if (tmp.is<int>()) {
                if (tmp.as<int>() == -1) return 1;
                if (tmp.as<int>() == -2) return 0;
            } else if (tmp.is<std::vector<BaseType>>())
                return tmp;
            else if (tmp.as<BaseType>() != BaseType()) 
                return tmp;
        }
        return 0;
    }

    virtual antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) override {
        return visitOr_test(ctx->or_test());
    }

    virtual antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx) override {
        auto tmp = ctx->and_test();
        if (tmp.size() == 1)
            return visitAnd_test(tmp[0]);
        for (auto x : tmp)
            if ((bool) visitAnd_test(x).as<BaseType>())
                return BaseType(true);
        return BaseType(false);
    }

    virtual antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx) override {
        auto tmp = ctx->not_test();
        if (tmp.size() == 1)
            return visitNot_test(tmp[0]);
        for (auto x : tmp)
            if (! (bool) visitNot_test(x).as<BaseType>()) 
                return BaseType(false);
        return BaseType(true);
    }

    virtual antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx) override {
        if (ctx->NOT()) return BaseType(!(bool)visitNot_test(ctx->not_test()).as<BaseType>());
        else return visitComparison(ctx->comparison());
    }

    bool mycmp(const BaseType &lhs, const BaseType &rhs, const string &opt) {
        if (opt == "<") return lhs < rhs;
        if (opt == ">") return lhs > rhs;
        if (opt == "==") return lhs == rhs;
        if (opt == ">=") return lhs >= rhs;
        if (opt == "<=") return lhs <= rhs;
        if (opt == "!=") return lhs != rhs;
        // '<'|'>'|'=='|'>='|'<=' | '!='
    }

    virtual antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *ctx) override {
        auto vec = ctx->arith_expr();
        auto last = visitArith_expr(vec[0]);
        auto szv = vec.size();
        if (szv == 1) return last;
        auto opt = ctx->comp_op();
        for (int i = 1; i < szv; ++i) {
            auto now = visitArith_expr(vec[i]);
            if (!mycmp(last.as<BaseType>(), now.as<BaseType>(), visitComp_op(opt[i - 1]).as<string>()))
                return BaseType(false);
            last = now;
        }
        return BaseType(true);
    }

    virtual antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) override {
        return ctx->getText();
    }

    virtual antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
        auto t = ctx->term();
        auto szt = t.size();
        if (szt == 1) return visitTerm(t[0]);
        BaseType res = visitTerm(t[0]).as<BaseType>();
        auto o = ctx->addorsub_op();
        for (int i = 1; i < szt; ++i) {
            if ((visitAddorsub_op(o[i - 1])).as<bool>())
                res = res + visitTerm(t[i]).as<BaseType>();
            else res = res - visitTerm(t[i]).as<BaseType>();
        }
        return res;
    }

    virtual antlrcpp::Any visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) override {
        return bool(ctx->getText() == "+");
    }

    virtual antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx) override {
        auto f = ctx->factor();
        auto szf = f.size();
        if (szf == 1) return visitFactor(f[0]); 
        BaseType res = visitFactor(f[0]).as<BaseType>();
        auto o = ctx->muldivmod_op();
        for (int i = 1; i < szf; ++i) {
            string opt = visitMuldivmod_op(o[i - 1]).as<string>();
            if (opt == "*") res = mul(res, visitFactor(f[i]).as<BaseType>());
            if (opt == "/") res = ddiv(res, visitFactor(f[i]).as<BaseType>());
            if (opt == "//") res = idiv(res, visitFactor(f[i]).as<BaseType>());
            if (opt == "%") res = mod(res, visitFactor(f[i]).as<BaseType>());
        }
        return res;
    }

    virtual antlrcpp::Any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) override {
        return ctx->getText();
    }

    virtual antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx) override {
        auto atomExpr = ctx->atom_expr();
        if (atomExpr) return visitAtom_expr(atomExpr);
        if (ctx->getText() == "+") return visitFactor(ctx->factor());
        else return BaseType(-visitFactor(ctx->factor()).as<BaseType>());
    }

    virtual antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
        auto trailer = ctx->trailer();
        if (!trailer) return visitAtom(ctx->atom());
        auto functionName = ctx->atom()->getText();
        auto var = visitTrailer(trailer).as<std::vector<std::pair<std::string, BaseType> > >();
        if (functionName == "print") {
            for (auto i : var)
                i.second.print(' ');
            cout << endl;
            return BaseType();
        } else if (functionName == "exit") {
            exit(0);
        } else if (functionName == "int") {
        } else if (functionName == "float") {
        } else if (functionName == "str") {
        } else if (functionName == "bool") {
        } else {
            const Func &nowFunc = Function[functionName];
            Scope nowScope = nowFunc.scope;
            int idx = 0;
            for (auto x : var) {
                if (x.first == "")
                    nowScope.varRegister(nowFunc.testlist[idx++], x.second);
                else nowScope.varRegister(x.first, x.second);
            }
            Stack.push(nowScope);
            auto res = visitSuite(nowFunc.suite);
            Stack.pop();

//            cout << functionName << endl;
//            cout << res.is<BaseType>() << endl;
//            cout << res.is<std::vector<BaseType>>() << endl;
//            cout << res.is<int>() << endl;

            if (res.is<std::vector<BaseType> >()) {
                auto ret = res.as<std::vector<BaseType> >();
                if (ret.size() == 1) return ret[0];
                else return ret;
            } else return BaseType(int2048(0));
        }
    }

// trailer: '(' (arglist)? ')' ;
// arglist: argument (',' argument)*  (',')?;
// argument: ( test |
//             test '=' test );

    virtual antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx) override {
        if (ctx->arglist())
            return visitArglist(ctx->arglist());
        return std::vector<std::pair<std::string, BaseType> >();
    }

    std::pair<bool, double> stringToDouble(const string &number) { // TODO: Utils.h
        int idx = -1, sz = number.size();
        for (int i = 0; i < sz; ++i)
            if (number[i] == '.') idx = i;
        if (idx == -1) return std::make_pair(false, 0);
        double res = 0;
        for (int i = idx - 1; i >= 0; --i)
            res = res * 10 + number[i] - '0';

        double tmp = 0.1;
        for (int i = idx + 1; i < sz; ++i) {
            res += tmp * (number[i] - '0');
            tmp /= 10.0;
        }

        return std::make_pair(true, res);
    }

    virtual antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx) override {
        if (ctx->NUMBER()) {
            std::string number = ctx->NUMBER()->getText();
            std::pair<bool, double> tmp = stringToDouble(number);
            if (tmp.first) return BaseType(tmp.second);
            return BaseType(int2048(number));
        } else if (ctx->NAME()) {
            auto result = Stack.top().varQuery(ctx->NAME()->getText());
            if (result.first) return result.second;
        } else if (ctx->test()) return visitTest(ctx->test());
        else if (ctx->TRUE()) return BaseType(true);
        else if (ctx->FALSE()) return BaseType(false);
        else if (ctx->NONE()) return BaseType();
        else {
            auto s = ctx->STRING();
            string res;
            res.clear();
            for (auto t : s) {
                string tmp = t->getText();
                for (int i = 1, sz = tmp.size(); i < sz - 1; ++i)
                    res += tmp[i];
            }
                
            return BaseType(res);
        }
    }

    virtual antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx) override {
        std::vector<BaseType> varData;
        auto test = ctx->test();
        for (auto x : test) {
            auto getTest = visitTest(x);
            if (getTest.is<BaseType>())
                varData.push_back(getTest.as<BaseType>());
            else {
                auto testList = getTest.as<std::vector<BaseType> >();
                for (auto x : testList)
                    varData.push_back(x);
            }
        }
//        cout << " @ " << varData.size() << endl;
//        for (auto x : varData)
//            x.print('\n');
//        cout << " # # # " << endl;
        return varData;
    } // testlist: test (',' test)* (',')?;

    virtual antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx) override {
        std::vector<std::pair<std::string, BaseType> > res;
        auto argu = ctx->argument();
        for (auto x : argu)
            res.push_back(visit(x).as<std::pair<std::string, BaseType> >());
        return res;
    } // arglist: argument (',' argument)*  (',')?;

    virtual antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx) override {
        auto test = ctx->test();
        if (test.size() == 1)
            return std::make_pair(std::string(), visitTest(test[0]).as<BaseType>());
        return std::make_pair(test[0]->getText(), visitTest(test[1]).as<BaseType>());
    } // argument: ( test | test '=' test );
};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H

