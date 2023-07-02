// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// MIT License. The full license is in the file LICENSE, distributed
// with this software.

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define IMG_CHANNELS 3
#include "stb_image_write.h"

#include "core/array.hpp"

namespace dunescape
{

Array::Array(std::vector<int> shape) : shape(shape)
{
  this->vector.resize(this->shape[0] * this->shape[1]);
}

void Array::infos() const
{
  std::cout << "Array:";
  std::cout << "address: " << this << ", ";
  std::cout << "shape: {" << this->shape[0] << ", " << this->shape[1] << "}"
            << ", ";
  std::cout << "min: " << this->min() << ", ";
  std::cout << "max: " << this->max();
  std::cout << std::endl;
}

void Array::randomize(int a, int b, uint seed)
{
  std::mt19937                       gen(seed);
  std::uniform_int_distribution<int> dis(a, b);
  for (auto &v : this->vector)
    v = dis(gen);
}

std::vector<uint8_t> Array::to_img_8bit_grayscale()
{
  std::vector<uint8_t> data(this->shape[0] * this->shape[1]);
  const int            vmax = this->max();

  if (vmax > 0) // export a black image if not
  {
    // reorganize things to get an image with (i, j) used as (x, y)
    // coordinates, i.e. with (0, 0) at the bottom left
    float a = 1.f / (float)vmax;
    int   k = 0;

    for (int j = this->shape[1] - 1; j > -1; j--)
      for (int i = 0; i < this->shape[0]; i++)
      {
        float   v = a * (float)(*this)(i, j);
        uint8_t c = (uint8_t)std::floor(255 * v);
        data[k++] = c;
      }
  }
  return data;
}

std::vector<uint8_t> Array::to_img_8bit_nipy()
{
  std::vector<uint8_t> data(this->shape[0] * this->shape[1] * 3);
  const int            vmax = this->max();

  std::vector<std::vector<float>> colors = {
      {0.000f, 0.000f, 0.000f},
      {0.521f, 0.000f, 0.588f},
      {0.000f, 0.000f, 0.794f},
      {0.000f, 0.527f, 0.867f},
      {0.000f, 0.667f, 0.630f},
      {0.000f, 0.612f, 0.000f},
      {0.000f, 0.855f, 0.000f},
      {0.533f, 1.000f, 0.000f},
      {0.970f, 0.861f, 0.000f},
      {1.000f, 0.382f, 0.000f},
      {0.855f, 0.000f, 0.000f},
      {0.800f, 0.800f, 0.800f},
  };
  int nc = (int)colors.size();

  if (vmax > 0) // export a black image if not
  {
    float a = 1.f / (float)vmax;
    int   k = 0;

    for (int j = this->shape[1] - 1; j > -1; j--)
      for (int i = 0; i < this->shape[0]; i++)
      {
        std::vector<float> rgb(3);
        float              v = a * (float)(*this)(i, j); // in [0, 1]
        float              vc = v * (float)(nc - 1);     // in [0, nc - 1]
        int                ic = (int)vc;

        if (ic == nc - 1)
          rgb = colors[nc - 1];
        else
        {
          float t = vc - (float)ic;
          for (int p = 0; p < 3; p++)
            rgb[p] = (1.f - t) * colors[ic][p] + t * colors[ic + 1][p];
        }

        data[++k] = (uint8_t)std::floor(255 * rgb[1]);
        data[++k] = (uint8_t)std::floor(255 * rgb[2]);
        data[++k] = (uint8_t)std::floor(255 * rgb[0]);
      }
  }
  return data;
}

void Array::to_png(std::string fname)
{
  std::vector<uint8_t> data(IMG_CHANNELS * this->shape[0] * this->shape[1]);
  const int            vmin = this->min();
  const int            vmax = this->max();

  if (vmin != vmax) // export a black image if not
  {
    // reorganize things to get an image with (i, j) used as (x, y)
    // coordinates, i.e. with (0, 0) at the bottom left
    float a = 1.f / (float)(vmax - vmin);
    float b = vmin / (float)(vmax - vmin);
    int   k = 0;

    for (int j = this->shape[1] - 1; j > -1; j--)
      for (int i = 0; i < this->shape[0]; i++)
      {
        float   v = a * (float)(*this)(i, j) + b;
        uint8_t c = (uint8_t)std::floor(255 * v);
        data[k++] = c;
        data[k++] = c;
        data[k++] = c;
      }
  }

  // row and column are permutted
  stbi_write_png(fname.c_str(),
                 this->shape[0],
                 this->shape[1],
                 IMG_CHANNELS,
                 data.data(),
                 IMG_CHANNELS * this->shape[0]);
}

} // namespace dunescape
