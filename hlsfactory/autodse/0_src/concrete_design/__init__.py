"""
Concrete Design module for HLS synthesis and analysis.

This module provides the core abstraction for managing individual HLS designs,
including synthesis execution and results analysis.
"""

from .design import ConcreteDesign
from .c_to_rtl_mapper import CToRTLMapper

__all__ = ['ConcreteDesign', 'CToRTLMapper']