# -*- coding: utf-8 -*-
"""
Created on Thu May  4 08:10:48 2023

@author: ritha
"""

import math

def create_lookup_table(start, end, step):
    table = {}
    radians_start = math.radians(start)
    radians_end = math.radians(end)
    current_angle = radians_start
    
    while current_angle <= radians_end:
        table[round(current_angle, 2)] = round(math.atan(current_angle), 2)
        current_angle += step
    
    return table

lookup_table = create_lookup_table(-1, 1, 0.01)

# Save the lookup table in a file
with open("tan_inverse_lut.txt", "w") as file:
    for angle, approximation in lookup_table.items():
        file.write("{} {}\n".format(angle, approximation))

# Example usage:
angle = 0.5
approximation = lookup_table[round(math.radians(angle), 2)]
print("Approximation of tan^(-1)({}) is {}".format(angle, approximation))
