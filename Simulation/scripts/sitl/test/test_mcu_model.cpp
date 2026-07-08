// test_mcu_model.cpp — Modell-SITL-Golden: der generierte MCU-Code (mcu.cpp)
// wird tickweise gegen die in Simulink aufgezeichneten I/O (golden_mcu_io.csv,
// via log_mcu_golden.m) gedifft.  Reset = frische MCU-Instanz (aller Zustand
// wird im Konstruktor genullt; initialize()/terminate() sind leer).
//
// Erwartung: rotor_cmd bit-nah an Golden (GOLDEN_TOL=1e-9 Default, eng lassen).
#include <array>
#include <gtest/gtest.h>
#include "mcu_io.hpp"

#ifndef GOLDEN_MCU_CSV
#define GOLDEN_MCU_CSV "data/golden_mcu_io.csv"
#endif
#ifndef GOLDEN_TOL
#define GOLDEN_TOL 1e-9
#endif

using namespace sitl;

// Golden einmal laden; fehlt die Datei -> SKIP mit klarer Anweisung (nicht FAIL,
// damit ctest vor dem ersten log_mcu_golden-Lauf nicht hart bricht).
static const NamedCsv* golden() {
    static bool tried = false; static NamedCsv g; static bool ok = false;
    if (!tried) { tried = true;
        try { g = read_named_csv(GOLDEN_MCU_CSV); ok = !g.rows.empty(); }
        catch (const std::exception&) { ok = false; } }
    return ok ? &g : nullptr;
}

TEST(McuGolden, RotorCmdMatchesGolden) {
    const NamedCsv* g = golden();
    if (!g) GTEST_SKIP() << "Golden fehlt: " << GOLDEN_MCU_CSV
                         << " — erst log_mcu_golden.m in MATLAB laufen lassen.";
    MCU obj; obj.initialize();
    double worst = 0.0; std::size_t worst_row = 0;
    for (std::size_t r = 0; r < g->rows.size(); ++r) {
        MCU::ExtU_mcu_T u{}; wire_inputs(u, *g, r);
        obj.setExternalInputs(&u); obj.step();
        const auto& y = obj.getExternalOutputs();
        double d = diff_rotor(y, *g, r);
        if (d > worst) { worst = d; worst_row = r; }
        ASSERT_LE(d, (double)GOLDEN_TOL)
            << "rotor_cmd-Divergenz in Zeile " << r
            << " (t=" << g->get(r,"t") << " s), |dq|=" << d;
        // led ist uint8 (Batterie-FSM-state) -> exakter Ganzzahlvergleich.
        ASSERT_EQ(diff_led(y, *g, r), 0.0)
            << "led-Divergenz in Zeile " << r
            << ": got " << (int)y.led << " expected " << g->get(r,"led.1");
    }
    RecordProperty("worst_abs_diff", worst);
    RecordProperty("worst_row", (int)worst_row);
    SUCCEED() << "max|dq|=" << worst << " @ Zeile " << worst_row;
}

// Zwei frische Laeufe muessen bit-identisch sein (Determinismus + Reset).
TEST(McuGolden, DeterministicAcrossFreshInstances) {
    const NamedCsv* g = golden();
    if (!g) GTEST_SKIP() << "Golden fehlt: " << GOLDEN_MCU_CSV;
    auto run_once = [&](std::vector<std::array<double,5>>& out){
        MCU obj; obj.initialize();
        for (std::size_t r = 0; r < g->rows.size(); ++r) {
            MCU::ExtU_mcu_T u{}; wire_inputs(u, *g, r);
            obj.setExternalInputs(&u); obj.step();
            const auto& y = obj.getExternalOutputs();
            out.push_back({y.rotor_cmd[0],y.rotor_cmd[1],y.rotor_cmd[2],y.rotor_cmd[3],
                           (double)y.led});
        }
    };
    std::vector<std::array<double,5>> a, b; run_once(a); run_once(b);
    ASSERT_EQ(a.size(), b.size());
    for (std::size_t r = 0; r < a.size(); ++r)
        for (int i = 0; i < 5; ++i)
            EXPECT_DOUBLE_EQ(a[r][i], b[r][i]) << "divergenz Zeile " << r << " ch " << i;
}