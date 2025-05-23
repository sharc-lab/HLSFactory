import math


def create_lookup_table(start, end, step):
    table_radians = {}
    table_degrees = {}
    radians_start = math.radians(start)
    radians_end = math.radians(end)
    current_angle_radians = radians_start
    current_angle_degrees = start

    while current_angle_radians <= radians_end:
        table_radians[round(current_angle_radians, 2)] = round(
            math.atan(current_angle_radians), 2
        )
        table_degrees[current_angle_degrees] = round(
            math.atan(current_angle_radians), 2
        )
        current_angle_radians += step
        current_angle_degrees += step

    return table_radians, table_degrees


lookup_table_radians, lookup_table_degrees = create_lookup_table(0, 90, 0.01)

# Save the lookup table in a file
with open("tan_inverse_lut.txt", "w") as file:
    file.write("Radians\n")
    for angle, approximation in lookup_table_radians.items():
        file.write("{} {}\n".format(angle, approximation))

    file.write("\nDegrees\n")
    for angle, approximation in lookup_table_degrees.items():
        file.write("{} {}\n".format(angle, approximation))

# Example usage:
angle_radians = math.radians(45)
approximation_radians = lookup_table_radians[round(angle_radians, 2)]
print(
    "Approximation of tan^(-1)({} radians) is {}".format(
        angle_radians, approximation_radians
    )
)

angle_degrees = 45
approximation_degrees = lookup_table_degrees[angle_degrees]
print(
    "Approximation of tan^(-1)({} degrees) is {}".format(
        angle_degrees, approximation_degrees
    )
)
