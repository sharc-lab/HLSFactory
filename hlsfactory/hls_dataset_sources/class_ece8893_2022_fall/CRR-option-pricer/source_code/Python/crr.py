# Import packages
# https://github.com/tienusss/Option_Calculations
import numpy as np

# Defined functions


def CRRBinomial(OutputFlag, AmeEurFlag, CallPutFlag, S, X, T, r, c, v, n):
    # This functions calculates CRR price, delta and gamma for of American and European options

    # This code is based on "The complete guide to Option Pricing Formulas" by Espen Gaarder Haug (2007)

    # Translated from a VBA code

    # OutputFlag:

    # "P" Returns the options price

    # "d" Returns the options delta

    # "a" Returns an array containing the option value, delta and gamma

    # AmeEurFlag:

    # "a" Returns the American option value

    # "e" Returns the European option value

    # CallPutFlag:

    # "C" Returns the call value

    # "P" Returns the put value

    # S is the share price at time t

    # X is the strike price

    # T is the time to maturity in years (days/365)

    # r is the risk-free interest rate

    # c is the cost of carry rate

    # v is the volatility

    # n determines the stepsize

    # Creates a list with values from 0 up to n (which will be used to determine to exercise or not)

    n_list = np.arange(0, (n + 1), 1)

    # Checks if the input option is a put or a call, if not it returns an error

    if CallPutFlag == "C":
        z = 1

    elif CallPutFlag == "P":
        z = -1

    else:
        return "Call or put not defined"

    # Calculates the stepsize in years

    dt = T / n

    # The up and down factors

    u = np.exp(v * np.sqrt(dt))

    d = 1.0 / u

    p = (np.exp((r - q) * dt) - d) / (u - d)

    df = np.exp(-r * dt)

    # Creates the most right column of the tree

    max_pay_off_list = []

    for i in n_list:
        i = i.astype("int")

        max_pay_off = np.maximum(0, z * (S * u**i * d ** (n - i) - X))

        max_pay_off_list.append(max_pay_off)

    # The binominal tree

    for j in np.arange(n - 1, 0 - 1, -1):
        for i in np.arange(0, j + 1, 1):
            i = i.astype(int)  # Need to be converted to a integer

            if AmeEurFlag == "e":
                max_pay_off_list[i] = (
                    p * max_pay_off_list[i + 1] + (1 - p) * max_pay_off_list[i]
                ) * df

            elif AmeEurFlag == "a":
                max_pay_off_list[i] = np.maximum(
                    (z * (S * u**i * d ** (j - i) - X)),
                    (p * max_pay_off_list[i + 1] + (1 - p) * max_pay_off_list[i]) * df,
                )

        if j == 2:
            gamma = (
                (max_pay_off_list[2] - max_pay_off_list[1]) / (S * u**2 - S * u * d)
                - (max_pay_off_list[1] - max_pay_off_list[0]) / (S * u * d - S * d**2)
            ) / (0.5 * (S * u**2 - S * d**2))

        if j == 1:
            delta = (max_pay_off_list[1] - max_pay_off_list[0]) / (S * u - S * d)

    price = max_pay_off_list[0]

    # Put all the variables in the list

    variable_list = [delta, gamma, price]

    # Return values

    if OutputFlag == "P":
        return price

    elif OutputFlag == "d":
        return delta

    elif OutputFlag == "g":
        return gamma

    elif OutputFlag == "a":
        return variable_list

    else:
        return "Indicate if you want to return P, d, g or a"


# Body of the script

# CRRBinomial(OutputFlag, AmeEurFlag, CallPutFlag, S, X, T, r, c, v, n)


S = 100

X = 100

T = 1.0

r = 0.05

c = 0.05

v = 0.2

n = 50

q = 0.05

dt = T / n

u = np.exp(v * np.sqrt(dt))

d = 1.0 / u

p = (np.exp((r - q) * dt) - d) / (u - d)

print("U", u, "p", p)


Eur_call_result = CRRBinomial("P", "e", "C", S, X, T, r, c, v, n)

American_call_result = CRRBinomial("P", "a", "C", S, X, T, r, c, v, n)

Eur_put_result = CRRBinomial("P", "e", "P", S, X, T, r, c, v, n)

American_put_result = CRRBinomial("P", "a", "P", S, X, T, r, c, v, n)


# Print the output of the results

print("The price of the European call option is equal to " + str(Eur_call_result))

print("The price of the American call option is equal to " + str(American_call_result))

print("The price of the European put option is equal to " + str(Eur_put_result))

print("The price of the American put option is equal to " + str(American_put_result))
