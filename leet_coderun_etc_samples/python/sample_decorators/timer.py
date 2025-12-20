from functools import wraps
from time import time

def timer(func):
    @wraps(func)
    def func_wrapper(*args, **kwargs):
        start_time = time()
        result = func(*args, **kwargs)
        end_time = time()
        print(f"Execution time of function {func.__name__!r}: {end_time - start_time:.7f} seconds.")
        return result

    return func_wrapper

def timer_no_wraps(func):
    def func_wrapper(*args, **kwargs):
        start_time = time()
        result = func(*args, **kwargs)
        end_time = time()
        print(f"Execution time of function {func.__name__!r}: {end_time - start_time:.7f} seconds.")
        return result

    return func_wrapper
