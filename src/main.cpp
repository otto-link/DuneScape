// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// MIT License. The full license is in the file LICENSE, distributed
// with this software.

#include <iostream>
#include <string>
#include <vector>
#include <omp.h>

#include "core/array.hpp"
#include "core/dunefield.hpp"

std::string zfill(int i, int n_zeros)
{
  std::string str = std::to_string(i);
  return std::string(n_zeros - std::min(n_zeros, (int)str.length()), '0') + str;
}
  
int main()
{
  omp_set_num_threads(omp_get_max_threads());

  std::cout << "nthread max: " << omp_get_max_threads() << std::endl;
  std::cout << "nthread: " << omp_get_num_threads() << std::endl;
    
  std::vector<int> shape = {256, 256};

  dunescape::DuneField df = dunescape::DuneField(shape);
  df.h.randomize(0, 20, 1);

  for (int it = 0; it < 500; it++)
  {
    std::cout << it << "\n";
    df.update_shadow();
    df.cycle();

    if (it % 50 == 0)
    {
      df.h.infos();
      df.h.to_png("h" + zfill(it, 4) + ".png");
    }
  }

  df.h.to_png("h.png");
  df.shadow.to_png("shadow.png");

  return 0;
}
