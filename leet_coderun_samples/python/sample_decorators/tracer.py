from functools import wraps

def parse_args(*args):
    result = "\n- ".join(list(map(str, args)))
    return result

def parse_kwargs(**kwargs):
    result = "\n".join([f"- {x[0]}: {x[1]}" for x in kwargs.items()])
    return result

def tracer(func):
    @wraps(func)
    def tracer_wrapper(*args, **kwargs):
        print(f"The input parameters for function {func.__name__!r} are:\n- {parse_args(*args)}\n{parse_kwargs(**kwargs)}.")
        result = func(*args, **kwargs)
        print(f"The function returns {result} as the result.")
        return result

    return tracer_wrapper

def tracer_no_wraps(func):
    def tracer_wrapper(*args, **kwargs):
        print(f"The input parameters for function {func.__name__!r} are:\n- {parse_args(*args)}\n{parse_kwargs(**kwargs)}.")
        result = func(*args, **kwargs)
        print(f"The function returns {result} as the result.")
        return result

    return tracer_wrapper