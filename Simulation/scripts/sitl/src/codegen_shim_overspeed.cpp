// codegen_shim_overspeed.cpp — Brücke Coder-ABI -> Test-ABI (safety_helpers.h).
// Nur aktiv bei SAFETY_IMPL=codegen. Passt zu deinem generierten safety_overspeed.h:
//   void safety_overspeed(const double gyro_corr[3], unsigned char estop,
//                         boolean_T ack, boolean_T *kill,
//                         unsigned char *fault_src, double dbg[3]);
//   void safety_overspeed_init(void);
// ACHTUNG: nach jeder Signatur-Aenderung MUSS gen_lib_codegen.m neu laufen,
// sonst passt der generierte Prototyp nicht mehr zu diesem Shim.
// Die Schwellen (omega_max/debounce_N/use_norm) sind via coder.Constant
// EINKOMPILIERT -> der OverspeedParams-Zeiger wird hier ignoriert. Deshalb ist
// use_norm fix (per-Achse); Szenario S9 (norm-Modus) wird im Test ausgeblendet.
#include "safety_helpers.h"

extern "C" {
#include "safety_overspeed.h"   // zieht rtwtypes.h (boolean_T)
}

void overspeed_reset(void) {
    safety_overspeed_init();
}

void overspeed_step(const double gyro_corr[3], uint8_t estop, bool ack,
                    const OverspeedParams* /*p (einkompiliert, ignoriert)*/,
                    bool* kill, uint8_t* fault_src, double dbg[3]) {
    boolean_T k;
    unsigned char fs;
    safety_overspeed(gyro_corr, static_cast<unsigned char>(estop),
                     static_cast<boolean_T>(ack), &k, &fs, dbg);
    *kill      = (k != 0);
    *fault_src = fs;
}
