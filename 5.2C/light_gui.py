import tkinter as tk
from gpiozero import PWMLED

# BCM GPIO numbers
leds = [
    PWMLED(18),
    PWMLED(23),
    PWMLED(24)
]

window = tk.Tk()
window.title("Light Brightness Control")

room_names = ["Living Room", "Bedroom", "Kitchen"]


def create_controls(name, led):
    frame = tk.LabelFrame(window, text=name, padx=15, pady=10)
    frame.pack(padx=20, pady=10, fill="x")

    brightness = tk.IntVar(value=0)

    def change_brightness(value):
        led.value = float(value) / 100

    def turn_on():
        brightness.set(100)
        change_brightness(100)

    def turn_off():
        brightness.set(0)
        change_brightness(0)

    slider = tk.Scale(
        frame,
        from_=0,
        to=100,
        orient=tk.HORIZONTAL,
        label="Brightness (%)",
        length=300,
        variable=brightness,
        command=change_brightness
    )
    slider.pack()

    buttons = tk.Frame(frame)
    buttons.pack(pady=5)

    tk.Button(
        buttons, text="ON", width=10, command=turn_on
    ).pack(side=tk.LEFT, padx=5)

    tk.Button(
        buttons, text="OFF", width=10, command=turn_off
    ).pack(side=tk.LEFT, padx=5)


for name, led in zip(room_names, leds):
    create_controls(name, led)

tk.Button(
    window, text="Exit", width=15, command=window.destroy
).pack(pady=10)

try:
    window.mainloop()
finally:
    for led in leds:
        led.off()
        led.close()
