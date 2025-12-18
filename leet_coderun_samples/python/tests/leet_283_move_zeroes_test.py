import pytest
from leet_283_move_zeroes import Solution_leet_283
        
@pytest.fixture
def calc():
    """Provides a Solution_leet_283 instance."""
    return Solution_leet_283()

def test_leet_283_0(calc):
    """Test moveZeroes 0."""
    data = [9, 0, 0, 0, 1, 2, 3]
    calc.moveZeroes(data)
    assert data == [9, 1, 2, 3, 0, 0, 0]

def test_leet_283_1(calc):
    """Test moveZeroes 1."""
    data = [0]
    calc.moveZeroes(data)
    assert data == [0]

def test_leet_283_2(calc):
    """Test moveZeroes 2."""
    data = []
    calc.moveZeroes(data)
    assert data == []

def test_leet_283_3(calc):
    """Test moveZeroes 3."""
    data = [1, 2, 3, 4, 5, 6]
    calc.moveZeroes(data)
    assert data == [1, 2, 3, 4, 5, 6]

def test_leet_283_4(calc):
    """Test moveZeroes 4."""
    data = [0, 0, 0, 0]
    calc.moveZeroes(data)
    assert data == [0, 0, 0, 0]