//%HIPE_LICENSE
#pragma once

template<typename Obj, typename Result, typename ...Args>
struct Delegate
{
    Obj x;
    Result (Obj::*f)(Args...);

    template<typename ...Ts>
    Result operator()(Ts&&... args)
    {
        return (x.*f)(forward<Ts>(args)...);
    }
};

template<typename Obj, typename Result, typename ...Args>
auto make_delegate(const Obj &x, Result (Obj::*fun)(Args...))
    -> Delegate<Obj, Result, Args...>
{
    Delegate<Obj, Result, Args...> result{x, fun};
    return result;
}

