# Activity Tracking Wristband

## Overview

The activity tracking wristband is a custom-built device designed to monitor and detect human physical activities in real-time. It works in tandem with a dedicated mobile application to provide accurate visualization outcomes, such as standing, walking, running, or unknown movements.

## Hardware Components

- **Development Board**: ESP32-DevKitC-V4
- **Accelerometer**: 3-axis LIS2DW12

## Features

- **Artificial Neural Network**: Implemented on the ESP32 platform to classify different types of physical activities based on accelerometer data.
- **Real-Time Activity Recognition**: Determines the current activity being performed by the user.
- **Wireless Communication**: Connects to the mobile application via Bluetooth Low Energy (BLE).
- **Edge Impulse Integration**: Utilizes Edge Impulse service for training the neural network with data collected from human movements.

## Software Details

- **Programming Language**: C
- **Machine Learning**: The neural network is trained on patterns obtained from human motion data.
- **BLE Communication**: Enables seamless data transfer between the wristband and the mobile application.

## How It Works

1. **Data Collection**: The 3-axis accelerometer collects movement data from the user's wrist.
2. **Neural Network Processing**: The on-board neural network processes the accelerometer data to classify the activity.
3. **Data Transmission**: The classified activity data is sent to the mobile application via BLE.
4. **User Feedback**: The mobile application displays the detected activity and logs it for analysis.

## Training the Neural Network

- **Edge Impulse Service**: Used to train the neural network with datasets collected from various human activities.
- **Model Deployment**: The trained model is deployed onto the ESP32 board for on-device inference.

## Integration with Mobile Application

- **BLE Connectivity**: Establishes a connection with the Android mobile application.
- **Data Visualization**: The app visualizes the activity data received from the wristband.
- **Synchronization**: Works offline and synchronizes data with the backend when an internet connection is available.

## Potential Applications

- **Fitness Tracking**: Monitor workouts and physical activities.
- **Health Monitoring**: Keep track of daily movements for health assessments.
- **Research and Development**: Can be used as a platform for further development in activity recognition technologies.
