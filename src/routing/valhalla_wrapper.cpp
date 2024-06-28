/*

This file is part of VROOM.

Copyright (c) 2015-2022, Julien Coupey.
All rights reserved (see LICENSE).

*/

#include "../../include/polylineencoder/src/polylineencoder.h"

#include "routing/valhalla_wrapper.h"
#include "utils/helpers.h"

namespace vroom::routing {

constexpr unsigned km_to_m = 1000;
constexpr unsigned polyline_precision = 5;
constexpr unsigned valhalla_polyline_precision = 6;

ValhallaWrapper::ValhallaWrapper(const std::string& profile,
                                 const Server& server)
  : HttpWrapper(profile,
                server,
                "sources_to_targets",
                "sources_to_targets",
                "sources_to_targets",
                "route",
                R"("directions_type":"none")") {
}

std::string ValhallaWrapper::get_matrix_query(
  const std::vector<Location>& locations) const {
  // Building matrix query for Valhalla.
  std::string query = "GET /" + _server.path + _matrix_service + "?json=";

  // List locations.
  std::string all_locations;
  for (auto const& location : locations) {
    all_locations += "{\"lon\":" + std::to_string(location.lon()) + "," +
                     "\"lat\":" + std::to_string(location.lat()) +
                     R"(,"type":"break_through"},)";

  }
  all_locations.pop_back(); // Remove trailing ','.

  query += "{\"sources\":[" + all_locations;
  query += "],\"targets\":[" + all_locations;
  query += R"(],"costing":")" + profile + "\"";
  query += R"(,"costing_options":{"truck":{)";
  query += R"("exclude_unpaved":1)";
  query += R"(,"maneuver_penalty":38)";
  query += R"(,"country_crossing_penalty":0)";
  query += R"(,"country_crossing_cost":600)";
  query += R"(,"length":21.5)";
  query += R"(,"width":2.6)";
  query += R"(,"height":4.11)";
  query += R"(,"weight":21.77)";
  query += R"(,"axle_load":9)";
  query += R"(,"hazmat":false)";
  query += R"(,"use_highways":1)";
  query += R"(,"use_tolls":1)";
  query += R"(,"use_ferry":1)";
  query += R"(,"ferry_cost":300)";
  query += R"(,"use_living_streets":1)";
  query += R"(,"use_tracks":1)";
  query += R"(,"private_access_penalty":450)";
  query += R"(,"ignore_closures":false)";
  query += R"(,"ignore_restrictions":false)";
  query += R"(,"ignore_access":false)";
  query += R"(,"closure_factor":9)";
  query += R"(,"service_penalty":15)";
  query += R"(,"service_factor":1)";
  query += R"(,"shortest":false)";
  query += R"(,"exclude_cash_only_tolls":false)";
  query += R"(,"top_speed":80)";
  query += R"(,"axle_count":5)";
  query += R"(,"fixed_speed":0)";
  query += R"(,"toll_booth_penalty":0)";
  query += R"(,"toll_booth_cost":15)";
  query += R"(,"gate_penalty":300)";
  query += R"(,"gate_cost":30)";
  query += R"(,"include_hov2":false)";
  query += R"(,"include_hov3":false)";
  query += R"(,"include_hot":false)";
  query += R"(,"disable_hierarchy_pruning":false)";
  query += R"(}})";

//  query += R"(,"costing_options":{"truck":{,,,,,,,,}})";
  query += "}";
  query += " HTTP/1.1\r\n";
  query += "Host: " + _server.host + "\r\n";
  query += "Accept: */*\r\n";
  query += "Connection: Close\r\n\r\n";

  return query;
}

std::string
ValhallaWrapper::get_route_query(const std::vector<Location>& locations) const {
  // Building matrix query for Valhalla.
  std::string query =
    "GET /" + _server.path + _route_service + "?json={\"locations\":[";

  for (auto const& location : locations) {
    query += "{\"lon\":" + std::to_string(location.lon()) + "," +
             "\"lat\":" + std::to_string(location.lat()) +
             R"(,"type":"break_through"},)";
  }
  query.pop_back(); // Remove trailing ','.

  query += R"(],"costing":")" + profile + "\"";
  query += R"(,"costing_options":{"truck":{)";
  query += R"("exclude_unpaved":1)";
  query += R"(,"maneuver_penalty":39)";
  query += R"(,"country_crossing_penalty":0)";
  query += R"(,"country_crossing_cost":600)";
  query += R"(,"length":21.5)";
  query += R"(,"width":2.6)";
  query += R"(,"height":4.11)";
  query += R"(,"weight":21.77)";
  query += R"(,"axle_load":9)";
  query += R"(,"hazmat":false)";
  query += R"(,"use_highways":1)";
  query += R"(,"use_tolls":1)";
  query += R"(,"use_ferry":1)";
  query += R"(,"ferry_cost":300)";
  query += R"(,"use_living_streets":1)";
  query += R"(,"use_tracks":1)";
  query += R"(,"private_access_penalty":450)";
  query += R"(,"ignore_closures":false)";
  query += R"(,"ignore_restrictions":false)";
  query += R"(,"ignore_access":false)";
  query += R"(,"closure_factor":9)";
  query += R"(,"service_penalty":15)";
  query += R"(,"service_factor":1)";
  query += R"(,"shortest":false)";
  query += R"(,"exclude_cash_only_tolls":false)";
  query += R"(,"top_speed":80)";
  query += R"(,"axle_count":5)";
  query += R"(,"fixed_speed":0)";
  query += R"(,"toll_booth_penalty":0)";
  query += R"(,"toll_booth_cost":15)";
  query += R"(,"gate_penalty":300)";
  query += R"(,"gate_cost":30)";
  query += R"(,"include_hov2":false)";
  query += R"(,"include_hov3":false)";
  query += R"(,"include_hot":false)";
  query += R"(,"disable_hierarchy_pruning":false)";
  query += R"(}})";
  query += "," + _routing_args;
  query += "}";

  query += " HTTP/1.1\r\n";
  query += "Host: " + _server.host + "\r\n";
  query += "Accept: */*\r\n";
  query += "Connection: Close\r\n\r\n";

  return query;
}

std::string ValhallaWrapper::build_query(const std::vector<Location>& locations,
                                         const std::string& service) const {
  assert(service == _matrix_service || service == _route_service);

  return (service == _matrix_service) ? get_matrix_query(locations)
                                      : get_route_query(locations);
}

void ValhallaWrapper::check_response(const rapidjson::Document& json_result,
                                     const std::vector<Location>&,
                                     const std::string& service) const {
  assert(service == _matrix_service || service == _route_service);

  if (constexpr unsigned HTTP_OK = 200;
      json_result.HasMember("status_code") &&
      json_result["status_code"].IsUint() &&
      json_result["status_code"].GetUint() != HTTP_OK) {
    // Valhalla responses seem to only have a status_code key when a
    // problem is encountered. In that case it's not really clear what
    // keys can be expected so we're playing guesses. This happens
    // e.g. when requested matrix/route size goes over the server
    // limit.
    std::string service_str = (service == _route_service) ? "route" : "matrix";
    std::string error = "Valhalla " + service_str + " error (";

    if (json_result.HasMember("error") && json_result["error"].IsString()) {
      error += json_result["error"].GetString();
      error += ").";
    }
    throw RoutingException(error);
  }

  if (service == _route_service) {
    assert(json_result.HasMember("trip") &&
           json_result["trip"].HasMember("status"));
    if (json_result["trip"]["status"] != 0) {
      throw RoutingException(
        std::string(json_result["trip"]["status_message"].GetString()));
    }
  }
}

bool ValhallaWrapper::duration_value_is_null(
  const rapidjson::Value& matrix_entry) const {
  assert(matrix_entry.HasMember("time"));
  return matrix_entry["time"].IsNull();
}

bool ValhallaWrapper::distance_value_is_null(
  const rapidjson::Value& matrix_entry) const {
  assert(matrix_entry.HasMember("distance"));
  return matrix_entry["distance"].IsNull();
}

UserDuration ValhallaWrapper::get_duration_value(
  const rapidjson::Value& matrix_entry) const {
  assert(matrix_entry["time"].IsUint());
  return matrix_entry["time"].GetUint();
}

UserDistance ValhallaWrapper::get_distance_value(
  const rapidjson::Value& matrix_entry) const {
  assert(matrix_entry["distance"].IsDouble());
  return utils::round<UserDistance>(km_to_m *
                                    matrix_entry["distance"].GetDouble());
}

unsigned
ValhallaWrapper::get_legs_number(const rapidjson::Value& result) const {
  return result["trip"]["legs"].Size();
}

std::string ValhallaWrapper::get_geometry(rapidjson::Value& result) const {
  // Valhalla returns one polyline per route leg so we need to merge
  // them. Also taking the opportunity to adjust the encoding
  // precision as Valhalla uses 6 and we use 5 based on other routing
  // engine output. Note: getting directly a single polyline (e.g. by
  // not sending type=break for the route request) is not an option
  // since we have to force allowing u-turns in order to get a
  // geometry that is consistent with the time/distance values in
  // matrices.

  auto full_polyline =
    gepaf::PolylineEncoder<valhalla_polyline_precision>::decode(
      result["trip"]["legs"][0]["shape"].GetString());

  for (rapidjson::SizeType i = 1; i < result["trip"]["legs"].Size(); ++i) {
    auto decoded_pts =
      gepaf::PolylineEncoder<valhalla_polyline_precision>::decode(
        result["trip"]["legs"][i]["shape"].GetString());

    if (!full_polyline.empty()) {
      full_polyline.pop_back();
    }
    full_polyline.insert(full_polyline.end(),
                         std::make_move_iterator(decoded_pts.begin()),
                         std::make_move_iterator(decoded_pts.end()));
  }

  gepaf::PolylineEncoder<polyline_precision> encoder;
  for (const auto& p : full_polyline) {
    encoder.addPoint(p.latitude(), p.longitude());
  }

  return encoder.encode();
}

} // namespace vroom::routing
