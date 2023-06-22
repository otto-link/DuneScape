// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// MIT License. The full license is in the file LICENSE, distributed
// with this software.
#define _USE_MATH_DEFINES
#include <cmath>

#include "macrologger.h"

#include "core/array.hpp"
#include "core/dunefield.hpp"

namespace dunescape
{

DuneField::DuneField(std::vector<int> shape) : shape(shape)
{
  this->h = Array(shape);
  this->shadow = Array(shape);
  this->gen.seed(this->seed);
}

void DuneField::cycle()
{
  std::uniform_real_distribution<float> dis(0.f, 1.f);
  std::uniform_int_distribution<int>    dis_ij(0, this->shape[0] - 1);

#pragma omp parallel for
  for (int i = 0; i < this->shape[0]; i++)
    for (int j = 0; j < this->shape[1]; j++)
    {
      // shuffle cell indices to avoid artifacts
      i = (i + dis_ij(this->gen)) % this->shape[0];
      j = (j + dis_ij(this->gen)) % this->shape[1];

      if ((this->h(i, j) > 0) and (this->shadow(i, j) == 0))
      {
        // remove slab from initial cell
        this->depose_at(i, j, -1, this->di_up, this->dj_up);

        // keep moving the cell downwind by 'hop_length' jumps until
        // it deposits
        bool keep_hopping = true;
        int  ic = i;

        while (keep_hopping)
        {
          ic = (ic + this->hop_length) % this->shape[0];

          if (this->shadow(ic, j) == 1)
          {
            this->depose_at(ic, j, 1, this->di_down, this->dj_down);
            keep_hopping = false;
          }
          else
          {
            float rd = dis(this->gen);
            if (((this->h(ic, j) == 0) and (rd < this->prob_deposit_bare)) or
                (rd < this->prob_deposit_sand))
            {
              this->depose_at(ic, j, 1, this->di_down, this->dj_down);
              keep_hopping = false;
            }
          }
        }
      }
    }
}

void DuneField::depose_at(int               i,
                          int               j,
                          int               amount,
                          std::vector<int> &di,
                          std::vector<int> &dj)
{
  // loop over the neighbors
  int p = i;
  int q = j;
  
  for (size_t k = 0; k < di.size(); k++)
  {
    int in = (i + di[k] + this->shape[0]) % this->shape[0];
    int jn = (j + dj[k] + this->shape[1]) % this->shape[1];

    if (amount * (this->h(i, j) - this->h(in, jn)) > 2)
    {
      p = in;
      q = jn;
      break;
    }
  }
  this->h(p, q) = this->h(p, q) + amount;
}

void DuneField::update_shadow()
{
  int kmax = 1 + (int)((float)this->h.max() / this->shadow_slope);
  std::vector<float> dh(kmax);

#pragma omp parallel for
  for (int i = 0; i < this->shape[0]; i++)
    for (int j = 0; j < this->shape[1]; j++)
    {
      this->shadow(i, j) = 0;
      float slope = this->shadow_slope;
      int   k = 1;
      float dh = -1.f;

      // look for a cell upstream that would cast a shadow over the
      // current cell
      while ((k < kmax) and (dh <= 0.f))
      {
        int k_shifted = (i - k + this->shape[0]) % this->shape[0];
        dh = (float)this->h(k_shifted, j) - (float)this->h(i, j) - slope;
        slope += this->shadow_slope;
        k++;
      }

      if (dh > 0.f)
        this->shadow(i, j) = 1;
    }
}

} // namespace dunescape
