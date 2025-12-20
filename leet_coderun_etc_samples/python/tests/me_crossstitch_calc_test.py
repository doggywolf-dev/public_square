import pytest
from me_crossstitch_calc import OneSchemeProbability
        
def test_me_crossstitch_calc_0():
    """Test OneSchemeProbability 0."""
    scheme = OneSchemeProbability(3, 3, 3)
    assert f"{scheme.сalc():.7f}" == "0.3333333"

def test_me_crossstitch_calc_1():
    """Test OneSchemeProbability 1."""
    scheme = OneSchemeProbability(9, 16, 18)
    assert f"{scheme.сalc():.7f}" == "0.9012613"

def test_me_crossstitch_calc_2():
    """Test OneSchemeProbability 2."""
    scheme = OneSchemeProbability(16, 9, 18)
    assert f"{scheme.сalc():.7f}" == "0.0696533"

def test_me_crossstitch_calc_3():
    """Test OneSchemeProbability 3."""
    scheme = OneSchemeProbability(4, 5, 6)
    assert f"{scheme.сalc():.7f}" == "0.5446606"
