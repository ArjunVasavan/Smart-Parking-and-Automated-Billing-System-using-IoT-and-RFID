# 🚗 Smart Parking and Automated Billing System (Arduino Cloud)

An intelligent, cloud-connected smart parking system powered by ESP32, RFID, and IR sensors, with seamless integration into Arduino IoT Cloud. This project automates vehicle access, real-time slot monitoring, and dynamic billing using servo-controlled gate mechanisms and cloud-based data management.

---

## 🎯 Project Objective

To design and implement a contactless smart parking solution that:

* Authenticates vehicles using RFID tags
* Detects parking slot occupancy using IR sensors
* Logs entry and exit times accurately
* Calculates parking duration and billing automatically
* Sends real-time updates to the Arduino IoT Cloud dashboard

---

## 🛠️ Components Used

* ESP32 Dev Board
* MFRC522 RFID Reader with RFID Tags
* IR Sensors (x2)
* SG90 Servo Motor
* 5V Regulated Power Supply (LM7805)
* Arduino IoT Cloud Account

---

## ☁️ Arduino Cloud Variables

* `irvalue1` (int) – IR Sensor 1 Status
* `irvalue2` (int) – IR Sensor 2 Status

> These variables are automatically updated by the `updateIRValues()` function during runtime.

---

## 🔁 System Workflow

1. Vehicle approaches the gate → Detected by IR sensor
2. RFID card is scanned → UID is authenticated
3. Gate opens via servo motor → Entry time is logged
4. IR sensor detects parking slot occupancy
5. On exit → IR sensor detects absence → Exit time is logged
6. Parking duration is calculated
7. Billing amount is computed based on the rate
8. Cloud variables are updated for real-time monitoring

---

## 💰 Billing Logic

The billing structure is based on parking duration:

* ₹10 for up to 1 hour
* ₹10 per additional hour (capped at ₹50 per day)
* Maximum charge per 24 hours: ₹50

```cpp
float billingAmount = min(MAX_HOURLY_RATE, parkingHours * HOURLY_RATE);
```

---

## 🧠 Key Functions and Code Highlights

* `handleParkingSpot()` – Manages vehicle presence, entry/exit time, and triggers billing
* `calculateBillingAmount()` – Implements billing calculation logic
* `ArduinoCloud.update()` – Maintains cloud sync in `loop()`
* `thingProperties.h` – Auto-generated configuration from Arduino IoT Cloud

---

## 🚀 Setup Instructions

1. **Connect the hardware** as follows:

   * RFID Reader → `SS = GPIO5`, `RST = GPIO22`
   * Servo Motor → `GPIO32`
   * IR Sensors → `GPIO13` and `GPIO12`
2. **Log in to Arduino IoT Cloud** and create a new **Thing**
3. **Add variables**: `irvalue1`, `irvalue2` as integers
4. **Upload your sketch** using the Arduino Cloud Web Editor
5. **Configure your dashboard** to visualize IR sensor data

---

## 📋 Project Capabilities

* Monitors up to two parking slots
* Authenticates vehicles using RFID tags
* Real-time updates to cloud dashboard
* Automatically calculates and displays billing
* Modular and scalable for more slots or sensors

---

## 🔮 Potential Enhancements

* Mobile-friendly dashboards for users
* OLED or LCD screen for on-site display
* Online booking system with slot reservation
* Integration of number plate recognition using camera

---

## 👨‍💻 Author

**ARJUN VASAVAN**
[GitHub](https://github.com/ArjunVasavan)
[LinkedIn](https://www.linkedin.com/in/arjun-vasavan/)

---

## 📄 License

This project is licensed under the MIT License.
