# OpenCV/OpenGL Video Filter Lab

This project is an interactive C++ application for real-time video processing and visualization. It allows you to apply various image filters and geometric transformations to webcam video, using either CPU (OpenCV) or GPU (OpenGL) rendering. You can compare performance and visual results between the two implementations.


## Controls
As mentioned at the lecture, I implemented transformations with keyboard input instead of mouse input. Below are all the keyboard controls:
| Key         | Action                                 |
|-------------|----------------------------------------|
| `g`         | Toggle between OpenCV and OpenGL modes |
| `1`         | Toggle grayscale filter                |
| `2`         | Toggle Gaussian blur (only cpu)        |
| `3`         | Toggle edge detection (only cpu)       |
| `4`         | Toggle pixelation                      |
| `i` / `o`   | Scale up / down                        |
| Arrow keys  | Rotate / translate quad                |
| `Esc`       | Exit                                   |


2. **Run**
cd into one of the build folders
depending on the folder cd into either the debug or release folders
run "./OpenCVLab" in terminal

