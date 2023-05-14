///////////////////////////////////////////////////////////////////////////////
// cranes_algs.hpp
//
// Algorithms that solve the crane unloading problem.
//
// All of the TODO sections for this project reside in this file.
//
// This file builds on crane_types.hpp, so you should familiarize yourself
// with that file before working on this file.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cassert>
#include <math.h>

#include "cranes_types.hpp"

namespace cranes {

// Solve the crane unloading problem for the given grid, using an exhaustive
// optimization algorithm.
//
// This algorithm is expected to run in exponential time, so the grid's
// width+height must be small enough to fit in a 64-bit int; this is enforced
// with an assertion.
//
// The grid must be non-empty.
path crane_unloading_exhaustive(const grid& setting) {

  // grid must be non-empty.
  assert(setting.rows() > 0);
  assert(setting.columns() > 0);

  // Compute maximum path length, and check that it is legal.
  const size_t max_steps = setting.rows() + setting.columns() - 2;
  assert(max_steps < 64);

  path best(setting);
  for (size_t steps = 0; steps <= pow(2, max_steps); steps++) {
    path current_path(setting);
    std::vector<step_direction> count_steps;
    for(size_t i = 0; i < max_steps; i++) {
      int iter = (steps >> i) & 1;
      count_steps.push_back((iter == 1) ? STEP_DIRECTION_SOUTH : STEP_DIRECTION_EAST);
    }
    for(step_direction step: count_steps) {
      if(!current_path.is_step_valid(step)) {
        break;
      }
      current_path.add_step(step);
    }  
    if(current_path.total_cranes() > best.total_cranes()) {
      best = current_path;
    }
  }
return best;
}

// Solve the crane unloading problem for the given grid, using a dynamic
// programming algorithm.
//
// The grid must be non-empty.
//path crane_unloading_dyn_prog(const grid& setting) {
path crane_unloading_dyn_prog(const grid& setting) {

  // grid must be non-empty.
  assert(setting.rows() > 0);
  assert(setting.columns() > 0);

  
  using cell_type = std::optional<path>;

  std::vector<std::vector<cell_type> > A(setting.rows(),
                                        std::vector<cell_type>(setting.columns()));

  A[0][0] = path(setting);
  assert(A[0][0].has_value());

  for (coordinate r = 0; r < setting.rows(); ++r) {
    for (coordinate c = 0; c < setting.columns(); ++c) {

      if (setting.get(r, c) == CELL_BUILDING){
        A[r][c].reset();
        continue;
      }

    cell_type from_above = std::nullopt;
    cell_type from_left = std::nullopt;

    // TODO: implement the dynamic programming algorithm, then delete this
    // comment.
      if (r > 0 && setting.get(r - 1, c) != CELL_BUILDING && A[r - 1][c].has_value()) {
        from_above = A[r - 1][c].value();
        from_above->add_step(STEP_DIRECTION_SOUTH);
      }

      if (c > 0 && setting.get(r, c - 1) != CELL_BUILDING && A[r][c - 1].has_value()) {
        from_left = A[r][c - 1].value();
        from_left->add_step(STEP_DIRECTION_EAST);
      }

      if (from_above.has_value() && from_left.has_value()) {
        A[r][c] = from_above->total_cranes() >= from_left->total_cranes() ? from_above : from_left;
      } else if (from_above.has_value() && !from_left.has_value()) {
        A[r][c] = from_above;
      } else if (!from_above.has_value() && from_left.has_value()) {
        A[r][c] = from_left;
      }
    }
  }

  coordinate row_max = 0;
  coordinate col_max = 0;
  unsigned int max_cranes = 0;

  for (coordinate row = 0; row < setting.rows(); ++row) {
    for (coordinate col = 0; col < setting.columns(); ++col) {
      if (A[row][col].has_value() && A[row][col]->total_cranes() > max_cranes) {
        row_max = row;
        col_max = col;
        max_cranes = A[row][col]->total_cranes();
      }
    }
  }

  cell_type *best = &A[row_max][col_max];

  assert(best->has_value());
//  //   std::cout << "total cranes" << (**best).total_cranes() << std::endl;

   return **best;
   }
 }
