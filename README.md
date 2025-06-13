# esp-phisher-node

A repeater companion node for the [esp-phisher](https://github.com/socalit/esp-phisher) project.  
This ESP32-based device clones the strongest nearby **open Wi-Fi SSID** and redirects all DNS traffic to the main captive portal server at `192.168.4.1`.

---

## Purpose

**esp-phisher-node** is an educational tool created to demonstrate the risks of connecting to **open Wi-Fi networks** and blindly trusting captive portals. It helps extend the reach of a primary phishing AP by repeating the SSID and redirecting clients.

---

## Features

- Scans and clones the strongest open SSID
- Creates a fake AP using the cloned name
- Redirects all DNS traffic to the main captive portal (e.g., `192.168.4.1/index.html`)
- Supports captive portal auto-launch on most devices
- LED indicators:
  - `LED_PWR` (GPIO 14): Always ON when powered
  - `LED_TX` (GPIO 12): Solid ON when a client connects

---

## Educational Use Only

This tool is meant for:
- Red team demos
- Ethical hacking research
- Wi-Fi security awareness
- DEFCON-style experiments

---

## Setup

1. Flash using **Arduino IDE** or **PlatformIO**
2. Board: `ESP32 Dev Module`
3. Wiring:
   - `LED_PWR` → GPIO 14
   - `LED_TX` → GPIO 12

4. Power via USB or 5V to VIN

---

## How It Works

- The ESP32 scans for nearby open networks
- It clones the strongest SSID found
- Starts a fake AP using that SSID
- All DNS traffic is redirected to `192.168.4.1`
- The main `esp-phisher` handles web content and logging

