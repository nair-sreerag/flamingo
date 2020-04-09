#include "util.h"
#include "env.h"

Env *env_new(void)
{
    Env *ret = s_malloc(sizeof(Env));

    ret->nelem = 0;
    ret->sym_list = NULL;
    ret->obj_list = NULL;
    ret->parent = NULL;

    env_register_all(ret);

    return ret;
}

Object *env_get(Env *env, Object *obj)
{
    for (size_t i = 0; i < env->nelem; ++i)
        if (!strcmp(env->sym_list[i], obj->r.symbol))
            return obj_cp(env->obj_list[i]);
    return env->parent
        ? env_get(env->parent, obj)
        : obj_new_err("Use of undefined symbol '%s'", obj->r.symbol);
}

void env_set(Env *env, Object *key, Object *obj)
{
    /* check for existing variable(s) */
    for (size_t i = 0; i < env->nelem; ++i)
        if (!strcmp(env->sym_list[i], key->r.symbol)) {
            obj_free(env->obj_list[i]);
            env->obj_list[i] = obj_cp(obj);
            return;
        }

    /* none found, insert new at end */
    env->sym_list = s_realloc(env->sym_list, ++env->nelem * sizeof(char *));
    env->obj_list = s_realloc(env->obj_list, env->nelem * sizeof(Object *));

    env->sym_list[env->nelem - 1] = dupstr(key->r.symbol);
    env->obj_list[env->nelem - 1] = obj_cp(obj);
}

void env_set_global(Env *env, Object *key, Object *obj)
{
    for (; env->parent; env = env->parent);
    env_set(env, key, obj);
}

Env *env_cp(Env *env)
{
    Env *ret = s_malloc(sizeof(Env));

    ret->nelem = env->nelem;
    ret->parent = env->parent;
    ret->sym_list = s_malloc(ret->nelem * sizeof(char *));
    ret->obj_list = s_malloc(ret->nelem * sizeof(Object *));

    for (size_t i = 0; i < ret->nelem; ++i) {
        ret->sym_list[i] = dupstr(env->sym_list[i]);
        ret->obj_list[i] = obj_cp(env->obj_list[i]);
    }

    return ret;
}

void env_register(Env *env, const char *id, BuiltinFn func)
{
    Object *key = obj_new_sym(id);
    Object *obj = obj_new_func(func);
    env_set(env, key, obj);
    obj_free(key);
    obj_free(obj);
}

void env_register_all(Env *env)
{
    env_register(env, "exit", bi_exit);

    env_register(env, "list", bi_list);
    env_register(env, "car", bi_car);
    env_register(env, "cdr", bi_cdr);
    env_register(env, "eval", bi_eval);
    env_register(env, "attach", bi_attach);
    env_register(env, "init", bi_init);

    env_register(env, "+", bi_add);
    env_register(env, "-", bi_sub);
    env_register(env, "*", bi_mul);
    env_register(env, "/", bi_div);
    env_register(env, "%", bi_mod);
    env_register(env, "^", bi_bxor);
    env_register(env, "&", bi_band);
    env_register(env, "|", bi_bor);
    env_register(env, "~", bi_bnot);
    env_register(env, "pow", bi_pow);
    env_register(env, "min", bi_min);
    env_register(env, "max", bi_max);

    env_register(env, "<", bi_lt);
    env_register(env, "<=", bi_le);
    env_register(env, ">", bi_gt);
    env_register(env, ">=", bi_ge);
    env_register(env, "==", bi_eq);
    env_register(env, "!=", bi_ne);

    env_register(env, "def", bi_def);
    env_register(env, "=", bi_loc);
    env_register(env, "$", bi_lambda);
}

void env_free(Env *env)
{
    for (size_t i = 0; i < env->nelem; ++i) {
        free(env->sym_list[i]);
        obj_free(env->obj_list[i]);
    }

    free(env->sym_list);
    free(env->obj_list);
    free(env);
}
