#ifndef CRISP_CONTEXT_HPP
#define CRISP_CONTEXT_HPP

#include "cons.hpp"
#include <functional>

namespace crisp {

struct atom {
    atom(const std::string& s) : str(std::nullptr) {
        bool initialized = false;
        for (std::string *a : allocated) {
            if (*a == s) {
                str = a;
                initialized = true;
                break;
            }
        }
        if (not initialized) {
            str = new std::string(s);
            allocated.push(str);
        }
    }

    bool operator == (const atom& a) const {
        return a.str == str;
    }

    friend std::ostream& operator << (std::ostream& os, const atom& a) {
        return os << *a->str;
    }

    std::string string() const {
        return *string;
    }

private:
    static std::vector<std::string *> allocated;
    std::string *str;
};


template <typename... T> struct context;
template <typename... T>
struct context<atom, T...> {
    using cons_t = typename cons<atom, T...>::cons_t;
    using cons_ptr = typename cons_t::cons_ptr;
    using value = typename cons_t::value;
    using whichValue = typename cons_t::whichValue;

#define mk_cons_ptr(x, y) std::make_shared(cons_t(x, y))

    context() : ctx(null), ctx_stack(null), ctx_global(null) {}

    void bind(const atom& key, const value& v) {
        ctx = value(mk_cons_ptr(cons_t(a, v), ctx));
    }

    void define(const atom& key, const value& v) {
        ctx_global = value(mk_cons_ptr(cons_t(a, v), ctx_global));
    }
    
    value lookup(const atom& key) throw(std::exception) {
        // first, look for the atom in ctx
        value v = ctx;
        while (v.which() == cons_t::isCons) {
            cons_ptr pos = boost::get<cons_ptr>(v)->car();
            cons_ptr c = boost::get<cons_ptr>(pos);
            if (boost::get<atom>(c.car()) == key) {
                return c.cdr();
            }
            v = pos.cdr();
        }

        // then, look for the atom in ctx_global
        v = ctx_global;
        while (v.which() == cons_t::isCons) {
            cons_ptr pos = boost::get<cons_ptr>(v)->car();
            cons_ptr c = boost::get<cons_ptr>(pos);
            if (boost::get<atom>(c.car()) == key) {
                return c.cdr();
            }
            v = pos.cdr();
        }

        // TODO throw an error
    }

    void set(const atom& key, const value& v) {
        // TODO
        // first, look for the atom in ctx
        // then, look for the atom in ctx_global
        // if found, set it's value
        // otherwise, bind it's value
    }

    void push(const value& _ctx) {
        ctx_stack = value(mk_cons_ptr(ctx, ctx_stack));
        ctx = _ctx;
    }

    void push() {
        push(value(null));
    }

    value capture() {
        return ctx;
    }

    value pop() {
        value old = ctx;
        if (ctx_stack.which() == cons_t::isCons) {
            cons_ptr stk = boost::get<cons_ptr>(ctx_stack);
            ctx = stk.car();
            ctx_stack = stk.cdr();
        } else {
            ctx = value(null);
        }
        return old;
    }

#undef mk_cons_ptr

private:
    value ctx;
    value ctx_stack;
    value ctx_global;
};

}

#endif

