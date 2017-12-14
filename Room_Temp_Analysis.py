import sys
import numpy as np
import matplotlib.pyplot as plt
from lmfit import Model, Parameters



def main():
    #Input the name of the file to be read from the command line
    filename = str(sys.argv[1])
    #Read out the temperatures and times associated with them
    time, temps = np.loadtxt(filename, skiprows=1, usecols = (0, 1), unpack = True)
    plt.plot(time, temps, 'x', label="Heating Data")
    plt.legend()
    plt.xlabel("Time (min)")
    plt.ylabel("Temperature (C)")
    plt.show()

    exponentialModel = Model(exponentialFunction)
    p = Parameters()
    #The parameters in the equation are coupled, and so we can't solve for any of their actual values without knowing one
    #We choose an estimate of 30000 J/min for the heating rate
    p.add_many(
        ('h', 30000, False, None, None, None),
        ('k', 6000, True, 0, 10000, None),
        ('c', 600000, True, 0, 1000000, None),
        ('Tout', 16, False, None, None, None),
    )
    #Perform a least-squares regression to the data using the model function and parameters
    exponentialFit = exponentialModel.fit(temps, x=time, params=p)
    print(exponentialFit.fit_report())

    #Plot data and save the resulting figure.
    plt.plot(time, temps, 'x', label="Heating Data")
    plt.plot(time, exponentialFit.best_fit, label='Fit to Data')
    plt.legend()
    plt.xlabel("Time (min)")
    plt.ylabel("Temperature (C)")
    plt.grid()
    plt.suptitle("Fitting of Room Heating")
    plt.savefig("Room_Temp_Fitting.png")
    plt.show()


#Solution to a basic differential equation for a heater in a room
def exponentialFunction(x, h, k, c, Tout):
    y = (h/k)*(1-np.exp(-k/c*x)) + Tout
    return y


if __name__ == "__main__":
    main()