// test_mcu_model.cpp — Modell-SITL-Golden: der generierte MCU-Code (mcu.cpp)
// wird tickweise gegen die in Simulink aufgezeichneten I/O (golden_mcu_io.csv,
// via log_mcu_golden.m) gedifft.  Reset = frische MCU-Instanz (aller Zustand
// wird im Konstruktor genullt; initialize()/terminate() sind leer).
//
// Erwartung: rotor_cmd bit-nah an Golden (GOLDEN_TOL=1e-9 Default, eng lassen).
#include <array>
#include <cmath>
#include <gtest/gtest.h>
#include "mcu_io.hpp"
#include "throttle_poly.hpp"   // P_THROTTLE (aus params.m via run_mcu_recert.m)

#ifndef GOLDEN_MCU_CSV
#define GOLDEN_MCU_CSV "data/golden_mcu_io.csv"
#endif
#ifndef GOLDEN_TOL
#define GOLDEN_TOL 1e-9
#endif

using namespace sitl;

// throttle-Invariante: clamp(polyval(P, x), 0, 100), Horner wie MATLAB polyval.
// NICHT bit-exakt gegen den generierten Code moeglich (Polyval-Eingang im Modell
// ist das vorzeichenbehaftete omega_sq VOR abs; rotor_cmd=sqrt(abs(omega_sq)) ->
// sqrt(x)^2 weicht ~1 ULP von x ab). Beobachtet |d|<=7.1e-15, Schranke 1e-9.
static double throttle_ref(double x) {
    double y = mcuref::P_THROTTLE[0];
    for (int k = 1; k < mcuref::P_THROTTLE_N; ++k) y = y * x + mcuref::P_THROTTLE[k];
    if (y > mcuref::THROTTLE_MAX) y = mcuref::THROTTLE_MAX;
    if (y < mcuref::THROTTLE_MIN) y = mcuref::THROTTLE_MIN;
    return y;
}

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
    double worst = 0.0, worst_inv = 0.0; std::size_t worst_row = 0;
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
        // throttle: (a) generierter Code vs Simulink-Golden (<=GOLDEN_TOL).
        ASSERT_LE(diff_throttle(y, *g, r), (double)GOLDEN_TOL)
            << "throttle-Golden-Divergenz in Zeile " << r;
        // (b) Struktur-Invariante: throttle == clamp(polyval(P, rotor_cmd^2)).
        for (int i = 0; i < 4; ++i) {
            double di = std::abs(y.throttle[i] - throttle_ref(y.rotor_cmd[i] * y.rotor_cmd[i]));
            if (di > worst_inv) { worst_inv = di; }
            ASSERT_LE(di, 1e-9)
                << "throttle-Invariante verletzt in Zeile " << r << " Kanal " << i;
        }
    }
    RecordProperty("worst_abs_diff", worst);
    RecordProperty("worst_row", (int)worst_row);
    RecordProperty("worst_throttle_invariant", worst_inv);
    SUCCEED() << "max|dq|=" << worst << " @ Zeile " << worst_row
              << "; max throttle-Invariante=" << worst_inv;
}

// Zwei frische Laeufe muessen bit-identisch sein (Determinismus + Reset).
TEST(McuGolden, DeterministicAcrossFreshInstances) {
    const NamedCsv* g = golden();
    if (!g) GTEST_SKIP() << "Golden fehlt: " << GOLDEN_MCU_CSV;
    auto run_once = [&](std::vector<std::array<double,9>>& out){
        MCU obj; obj.initialize();
        for (std::size_t r = 0; r < g->rows.size(); ++r) {
            MCU::ExtU_mcu_T u{}; wire_inputs(u, *g, r);
            obj.setExternalInputs(&u); obj.step();
            const auto& y = obj.getExternalOutputs();
            out.push_back({y.rotor_cmd[0],y.rotor_cmd[1],y.rotor_cmd[2],y.rotor_cmd[3],
                           (double)y.led,
                           y.throttle[0],y.throttle[1],y.throttle[2],y.throttle[3]});
        }
    };
    std::vector<std::array<double,9>> a, b; run_once(a); run_once(b);
    ASSERT_EQ(a.size(), b.size());
    for (std::size_t r = 0; r < a.size(); ++r)
        for (int i = 0; i < 9; ++i)
            EXPECT_DOUBLE_EQ(a[r][i], b[r][i]) << "divergenz Zeile " << r << " ch " << i;
}

// Failsafe: estop==2 -> latched -> BEIDE Aktuator-Ausgaenge auf 0.
// Regression fuer den §3b-Bug: der throttle-Outport war NICHT vom latched-Gate
// erfasst -> der HAL (esc_write_all(throttle)) haette die Motoren bei Link-Verlust
// NICHT gestoppt. Braucht kein Golden (konstruiert die Eingaenge selbst).
TEST(McuFailsafe, Estop2KillsThrottleAndRotor) {
    MCU obj; obj.initialize();
    MCU::ExtU_mcu_T u{};
    u.Bus_IMU_k.imu_acc[2] = 9.81;                 // Schwerkraft -> Estimator laeuft
    u.Bus_Cmd_l.F_des = 9.4666;                     // ~Hover (F_des>0 -> ohne Kill waere throttle>0)
    u.Bus_Cmd_l.q_des[0] = 1.0;
    u.Bus_Cmd_l.q_ref[0] = 1.0;
    u.Bus_Cmd_l.q_ext[0] = 1.0;
    u.Bus_Cmd_l.estop = 2;                          // Hard-Kill (setzt latched, mcu.cpp Z.226)
    u.batt_count = 1000.0;
    for (int k = 0; k < 20; ++k) { obj.setExternalInputs(&u); obj.step(); }
    const auto& y = obj.getExternalOutputs();
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(0.0, y.throttle[i])  << "throttle["  << i << "] nicht gekillt bei estop=2";
        EXPECT_EQ(0.0, y.rotor_cmd[i]) << "rotor_cmd[" << i << "] nicht gekillt bei estop=2";
    }
}

// Overspeed-Latch im generierten Code: ||gyro||>omega_max(8.5) fuer > debounce_N(4)
// Ticks -> Kill (rotor/throttle=0). Latch HAELT ohne ack-Flanke; eine Taster-Flanke
// re-armt — UNABHAENGIG vom befohlenen Schub (der Arming-Idle-Interlock wurde in
// Session 9 verworfen, siehe safety_overspeed.m). Deckt die generierte ack-OR
// (Bus_Cmd.ack || btn_ack) + safety_overspeed ab.
TEST(McuOverspeed, KillHoldsAndReArmsOnAckEdge) {
    MCU obj; obj.initialize();
    MCU::ExtU_mcu_T u{};
    u.Bus_IMU_k.imu_acc[2] = 9.81;                  // Schwerkraft -> Estimator level
    u.Bus_Cmd_l.q_des[0] = 1.0;
    u.Bus_Cmd_l.q_ref[0] = 1.0;
    u.Bus_Cmd_l.q_ext[0] = 1.0;
    u.Bus_Cmd_l.estop = 0;                          // KEIN Hard-Kill (isolier Overspeed)
    u.batt_count = 944.0;                           // ~15.74 V -> Batterie NORMAL
    u.btn_ack = false;
    const double F_HOVER = 9.4666;                  // ~m*g

    // Phase 1: Overspeed bei Hover-Schub -> Kill-Latch.
    u.Bus_Cmd_l.F_des = F_HOVER;
    u.Bus_IMU_k.imu_gyro[0] = 9.0;                  // ||.||=9 > 8.5
    for (int k = 0; k < 10; ++k) { obj.setExternalInputs(&u); obj.step(); }
    {
        const auto& y = obj.getExternalOutputs();
        for (int i = 0; i < 4; ++i) {
            EXPECT_EQ(0.0, y.rotor_cmd[i]) << "rotor_cmd[" << i << "] nicht gekillt bei Overspeed";
            EXPECT_EQ(0.0, y.throttle[i])  << "throttle["  << i << "] nicht gekillt bei Overspeed";
        }
    }

    // Phase 2a: gyro 0, aber KEIN Taster -> Latch muss halten.
    u.Bus_IMU_k.imu_gyro[0] = 0.0;
    for (int k = 0; k < 10; ++k) { obj.setExternalInputs(&u); obj.step(); }
    {
        const auto& y = obj.getExternalOutputs();
        double s = 0.0; for (int i = 0; i < 4; ++i) s += y.throttle[i];
        EXPECT_EQ(0.0, s) << "Latch darf ohne ack-Flanke NICHT selbst freigeben";
    }

    // Phase 2b: Taster-Flanke bei HOVER-Schub -> Re-Arm geht durch. Regression fuer
    // die Session-9-Entscheidung: KEIN F_des-Interlock mehr (frueher blockiert).
    u.btn_ack = true;
    for (int k = 0; k < 10; ++k) { obj.setExternalInputs(&u); obj.step(); }
    {
        const auto& y = obj.getExternalOutputs();
        double s = 0.0; for (int i = 0; i < 4; ++i) s += y.throttle[i];
        EXPECT_GT(s, 0.0) << "ack-Flanke muss re-armen, auch bei Hover-Schub "
                             "(Idle-Interlock ist entfallen)";
    }

    // Phase 2c: gehaltenes ack erzeugt keine neue Flanke -> ein frischer Overspeed
    // muss trotzdem latchen (ack-Pegel darf den Trip nicht sofort wieder loeschen).
    u.Bus_IMU_k.imu_gyro[0] = 9.0;                  // erneut Overspeed, ack bleibt HIGH
    for (int k = 0; k < 10; ++k) { obj.setExternalInputs(&u); obj.step(); }
    {
        const auto& y = obj.getExternalOutputs();
        for (int i = 0; i < 4; ++i)
            EXPECT_EQ(0.0, y.throttle[i])
                << "gehaltenes ack darf einen frischen Trip nicht loeschen";
    }
}

// Batterie-FSM im generierten Code: fallende Spannung -> led 0(NORMAL)->1(WARN,<=14.0)
// ->2(CRIT,<=13.4). Treibt batt_count = round(V/k) mit HW-kalibriertem k und laesst
// den EMA-Tiefpass je Stufe auskonvergieren (grosszuegige Tickzahl).
TEST(McuBattery, RampEscalatesLed) {
    MCU obj; obj.initialize();
    MCU::ExtU_mcu_T u{};
    u.Bus_IMU_k.imu_acc[2] = 9.81;
    u.Bus_Cmd_l.q_des[0] = 1.0;
    u.Bus_Cmd_l.q_ref[0] = 1.0;
    u.Bus_Cmd_l.q_ext[0] = 1.0;
    u.Bus_Cmd_l.estop = 0;
    const double k = 0.016673728813559323;          // HW-kal. (== mcu_data.cpp)
    auto settle = [&](double volts, int n) -> int {
        u.batt_count = std::round(volts / k);
        for (int i = 0; i < n; ++i) { obj.setExternalInputs(&u); obj.step(); }
        return (int)obj.getExternalOutputs().led;
    };
    EXPECT_EQ(0, settle(15.5, 30000)) << "led sollte NORMAL sein bei 15.5 V";
    EXPECT_EQ(1, settle(13.7, 30000)) << "led sollte WARN sein bei 13.7 V (<=14.0, >13.4)";
    EXPECT_EQ(2, settle(13.0, 30000)) << "led sollte CRIT sein bei 13.0 V (<=13.4)";
}