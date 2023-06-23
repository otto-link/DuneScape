// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// MIT License. The full license is in the file LICENSE, distributed
// with this software.

/**
 * @file dunefield.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-06-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <random>

#include "core/array.hpp"

#define SHADOW_SLOPE 0.8038475772933681f // == 3 * tan(15 / 180 * pi)
// clang-format off
#define DI_MOORE_DOWN { 1,  0,  0,  1,  1}
#define DJ_MOORE_DOWN { 0,  1, -1, -1,  1}
#define DI_MOORE_UP   {-1,  0,  0, -1, -1}
#define DJ_MOORE_UP   { 0,  1, -1, -1,  1}
// clang-format on

namespace dunescape
{

class Array;

/**
 * @brief DuneField class.
 *
 */
class DuneField
{
public:
  /**
   * @brief Dune field underlying array shape {ni, nj}.
   *
   */
  std::vector<int> shape;

  /**
   * @brief Shadow slope.
   *
   */
  float shadow_slope = SHADOW_SLOPE;

  /**
   * @brief Hop length.
   *
   */
  int hop_length = 1;

  /**
   * @brief Probability of sand slab deposit on bare ground.
   *
   */
  float prob_deposit_bare = 0.4f;

  /**
   * @brief Probability of sand slab deposit on sandy ground.
   *
   */
  float prob_deposit_sand = 0.6f;

  /**
   * @brief Number of sand slabs.
   *
   */
  Array h = Array({0, 0});

  /**
   * @brief Define wether a cell is in the shadow (1) or not (0) of
   * other cells.
   *
   */
  Array shadow = Array({0, 0});

  /**
   * @brief Random seed number.
   *
   */
  uint seed = 1;

  /**
   * @brief Construct a new Array object.
   *
   * @param shape Array shape {ni, nj}.
   */
  DuneField(std::vector<int> shape);

  /**
   * @brief Set the dunefield shape.
   *
   * @param new_shape New shape.
   */
  void set_shape(std::vector<int> new_shape)
  {
    this->shape = new_shape;
    this->h.set_shape(new_shape);
    this->shadow.set_shape(new_shape);
  }

  /**
   * @brief Perform one simulation cycle.
   *
   */
  void cycle();

  /**
   * @brief Depose/erode one sand slab at location `(i, j){.}
   *
   * @param i Index.
   * @param j Index.
   * @param amount +- 1
   * @param di Avalanching coefficients for neighbor search.
   * @param dj Avalanching coefficients for neighbor search.
   */
  void depose_at(int               i,
                 int               j,
                 int               amount,
                 std::vector<int> &di,
                 std::vector<int> &dj);

  /**
   * @brief Update shadow field.
   *
   */
  void update_shadow();

  void update_shadow(int i, int j); ///< @overload

private:
  std::mt19937     gen;
  std::vector<int> di_down = DI_MOORE_DOWN;
  std::vector<int> dj_down = DJ_MOORE_DOWN;
  std::vector<int> di_up = DI_MOORE_UP;
  std::vector<int> dj_up = DJ_MOORE_UP;
};

} // namespace dunescape
