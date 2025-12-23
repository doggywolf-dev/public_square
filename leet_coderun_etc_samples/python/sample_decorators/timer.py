'''
Timer Decorators Module

This module provides decorator functions for measuring the execution time
of other functions. Useful for performance profiling and debugging.
'''

from functools import wraps
from time import time

def timer(func):
    '''
    A decorator that measures and prints the execution time of a function.

    Uses @wraps to preserve the original function's metadata (name, docstring, etc.),
    which is essential for debugging, documentation tools, and introspection.
    '''
    @wraps(func)
    def func_wrapper(*args, **kwargs):
        start_time = time()
        result = func(*args, **kwargs)
        end_time = time()
        print(f"Execution time of function {func.__name__!r}: {end_time - start_time:.7f} seconds.")
        return result

    return func_wrapper

def timer_no_wraps(func):
    '''
    A timer decorator WITHOUT @wraps — for demonstration purposes.

    This version does NOT preserve the original function's metadata.
    As a result:
        - decorated_func.__name__ will be "func_wrapper" instead of the original name
        - decorated_func.__doc__ will be None (or the wrapper's docstring)

    This function exists to illustrate WHY @wraps is important.
    In production code, always prefer the "timer" decorator above.
    '''
    def func_wrapper(*args, **kwargs):
        start_time = time()
        result = func(*args, **kwargs)
        end_time = time()
        print(f"Execution time of function {func.__name__!r}: {end_time - start_time:.7f} seconds.")
        return result

    return func_wrapper
