#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <unistd.h>

float A, B, C;

float cubeWidth = 20;
int width = 160, height = 44;
std::vector<float> zBuffer(160 * 44);
std::vector<char> buffer(160 * 44);
int backgroundASCIICode = ' ';
int distanceFromCam = 100;
float K1 = 40;

float incrementSpeed = 0.6;

float x, y, z;
float ooz;
int xp, yp;
int idx;

float calculateX(int i, int j, int k) {
  return j * std::sin(A) * std::sin(B) * std::cos(C) - k * std::cos(A) * std::sin(B) * std::cos(C) +
         j * std::cos(A) * std::sin(C) + k * std::sin(A) * std::sin(C) + i * std::cos(B) * std::cos(C);
}

float calculateY(int i, int j, int k) {
  return j * std::cos(A) * std::cos(C) + k * std::sin(A) * std::cos(C) -
         j * std::sin(A) * std::sin(B) * std::sin(C) + k * std::cos(A) * std::sin(B) * std::sin(C) -
         i * std::cos(B) * std::sin(C);
}

float calculateZ(int i, int j, int k) {
  return k * std::cos(A) * std::cos(B) - j * std::sin(A) * std::cos(B) + i * std::sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {
  x = calculateX(cubeX, cubeY, cubeZ);
  y = calculateY(cubeX, cubeY, cubeZ);
  z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;

  ooz = 1 / z;

  xp = static_cast<int>(width / 2 + K1 * ooz * x * 2);
  yp = static_cast<int>(height / 2 + K1 * ooz * y);

  idx = xp + yp * width;
  if (idx >= 0 && idx < width * height) {
    if (ooz > zBuffer[idx]) {
      zBuffer[idx] = ooz;
      buffer[idx] = ch;
    }
  }
}

int main() {
  std::cout << "\x1b[2J";
  while (true) {
    std::fill(buffer.begin(), buffer.end(), backgroundASCIICode);
    std::fill(zBuffer.begin(), zBuffer.end(), 0);

    for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
      for (float cubeY = -cubeWidth; cubeY < cubeWidth;
           cubeY += incrementSpeed) {
        calculateForSurface(cubeX, cubeY, -cubeWidth, '@');
        calculateForSurface(cubeWidth, cubeY, cubeX, '$');
        calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');
        calculateForSurface(-cubeX, cubeY, cubeWidth, '#');
        calculateForSurface(cubeX, -cubeWidth, -cubeY, ';');
        calculateForSurface(cubeX, cubeWidth, cubeY, '+');
      }
    }
    printf("\x1b[H");
    for (int k = 0; k < width * height; k++) {
      putchar(k % width ? buffer[k] : 10);
    }

    A += 0.05;
    B += 0.05;
    C += 0.01;
    usleep(8000 * 2);
  }
  return 0;
}
