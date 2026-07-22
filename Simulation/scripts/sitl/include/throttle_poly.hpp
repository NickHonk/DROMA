// Generiert von run_mcu_recert.m aus quadcop.p_from_omega — bitte nicht editieren.
// throttle = clamp(polyval(P_THROTTLE, rotor_cmd) * U_DS / Vc, 0, 100)
//   mit Vc = clamp(V_filt, V_THR_MIN, V_THR_MAX) aus safety_battery.
// Eingang ist rotor_cmd (= omega), NICHT omega^2 — das Polynom wurde auf
// omega umgestellt (Fit durch den Ursprung, Residuum 0.8% statt 5.6%).
#ifndef THROTTLE_POLY_HPP
#define THROTTLE_POLY_HPP
namespace mcuref {
static constexpr int    P_THROTTLE_N   = 3;
static constexpr double P_THROTTLE[3] = { 5.5397521107262334e-06, 0.015435978627782006, 0 };
static constexpr double U_DS       = 22.199999999999999;
static constexpr double V_THR_MIN  = 11;
static constexpr double V_THR_MAX  = 17.5;
static constexpr double THROTTLE_MIN = 0.0, THROTTLE_MAX = 100.0;
}  // namespace mcuref
#endif
