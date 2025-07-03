# 🐍 Snake Game (C++ & raylib)

This is a classic **Snake Game** implemented in **C++**, using the **[raylib](https://www.raylib.com/)** library for graphics and input handling.  
The project was created as a learning experience to improve my understanding of game loops, graphics rendering, and event handling in C++.

---

## 🎮 Gameplay

- Control the snake with the arrow keys.
- Eat the food to grow your snake.
- Don't hit the walls or yourself.
- Simple score tracking.

---

## 🛠️ Technologies Used

- **C++** — Core game logic and application structure.
- **raylib** — Simple and easy-to-use library to handle graphics, input, and window management.

---

## 📚 Credits

The game is **heavily inspired by the tutorial by @Robinson Technologies** on YouTube:  
👉 [ C++ Snake game using raylib - Beginner Tutorial 🐍 (OOP) ](https://www.youtube.com/watch?v=LGqsnM_WEK4)

I used this tutorial as a starting point and built upon it to create my own version.

---

## 🚀 How to Run

### ✅ Requirements
- **C++ compiler** (e.g., g++, clang++)
- **raylib library** installed on your system

### 🔨 Build (example using g++)
```bash
g++ main.cpp -o snake -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
