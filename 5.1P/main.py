from tkinter import *
from gpiozero import LED

# Set up the LEDs
living_room = LED(17)
bathroom = LED(27)
closet = LED(22)

# Turn off all LEDs
def all_off():
    living_room.off()
    bathroom.off()
    closet.off()

# Turn on the selected LED
def change_light():
    all_off()

    if choice.get() == 1:
        living_room.on()
    elif choice.get() == 2:
        bathroom.on()
    elif choice.get() == 3:
        closet.on()

# Turn off the LEDs and close the window
def exit_program():
    all_off()
    living_room.close()
    bathroom.close()
    closet.close()
    window.destroy()

# Create the window
window = Tk()
window.title("House Lights")
window.geometry("300x250")

choice = IntVar(value=0)

Label(window, text="Select a room light").pack(pady=15)

Radiobutton(
    window, text="Living Room",
    variable=choice, value=1,
    command=change_light
).pack(pady=5)

Radiobutton(
    window, text="Bathroom",
    variable=choice, value=2,
    command=change_light
).pack(pady=5)

Radiobutton(
    window, text="Closet",
    variable=choice, value=3,
    command=change_light
).pack(pady=5)

Button(window, text="Exit", command=exit_program).pack(pady=15)

# Also handle closing with the X button
window.protocol("WM_DELETE_WINDOW", exit_program)

window.mainloop()