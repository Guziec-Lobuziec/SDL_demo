#include "physics_recipient.hpp"

physics_recipient::physics_recipient(
  double mass,
  point<double> position,
  double time_resolution,
  std::vector<std::tuple<point<double>,double>> collision_map,
  double coefficient_of_restitution,
  double drag_amplitude,
  bool infinite_mass
):
_mass(mass),
_collision_map(collision_map),
_acceleration(alg_vector<double>(0,0)),
_velocity(alg_vector<double>(0,0)),
_position(position),
_time_resolution(time_resolution),
_coefficient_of_restitution(coefficient_of_restitution),
_drag_amplitude(drag_amplitude),
_infinite_mass(infinite_mass)
{
}

point<double> physics_recipient::get_position() const {
  return _position;
}

alg_vector<double> physics_recipient::get_velocity() const {
  return _velocity;
}

alg_vector<double> physics_recipient::get_acceleration() const {
  return _acceleration;
}

physics_recipient& physics_recipient::set_position(const point<double>& position) {
  _position = position;
  return *this;
}

physics_recipient& physics_recipient::set_velocity(const alg_vector<double>& velocity) {
  _velocity = velocity;
  return *this;
}

physics_recipient& physics_recipient::set_acceleration(const alg_vector<double>& acceleration) {
  _acceleration = acceleration;
  return *this;
}

physics_recipient& physics_recipient::tick_physics() {

  if(!_infinite_mass) {
    _velocity = _velocity*(1-_drag_amplitude) + _acceleration*_time_resolution;
    _position = _position + _velocity*_time_resolution;
  }

  return *this;

}

physics_recipient& physics_recipient::compute_collisons(physics_recipient& element) {

  auto intersection_check = [](
    const std::tuple<point<double>,double> & t1,
    const std::tuple<point<double>,double> & t2
  ) -> bool {
    double distance = d_to_2(std::get<0>(t1), std::get<0>(t2));
    return
    (std::pow(std::get<1>(t1) - std::get<1>(t2), 2) <= distance) &&
    (std::pow(std::get<1>(t1) + std::get<1>(t2), 2) >= distance);
  };

    std::for_each(_collision_map.begin(), _collision_map.end(), [&](auto st_collision_box_rel) {

      std::for_each(
        element._collision_map.begin(),
        element._collision_map.end(),
        [&](auto nd_collision_box_rel) {

            auto st_collision_box = std::make_tuple(
              std::get<0>(st_collision_box_rel)+_position,
              std::get<1>(st_collision_box_rel)
            );
            auto nd_collision_box = std::make_tuple(
              std::get<0>(nd_collision_box_rel)+element._position,
              std::get<1>(nd_collision_box_rel)
            );

            if(intersection_check(st_collision_box, nd_collision_box)) {

                alg_vector<double> st_position_vect(
                  std::get<0>(st_collision_box).x(),
                  std::get<0>(st_collision_box).y()
                );
                alg_vector<double> nd_position_vect(
                  std::get<0>(nd_collision_box).x(),
                  std::get<0>(nd_collision_box).y()
                );

                alg_vector<double> new_velocity_st(0,0);
                alg_vector<double> new_velocity_nd(0,0);

                if(_infinite_mass || element._infinite_mass) {

                  if(_infinite_mass && element._infinite_mass) {
                    //do nothing
                  } else if(element._infinite_mass) {
                    new_velocity_st =
                      _velocity -
                      (st_position_vect - nd_position_vect) *
                      2 *
                      (
                        dot(_velocity , st_position_vect - nd_position_vect) /
                        abs_to_2(st_position_vect - nd_position_vect)
                      );
                  } else if(_infinite_mass) {
                    new_velocity_nd =
                      element._velocity -
                      (nd_position_vect - st_position_vect) *
                      2 *
                      (
                        dot(element._velocity, nd_position_vect - st_position_vect) /
                        abs_to_2(nd_position_vect - st_position_vect)
                      );
                  }

                } else {
                  new_velocity_st =
                    _velocity -
                    (st_position_vect - nd_position_vect) *
                    (2 * element._mass / (element._mass + _mass)) *
                    (
                      dot(_velocity - element._velocity, st_position_vect - nd_position_vect) /
                      abs_to_2(st_position_vect - nd_position_vect)
                    );

                  new_velocity_nd =
                    element._velocity -
                    (nd_position_vect - st_position_vect) *
                    (2 * _mass / (element._mass + _mass)) *
                    (
                      dot(element._velocity - _velocity, nd_position_vect - st_position_vect) /
                      abs_to_2(nd_position_vect - st_position_vect)
                    );
                }

                set_velocity(new_velocity_st);
                element.set_velocity(new_velocity_nd);

            }
        });

    });

  return *this;
}
