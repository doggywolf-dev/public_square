import pytest
from leet_239_sliding_window_maximum import Solution_leet_239

@pytest.fixture
def calc():
    """Provides a Solution_leet_283 instance."""
    return Solution_leet_239()

def test_leet_283_0(calc):
    """Test maxSlidingWindow 0."""
    data = [9, 0, 0, 0, 1, 2, 3, 2, 57, 4, 1, 0, 2, 9, 3, 8, 8, 8, 8, 8, 6, 6, 6, 6, 6]
    k = 3
    result = calc.maxSlidingWindow(data, k)
    assert result == [9, 0, 1, 2, 3, 3, 57, 57, 57, 4, 2, 9, 9, 9, 8, 8, 8, 8, 8, 8, 6, 6, 6]

def test_leet_283_1(calc):
    """Test maxSlidingWindow 1."""
    data = [0]
    k = 1
    result = calc.maxSlidingWindow(data, k)
    assert result == [0]

def test_leet_283_2(calc):
    """Test maxSlidingWindow 2."""
    data = []
    k = 0
    result = calc.maxSlidingWindow(data, k)
    assert result == []

def test_leet_283_3(calc):
    """Test maxSlidingWindow 3."""
    data = [10, 9, 8, 7, 6, 5, 4, 3, 2, 1]
    k = 3
    result = calc.maxSlidingWindow(data, k)
    assert result == [10, 9, 8, 7, 6, 5, 4, 3]

def test_leet_283_4(calc):
    """Test maxSlidingWindow 4."""
    data = [0, 0, 0, 0]
    k = 1
    result = calc.maxSlidingWindow(data, k)
    assert result == [0, 0, 0, 0]