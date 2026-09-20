# Computer Vision Final Project: Plaza de Ferrandiz i Carbonell People Tracking

**Author:** Giorgi Gogsadze

**Date:** 27.06.2025

[**Project Documentation**](Final_Project_Report_Giorgi_Gogsadze.pdf)

[**Project Results**](https://drive.google.com/file/d/1R8IAv8MEyCfA96IDT4l1iQQClRTwv9AQ/view?usp=sharing)

## 📌 Project Overview

This project analyzes video feeds from a static webcam located in the Ferrándiz building to monitor pedestrian activity in the square and at the Carbonell building entrance [cite: 4]. The system accomplishes two primary tasks:

- **Square Occupancy:** Detects and counts the number of people present in the plaza at any given time [cite: 4].
- **Access Monitoring:** Tracks individuals entering and exiting the Carbonell building doors to estimate the current indoor occupancy [cite: 4].

## 🛠️ Technology Stack & User Interface

- **Core Architecture:** Developed as a C++ MFC (Microsoft Foundation Classes) dialog-based application [cite: 4].
- **Computer Vision:** Utilizes the OpenCV library for all primary image processing and tracking operations [cite: 4].
- **Application UI:** Allows users to upload video files, apply predefined static masks, or interactively draw custom masks directly within the application [cite: 4].

## 🔲 Mask Generation Strategy

The system relies on binary masks to isolate the doors and the main square [cite: 4]. Static masks were chosen over dynamic detection due to the fixed camera setup and a lack of consistent visual or geometric cues in the video frame [cite: 4].

- **Interactive Creation:** Users can draw polygons on the first video frame using a mouse callback function (`setMouseCallback`) [cite: 4]. The application connects these points and fills the polygon (`fillPoly`) to generate a binary mask [cite: 4].
- **Mask Processing:** Custom masks are converted to grayscale, thresholded, and cleaned using morphological open and close operations to ensure precise boundaries [cite: 4].

## 🚶 Tracking Pipeline

The application implements a custom multi-object tracking approach to identify and maintain human trajectories [cite: 4]:

- **Background Subtraction:** Uses the MOG2 (Mixture of Gaussians) algorithm to isolate moving objects, skipping the first 10 frames to allow the background model to stabilize [cite: 4].
- **Foreground Processing:** Applies Gaussian blur, binary thresholding, and morphological operations to reduce noise and fill holes in detected blobs [cite: 4].
- **Contour Detection & Filtering:** Extracts contours and filters them based on area (minimum 120, maximum 9000) and compactness [cite: 4]. This successfully prevents large non-human objects, such as trucks, from being tracked [cite: 4].
- **Track Association:** Matches current detections to existing tracks by calculating the minimum distance between centroids, utilizing a dynamic threshold that starts at 50 pixels [cite: 4].
- **Lifecycle Management:** Tracks are not deleted immediately upon occlusion; they are marked as missing and only removed after 3 consecutive missing frames [cite: 4].

## 🧮 Counting & Flow Logic

- **Proximity Estimation:** Because objects closer to the camera appear larger, the system estimates the number of people within a single contour based on its area and vertical position (y-coordinate) [cite: 4].
- **Square Counting:** Evaluates the centroid of each tracked object against the static `areaMask` [cite: 4]. If the centroid lands on a pixel value greater than 128 (inside the mask), the estimated number of people in that track is added to the plaza count [cite: 4].
- **Entry/Exit Detection:** Utilizes a separate `doorsMask` and monitors the bottom center point (`middleDown`) of the tracked bounding box [cite: 4].
  - **Entry:** Logged if the previous `middleDown` point was outside the mask (pixel < 128) and the current point is inside (pixel > 128) [cite: 4].
  - **Exit:** Logged if the previous point was inside and the current point moves outside [cite: 4].

## 📊 Results & Demonstration

The tracking and counting logic proved highly accurate across various test cases, successfully handling grouped movements and excluding vehicular noise [cite: 4]. The precise boundary crossing logic (using the bottom center point rather than the centroid) ensures that individuals merely walking close to the doors are not falsely counted as entering or exiting [cite: 4].
