#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3BaseVisitor.h"
#include "Scope.h"
#include "Exception.h"
#include "utils.h"
#include "BaseType.h"


class EvalVisitor: public Python3BaseVisitor {

    Scope scope;

    virtual antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
        // TODO: exprSTMT
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) override {
        return visitOr_test(ctx->or_test());
    }

    virtual antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx) override {
        auto tmp = ctx->and_test();
        if (tmp.size() == 1)
            return visitAnd_test(tmp[0]);
        for (auto x : tmp)
            if ((bool) visitAnd_test(x))
                return true;
        return BaseType(false);
    }

    virtual antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx) override {
        auto tmp = ctx->not_test();
        if (tmp.size() == 1)
            return visitNot_test(tmp[0]);
        for (auto x : tmp)
            if (! (bool) visitNot_test(x)) 
                return false;
        return BaseType(true);
    }

    virtual antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx) override {
        if (ctx->NOT()) return BaseType(!(bool)visitNot_test(ctx->not_test()));
        else return visitComparison(ctx->comparison());
    }

    bool mycmp(const BaseType &lhs, const BaseType &rhs, const BaseType &opt) {
        string o = (string) opt;
        if (o == "<") return lhs < rhs;
        if (o == ">") return lhs > rhs;
        if (o == "==") return lhs == rhs;
        if (o == ">=") return lhs >= rhs;
        if (o == "<=") return lhs <= rhs;
        if (o == "!=") return lhs != rhs;
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
            if (!mycmp(last, now, visitComp_op(opt[i - 1])))
                return BaseType(false);
            last = now;
        }
        return BaseType(true);
    }

    virtual antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) override {
        return BaseType(ctx->getText());
    }

    virtual antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
        auto t = ctx->term();
        BaseType res = visitTerm(t[0]);
        auto szt = t.size();
        if (szt == 1) return res;
        auto o = ctx->addorsub_op();
        for (int i = 1; i < szt; ++i) {
            if ((bool)(visitAddorsub_op(o[i - 1])))
                res = res + visitTerm(t[i]);
            else res = res - visitTerm(t[i]);
        }
        return res;
    }

    virtual antlrcpp::Any visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) override {
        return BaseType((bool)(ctx->getText() == "+"));
    }

    virtual antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx) override {
        auto f = ctx->factor();
        BaseType res = visitFactor(f[0]);
        auto szf = f.size();
        if (szf == 1) return (res); 
        auto o = ctx->muldivmod_op();
        for (int i = 1; i < szf; ++i) {
            string opt = visitMuldivmod_op(o[i - 1]).as<string>();
            if (opt == "*") res = mul(res, visitFactor(f[i]));
            if (opt == "/") res = ddiv(res, visitFactor(f[i]));
            if (opt == "//") res = idiv(res, visitFactor(f[i]));
            if (opt == "%") res = mod(res, visitFactor(f[i]));
        }
        return res;
    }

    virtual antlrcpp::Any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) override {
        return BaseType(ctx->getText());
    }

    virtual antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx) override {
        auto a = ctx->atom_expr();
        if (a) return visitAtom_expr(a);
        if (ctx->getText() == "+") return visitFactor(ctx->factor());
        // else return -visitFactor(ctx->factor()); TODO
    }

    virtual antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
        auto t = ctx->trailer();
        if (t) ; // TODO; Function
        else return visitAtom(ctx->atom());
    }

    virtual antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx) override {
        // TODO function 
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx) override {
        if (ctx->NUMBER()) {
            return stringToInt(ctx->NUMBER()->getText());
        } else if (ctx->NAME()) {
            auto result = scope.varQuery(ctx->NAME()->getText());
            if (result.first) return result.second;
        } else if (ctx->test()) return visitTest(ctx->test());
        else if (ctx->TRUE()) return BaseType(true);
        else if (ctx->FALSE()) return BaseType(false);
        else if (ctx->NONE()) return BaseType();
        else {
            auto s = ctx->STRING();
            string res;
            res.clear();
            for (auto t : s)
                res += t->getText();
            return BaseType(res);
        }
    }

    virtual antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx) override {
        auto t = ctx->test();
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx) override {
        return visitChildren(ctx);
    }

};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H
