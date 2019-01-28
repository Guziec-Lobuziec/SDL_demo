#ifndef PHYSICS_RECIPIENT_HPP
#define PHYSICS_RECIPIENT_HPP

#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>

#include "alg.hpp"

class physics_recipient {
public:

  physics_recipient(
    double mass,
    point<double> position,
    double time_resolution,
    std::vector<std::tuple<point<double>,double>> collision_map,
    double coefficient_of_restitution,
    bool infinite_mass
  );

  point<double> get_position() const;

  alg_vector<double> get_velocity() const;

  alg_vector<double> get_acceleration() const;

  physics_recipient& set_position(const point<double>& position);

  physics_recipient& set_velocity(const alg_vector<double>& velocity);

  physics_recipient& set_acceleration(const alg_vector<double>& acceleration);

  physics_recipient& tick_physics();

  physics_recipient& compute_collisons(physics_recipient & element);

private:
  alg_vector<double> _acceleration;
  alg_vector<double> _velocity;
  point<double> _position;
  double _mass;
  double _time_resolution;
  double _coefficient_of_restitution;
  std::vector<std::tuple<point<double>,double>> _collision_map;
  bool _infinite_mass;

};

#endif
