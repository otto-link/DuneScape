// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// MIT License. The full license is in the file LICENSE, distributed
// with this software.

/**
 * @file array.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-06-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace dunescape
{

/**
 * @brief Array class, helper to manipulate 2D int array with "(i, j)"
 * indexing.
 *
 */
class Array
{
public:
  /**
   * @brief Array shape {ni, nj}.
   *
   */
  std::vector<int> shape;

  /**
   * @brief Vector for data storage, size shape[0] * shape[1].
   *
   */
  std::vector<int> vector;

  /**
   * @brief Construct a new Array object.
   *
   * @param shape Array shape {ni, nj}.
   */
  Array(std::vector<int> shape);

  /**
   * @brief Call overloading, return array value at index (i, j).
   *
   * @param i 'i' index.
   * @param j 'j' index.
   * @return int& Array value at index (i, j).
   */
  inline int &operator()(int i, int j)
  {
    return this->vector[i * this->shape[1] + j];
  }

  inline const int &operator()(int i, int j) const ///< @overload
  {
    return this->vector[i * this->shape[1] + j];
  }

  /**
   * @brief Set the array shape.
   *
   * @param new_shape New shape.
   */
  void set_shape(std::vector<int> new_shape)
  {
    this->shape = new_shape;
    this->vector.resize(this->shape[0] * this->shape[1]);
  }

  /**
   * @brief Display a bunch of infos on the array.
   *
   */
  void infos() const;

  /**
   * @brief Return the value of the greastest element in the array.
   *
   * @return int
   */
  inline int max() const
  {
    return *std::max_element(this->vector.begin(), this->vector.end());
  }

  /**
   * @brief Return the value of the smallest element in the array.
   *
   * @return int
   */
  inline int min() const
  {
    return *std::min_element(this->vector.begin(), this->vector.end());
  }

  /**
   * @brief Fill the array with white noise.
   *
   * @param a Lower bound of random distribution.
   * @param b Upper bound of random distribution.
   * @param seed Random number seed.
   */
  void randomize(int a, int b, uint seed);

  /**
   * @brief Convert array to a 8 bit grayscale image.
   *
   * @return std::vector<uint8_t> Image.
   */
  std::vector<uint8_t> to_img_8bit_grayscale();

  /**
   * @brief Convert array to a 8 bit RGB image using the `nipy spectral`
   * colormap.
   *
   * @return std::vector<uint8_t> Image.
   */
  std::vector<uint8_t> to_img_8bit_nipy();

  /**
   * @brief Export array as png image file.
   *
   * @param fname File name.
   */
  void to_png(std::string fname);
};

} // namespace dunescape
