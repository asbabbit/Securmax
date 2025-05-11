4G LTE Low-Power Security Camera System
Project Overview

This project was inspired by a conversation with a colleague whose father needed a security solution for a remote property with no access to power, Wi-Fi, or reliable sunlight. The goal: design a battery-powered security camera that streams live video over a 4G LTE connection, optimized for ultra-low power consumption — with the ambitious target of lasting 30 days on a single charge.
Current Features

    📡 Video Streaming: Transmitting SVGA compressed frames at 10 fps and 300 kbps to a remote server over 4G using an ESP32 development board

    📦 Data Transport: Utilizes UDP for low-latency packet transmission

    🔁 Network Resilience: Implements a circular buffer to handle intermittent network outages

    🧵 Multithreading: Employs threading to maximize CPU efficiency

Planned Improvements

    🔋 Power Consumption Testing: Evaluate and optimize energy use to hit the 30-day target

    🔐 Secure Transmission: Implement SSL encryption for secure data streaming

    🎞 Video Compression: Integrate H.265 hardware acceleration to reduce bandwidth and power usage
