# K-Means Clustering Visualizer with SDL2

This project implements the **K-Means clustering algorithm** in C and provides two interactive visualizations using [SDL2](https://libsdl.org/):
- **Manual/Test Mode** – draw or generate points on a canvas and watch clustering step-by-step.
- **MNIST Mode** – cluster the MNIST handwritten digits dataset and visualize the resulting centroids as images.

---

## 🧠 Features

- Basic K-Means implementation (`kmeans.c`)
- Optional centroid initialization with KMeans++
- Live visualization using `SDL2`
- MNIST reader for digit clustering
- Auto-generated centroid images (as `.bmp`)
- Interactive keyboard/mouse controls

---

## 🗃 File Structure

| File             | Description |
|------------------|-------------|
| `kmeans.c/h`     | Core K-Means logic |
| `test.c`         | Interactive demo: generate or draw points and cluster them |
| `mnist_img.c`    | Clusters the MNIST images and saves centroids as images |
| `run.sh`         | Shell script to compile and run (interactive menu or CLI arg) |
| `train-images-idx3-ubyte` | Raw MNIST image file (not included – download separately) |

---

## 🧪 Usage

### 📦 Requirements

- GCC or Clang
- SDL2 development libraries (`libsdl2-dev` on Ubuntu/Debian)

### 🖥 Run via Shell Script

```bash
./run.sh

