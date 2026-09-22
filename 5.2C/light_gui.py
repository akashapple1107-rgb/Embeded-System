import tkinter as tk
from gpiozero import PWMLED


def main():
    # BCM GPIO18 is physical pin 12 on the Raspberry Pi header.
    window = tk.Tk()
    window.title("Living Room Light")
    window.geometry("440x290")

    try:
        light = PWMLED(18, frequency=100, initial_value=0)
    except Exception:
        window.destroy()
        raise

    brightness = tk.IntVar(value=0)
    status = tk.StringVar(value="Light OFF | Duty cycle: 0%")

    def change_brightness(value):
        percent = int(float(value))
        light.value = percent / 100
        state = "OFF" if percent == 0 else "ON"
        status.set(f"Light {state} | Duty cycle: {percent}%")

    def set_brightness(percent):
        brightness.set(percent)
        change_brightness(percent)

    tk.Label(window, text="Living Room Light", font=("Arial", 20)).pack(pady=16)
    tk.Label(window, text="Move the slider to change the brightness.").pack()

    slider = tk.Scale(
        window, from_=0, to=100, orient=tk.HORIZONTAL,
        length=350, resolution=1, variable=brightness,
        command=change_brightness
    )
    slider.pack(pady=12)

    buttons = tk.Frame(window)
    buttons.pack()
    tk.Button(buttons, text="ON", width=10,
              command=lambda: set_brightness(100)).pack(side=tk.LEFT, padx=8)
    tk.Button(buttons, text="OFF", width=10,
              command=lambda: set_brightness(0)).pack(side=tk.LEFT, padx=8)
    tk.Label(window, textvariable=status).pack(pady=12)
    tk.Button(window, text="Exit", command=window.destroy).pack()
    window.protocol("WM_DELETE_WINDOW", window.destroy)

    try:
        window.mainloop()
    finally:
        light.off()
        light.close()


if __name__ == "__main__":
    main()
