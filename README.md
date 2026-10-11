# HERMES-I2C 

## Why This Project Exists
Honestly, I just wanted to build a custom circuit board that was actually simple to put together, but still gave me endless possibilities without getting boring after a week. I wanted something way more interactive and personal, not like a sensor to measure humidity or temp cause that's kinda boring.

By putting a 6-axis motion sensor (accelerometer + gyroscope) and a neat little screen on a shared I2C highway, this single piece of hardware can become whatever I need it to be just by changing the code. If I'm playing a racing gamE, I can literally mount this board inside a 3D-printed steering wheel loop and physically tilt it to steer my car.

---

## How It Works

### 1. The Hardware Architecture
The board relies on three main active parts chatting over a shared **I2C communication bus**:

*   **The Brain (XIAO RP2040):** This is the main microcontroller. It handles the raw calculations and has built-in USB functionality. This means it can easily trick my PC into thinking it's a real keyboard, mouse, or gaming controller.
*   **The Sensor (LSM6DS3TR-C):** This is the high-precision 6-axis IMU tracking my physical movements. The accelerometer handles raw speed and direction shifts, while the gyroscope measures physical angles and tilt; I chosethis by looking at multiple datasheets for many types of sensors
*   **The Screen (0.96" OLED Module):** An SSD1306-driven display sharing the same data tracks. It prints live variables, movement graphs, or setup menus so I can see what mode the device is in without guessing.

### 2. The Shared Communication Highway (I2C Standard)
Instead of running a million messy wires across the layout, the sensor and display share the exact same two tracks: **SDA (Serial Data)** and **SCL (Serial Clock)**. 
*   **Addressing:** To stop data from bumping into each other, each chip has its own unique "phone number" (I2C address). Grounding the sensor's `SA0` pin locks its address to `0x6A`, keeping it perfectly separated from the display.
*   **Pull-Up Resistors:** The I2C standard is "open-drain," meaning the chips can only pull the communication signal down to 0V, but they can't push it up to 3.3V. I added twin **4.7kΩ resistors** connecting the data lines to the 3.3V power line so the digital signals don't float in electrical limbo.
*   **Power Filtering:** To keep the sensor from glitching or throwing random numbers when it moves suddenly, twin **0.1µF decoupling capacitors** sit right next to the power pins to filter out invisible electrical noise.

---

## Bill of Materials (BOM)

Every part on this schematic uses standard surface-mount (SMD) footprints mapped directly to **LCSC Part Numbers**. This lets the automated pick-and-place robot arms at JLCPCB find the parts in their warehouse inventory and solder them onto the board automatically.

| Qty | Component Type | Value / Model | LCSC Part # | Mechanical Footprint | Designator |
| :--- | :--- | :--- | :--- | :--- | :--- |
| 1 | Microcontroller | Seeed Studio XIAO RP2040 | `C2915003` | SMD Castellated Pad Map | `U1` |
| 1 | 6-Axis Motion Sensor | LSM6DS3TR-C (STMicroelectronics) | `C967633` | LGA-14 (`2.5mm x 3mm`) | `U2` |
| 1 | Display Connector Socket | 4-Pin Female Header | `C2718488` | `1x04 2.54mm` Vertical | `J1` |
| 2 | I2C Pull-Up Resistors | 4.7kΩ | `C22843` | SMD `0603` (`1608 Metric`) | `R1, R2` |
| 2 | Noise-Filtering Capacitors | 0.1µF | `C14663` | SMD `0603` (`1608 Metric`) | `C1, C2` |

---

## Schematic Design and PCB 

<img width="790" height="612" alt="image" src="https://github.com/user-attachments/assets/502a41b8-8ddb-4609-8504-2d535e939aaf" />

<img width="825" height="772" alt="image" src="https://github.com/user-attachments/assets/88abb570-69ac-4898-94e0-ebb06dafa5b4" />

<img width="831" height="802" alt="Screenshot 2026-10-11 002258" src="https://github.com/user-attachments/assets/af5a0540-6646-459b-aac7-e33b4963ae82" />





