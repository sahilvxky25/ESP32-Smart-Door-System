# 🏠 IoT Smart Door Access & Theft Detection System

## 📌 Overview
This project is an IoT-based smart home security system built using ESP32 that enables real-time monitoring, intrusion detection, and remote door access control. It combines multiple sensors with a backend system to provide a reliable and scalable security solution.

---

## 🚀 Features
- 🔐 Remote door locking/unlocking using servo motor  
- 📹 Live video streaming using ESP32-CAM  
- 🚨 Real-time intrusion detection with buzzer alerts  
- 📡 Multi-sensor fusion (PIR + Ultrasonic + IR) for accuracy  
- 📲 Mobile/Web dashboard for monitoring and control  
- 🔔 Instant alerts for suspicious activity  
- 🧠 Smart detection logic to reduce false alarms  

---

## 🧰 Hardware Components
- ESP32 / ESP32-CAM  
- Ultrasonic Sensor  
- PIR Motion Sensor  
- IR Sensor  
- Servo Motor  
- Buzzer  
- Relay Module  
- Temperature Sensor  

---

## 🏗️ System Architecture
ESP32 Sensors → MQTT Broker → Go Backend → Database → WebSocket → Dashboard


---

## ⚙️ Tech Stack
- Embedded Systems: ESP32 (Arduino Framework)  
- Communication: MQTT Protocol  
- Backend: Go (Golang)  
- Database: PostgreSQL / Time-series DB  
- Frontend: Web/Mobile Dashboard (e.g., Flutter)  

---

## 📊 How It Works
1. Sensors detect motion or proximity near the door  
2. Camera activates and streams live video  
3. Owner receives alert and views live feed  
4. Owner can:
   - Unlock door remotely  
   - Ignore or monitor  
5. Forced entry triggers alarm + notification  

---

## 🔐 Use Cases
- Smart home security  
- Remote visitor monitoring  
- Intrusion detection systems  
- Automated access control  

---

## 📸 Future Improvements
- Face recognition for automatic access  
- Cloud storage for video logs  
- Mobile app integration  
- AI-based anomaly detection  

---

## 👨‍💻 Authors
- Sahil Kumar Patra, Dipankar Ghosh
