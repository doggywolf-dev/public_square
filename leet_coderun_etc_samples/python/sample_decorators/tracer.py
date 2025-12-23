'''
Tracer Decorators Module

This module provides decorator functions for logging inputs and return values
of other functions. Useful for performance profiling and debugging.
'''

from functools import wraps

def parse_args(*args):
    # Converts positional arguments into a formatted string representation.
    result = "\n- ".join(list(map(str, args)))
    return result

def parse_kwargs(**kwargs):
    # Converts keyword arguments into a formatted string representation.
    result = "\n".join([f"- {x[0]}: {x[1]}" for x in kwargs.items()])
    return result

def tracer(func):
    '''
    A decorator that logs function inputs and return values with metadata preservation.

    Uses @wraps to preserve the original function's metadata (name, docstring, etc.),
    which is essential for debugging, documentation tools, and introspection.
    '''
    @wraps(func)
    def tracer_wrapper(*args, **kwargs):
        print(f"The input parameters for function {func.__name__!r} are:\n- {parse_args(*args)}\n{parse_kwargs(**kwargs)}.")
        result = func(*args, **kwargs)
        print(f"The function returns {result} as the result.")
        return result

    return tracer_wrapper

def tracer_no_wraps(func):
    '''
    A tracer decorator WITHOUT @wraps — for demonstration purposes.

    This version does NOT preserve the original function's metadata.
    As a result:
        - decorated_func.__name__ will be "func_wrapper" instead of the original name
        - decorated_func.__doc__ will be None (or the wrapper's docstring)

    This function exists to illustrate WHY @wraps is important.
    In production code, always prefer the "tracer" decorator above.
    '''
    def tracer_wrapper(*args, **kwargs):
        print(f"The input parameters for function {func.__name__!r} are:\n- {parse_args(*args)}\n{parse_kwargs(**kwargs)}.")
        result = func(*args, **kwargs)
        print(f"The function returns {result} as the result.")
        return result

    return tracer_wrapper