// codegen_shim_battery.cpp — Brücke Coder-ABI -> Test-ABI (safety_helpers.h).
// Nur aktiv bei SAFETY_IMPL=codegen. Passt zu deinem generierten safety_battery.h:
//   void safety_battery(double batt_count, unsigned char *led,
//                       boolean_T *batt_land, double *V_filt);
//   void safety_battery_init(void);
// Params (batt_k/b/alpha, V_warn/crit/floor/hyst) via coder.Constant einkompiliert
// -> BatteryParams-Zeiger wird ignoriert. Die Konstanten in gen_lib_codegen.m
// muessen daher denen in test_safety.cpp (make_batt) entsprechen; die Battery-
// Tests sind invariantenbasiert (Toleranzen), daher unkritisch gegen Mini-Abweichung.
#include "safety_helpers.h"

extern "C" {
#include "safety_battery.h"
}

void battery_reset(void) {
    safety_battery_init();
}

void battery_step(double batt_count, const BatteryParams* /*p (einkompiliert)*/,
                  uint8_t* led, bool* batt_land, double* V_filt) {
    unsigned char l;
    boolean_T bl;
    safety_battery(batt_count, &l, &bl, V_filt);
    *led       = l;
    *batt_land = (bl != 0);
}
