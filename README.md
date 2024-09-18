# PoolBallP3D

PoolBallP3D is a 3D scene developed using OpenGL, featuring a snooker table setup. This project showcases various OpenGL techniques, including lighting, camera control, and interactive physics.

---

## Features

- **Snooker Table:**  
  A snooker table where each side is rendered in a distinct color.
  
- **15 Balls:**  
  The scene includes 15 snooker balls, positioned realistically and randomly within the table.

- **Camera Control:**  
  Users can freely navigate the scene with mouse controls:  
  - **Move the camera** to explore the table from different angles;  
  - **Zoom in/out** for a closer or wider view of the table.

- **Dynamic Lighting Modes:**  
  The project features 4 distinct lighting modes, which can be toggled for different visual effects:
  - **Ambient Light**;
  - **Directional Light**; 
  - **Point Light**;
  - **Spot Light**.

- **Interactive Ball Movement:**  
  By pressing the **space bar**, a ball is set in motion across the table. The ball will roll in a specified direction until:  
  - It **collides** with another ball;  
  - It **reaches** the edge of the table.

---

## Controls

- **Mouse:**  
  - Move the camera;  
  - Zoom in/out.

- **Keyboard:**  
  - `Space Bar`: Move a ball in the scene;  
  - Press `1` to `4`: Switch between lighting modes:
    - `1`: Ambient Light;  
    - `2`: Directional Light;  
    - `3`: Point Light;  
    - `4`: Spot Light;

---

## Images
<img src=https://github.com/user-attachments/assets/6a4f3b4b-90e1-4ee1-9268-774323c4444e alt="Ambient Light" width="500"/> <img src=https://github.com/user-attachments/assets/97f01974-3f99-4cef-ba77-bfad9dc8ec4a alt="Directional Light" width="500"/> <br>
<img src=https://github.com/user-attachments/assets/c9355e56-f0f8-4946-9e69-df3b2a294d17 alt="Point Light" width="500"/> <img src=https://github.com/user-attachments/assets/ef3c93e8-c1a5-4301-84f3-aa8bda964393 alt="Spot Light" width="500"/>

---

## Note

Be aware, the project contains a `.zip` file with the object files for the balls. These files should be placed inside the `textures` folder.

---

This project was developed for academic purposes as part of the **3rd Year 3D Programming** class at IPCA, 2024.
