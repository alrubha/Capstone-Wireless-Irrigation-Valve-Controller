#Valve control gui program
#Written by Ali Alrubh 3/4/2018
from Tkinter import *			#for gui
import serial

def set_cycle():
	#isinstance(valve.get(), int)
	if cycle.get().isdigit() == True:
		if valve.get().isdigit() == True:
			command = valve.get() + ',' + cycle.get() + ';'
			port.write(command)
			print port.readlines()						#sending cycle time
			#print port.readline()						#success or fail
			#print port.readline()						#getting status
			#print port.readline()						#opened or voltage is low

		else:
			print("Invalid Input")

	else:
		print("Invalid Input")

def volt():
	if valve.get().isdigit() == True:
		command = valve.get() + ',' + 'V' + ';'
		port.write(command)
		print port.readlines()								#asking for voltage
		#print port.readline()								#success or fail
		#print port.readline()								#getting voltage
		#print port.readline()								#voltage=
		#print port.readline()								#x
		#print port.readline()								#.
		#print port.readline()								#y
		#print port.readline()								#volts

	else:
		print("Invalid Input")

def close():
	if valve.get().isdigit() == True:
		command = valve.get() + ',' + 'D' + ';'
		port.write(command)
		print port.readlines()								#disabling valve
		#print port.readline()								#success or fail

	else:
		print("Invalid Input")


def status():
	command = 'S'
	port.write(command)
	#for x in range(1):
	print port.readlines()								#asking for state
	#	print port.readline()								#valve
	#	print port.readline()								#number
	#	print port.readline()								#>>>
	#	print port.readline()								#success or file
	#	print port.readline()								#getting state
	#	print port.readline()								#valve
	#	print port.readline()								#number
	#	print port.readline()								#is open


port = serial.Serial('COM5', 9600, timeout=3)				#serial port of Hub

root = Tk()

root.wm_title("Valves Control")

# *** Valve settings ***

Label(root,text="Valve Operations").grid(row=1)

Label(root, text="Choose Valve").grid(row=2)
valve = Entry(root)
valve.grid(row=2,column=1)

Label(root, text="Cycle in Mins").grid(row=2,column=2)
cycle = Entry(root)
cycle.grid(row=2,column=3)

submit2 = Button(root,text="Submit",command=set_cycle)      			#change command
submit2.grid(row=2,column=4)

Label(root, text="OR").grid(row=3,column=2)

voltage = Button(root,text="System Voltage",command=volt)		#change command
voltage.grid(row=3,column=3)

disable = Button(root,text="Disable",command=close)				#change command
disable.grid(row=3,column=4)

# *** Valve state ***

state = Button(root,text="Which valve is open?",command=status)		#change command
state.grid(row=4,column=0)

root.mainloop()
