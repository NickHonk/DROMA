// test_safety.cpp — Safety-Golden-Test.
// Portiert die Invarianten aus verify_overspeed.m (S1-S9) und verify_battery.m
// (B1-B6) nach GoogleTest. Persistente Funktionen -> vor jedem Szenario reset().
// Laeuft gegen safety_helpers_ref.cpp bzw. (Shim) gegen den generierten Code.
#include "safety_helpers.h"
#include <gtest/gtest.h>
#include <array>
#include <cmath>
#include <random>
#include <vector>

// ------------------------------------------------------------ Overspeed
namespace {
OverspeedParams OSP{10.0, 4, false};

struct OsOut { std::vector<int> k, src; };
// Treibt die Sequenz (g: Nx3, estop: N, ack: N) -> kill/src pro Sample.
OsOut drive(const std::vector<std::array<double,3>>& g,
            const std::vector<uint8_t>& estop,
            const std::vector<uint8_t>& ack,
            const OverspeedParams& p) {
    OsOut o; std::size_t n = g.size();
    for (std::size_t i=0;i<n;++i){
        bool kill; uint8_t src; double dbg[3];
        overspeed_step(g[i].data(), estop[i], ack[i]!=0, &p, &kill,&src,dbg);
        o.k.push_back(kill?1:0); o.src.push_back(src);
    }
    return o;
}
std::vector<std::array<double,3>> rep(std::array<double,3> v,int n){
    return std::vector<std::array<double,3>>(n,v);
}
std::vector<std::array<double,3>> cat(std::vector<std::array<double,3>> a,
                                      const std::vector<std::array<double,3>>& b){
    a.insert(a.end(),b.begin(),b.end()); return a;
}
} // namespace

TEST(Overspeed, S1_NoOverspeed) {
    overspeed_reset();
    auto o = drive(rep({1,1,1},20), std::vector<uint8_t>(20,0), std::vector<uint8_t>(20,0), OSP);
    for (int k : o.k) EXPECT_EQ(k,0);
}
TEST(Overspeed, S2_Nminus1_NoLatch) {
    overspeed_reset();
    auto g = cat(rep({20,0,0},3), rep({0,0,0},5));
    auto o = drive(g, std::vector<uint8_t>(8,0), std::vector<uint8_t>(8,0), OSP);
    for (int k : o.k) EXPECT_EQ(k,0);
}
TEST(Overspeed, S3_TripAtNth_Holds) {
    overspeed_reset();
    auto g = cat(rep({20,0,0},4), rep({0,0,0},10));
    auto o = drive(g, std::vector<uint8_t>(14,0), std::vector<uint8_t>(14,0), OSP);
    EXPECT_EQ(o.k[2],0);                       // 3. Sample noch nicht
    EXPECT_EQ(o.k[3],1);                        // Trip exakt am 4. (N-ten)
    for (std::size_t i=3;i<o.k.size();++i) EXPECT_EQ(o.k[i],1);
    EXPECT_EQ(o.src[3],1);                      // fault_src = overspeed
}
TEST(Overspeed, S4_AckDuringOverspeed_NoRearm) {
    overspeed_reset();
    auto g = cat(rep({20,0,0},4), rep({20,0,0},6));
    std::vector<uint8_t> a(10,0); for(int i=4;i<10;++i) a[i]=1;
    auto o = drive(g, std::vector<uint8_t>(10,0), a, OSP);
    for (std::size_t i=3;i<o.k.size();++i) EXPECT_EQ(o.k[i],1);
}
TEST(Overspeed, S5_AckEdge_Rearms) {
    overspeed_reset();
    auto g = cat(rep({20,0,0},4), rep({0,0,0},4));
    std::vector<uint8_t> a(8,0); a[6]=1; a[7]=1;
    auto o = drive(g, std::vector<uint8_t>(8,0), a, OSP);
    EXPECT_EQ(o.k[5],1);   // vor Flanke latched
    EXPECT_EQ(o.k[6],0);   // Flanke -> re-armed
    EXPECT_EQ(o.k[7],0);   // bleibt armed bei gehaltenem ack
}
TEST(Overspeed, S6_HeldAck_NoAutoRearm) {
    overspeed_reset();
    auto g = cat(cat(rep({0,0,0},2), rep({20,0,0},4)), rep({0,0,0},4));
    auto o = drive(g, std::vector<uint8_t>(10,0), std::vector<uint8_t>(10,1), OSP);
    EXPECT_EQ(o.k[5],1);
    for (std::size_t i=5;i<o.k.size();++i) EXPECT_EQ(o.k[i],1);  // keine Flanke
}
TEST(Overspeed, S7_HardKill_Immediate) {
    overspeed_reset();
    std::vector<uint8_t> e{2,2,2,2,0,0}, a{0,0,0,0,0,1};
    auto o = drive(rep({0,0,0},6), e, a, OSP);
    EXPECT_EQ(o.k[0],1); EXPECT_EQ(o.src[0],2);
    EXPECT_EQ(o.k[3],1);
    EXPECT_EQ(o.k[4],1);   // estop->0 allein re-armt nicht
    EXPECT_EQ(o.k[5],0);   // estop=0 + ack-Flanke -> re-armed
}
TEST(Overspeed, S8_KillDominatesLand) {
    overspeed_reset();
    auto g = cat(rep({20,0,0},4), rep({0,0,0},3));
    auto o = drive(g, std::vector<uint8_t>(7,1), std::vector<uint8_t>(7,0), OSP);
    EXPECT_EQ(o.k[3],1); EXPECT_EQ(o.src[3],1);  // Overspeed dominiert soft-land
}
// S9 braucht laufzeit-schaltbares use_norm. Bei codegen mit coder.Constant ist der
// Modus einkompiliert (per-Achse) -> S9 wird dort ausgeblendet. Fuer volle Abdeckung
// die Safety-Leafs mit Laufzeit-Params generieren (siehe README/gen_lib_codegen.m).
#ifndef SAFETY_CODEGEN_CONST_PARAMS
TEST(Overspeed, S9_NormVsPerAxis) {
    OverspeedParams sN{10.0,4,true};
    overspeed_reset();
    auto o1 = drive(rep({7.5,7.5,0},4), std::vector<uint8_t>(4,0), std::vector<uint8_t>(4,0), sN); // ||.||=10.6
    EXPECT_EQ(o1.k[3],1);
    overspeed_reset();
    auto o2 = drive(rep({6.0,6.0,0},6), std::vector<uint8_t>(6,0), std::vector<uint8_t>(6,0), sN); // ||.||=8.49
    for (int k : o2.k) EXPECT_EQ(k,0);
}
#endif  // SAFETY_CODEGEN_CONST_PARAMS

// (S10 war der Arming-Idle-Interlock-Test — Feature in Session 9 verworfen,
//  Begruendung im Schlusskommentar von safety_overspeed.m.)

// ------------------------------------------------------------ Battery
namespace {
BatteryParams make_batt() {
    const double Ts=1.0/100.0, tau=0.7;
    BatteryParams p;
    p.batt_k = 3.3*18.182/4095.0;  p.batt_b = 0.0;
    p.batt_alpha = 1.0 - std::exp(-Ts/tau);
    p.V_warn=14.0; p.V_crit=13.4; p.V_floor=12.0; p.V_hyst=0.2;
    return p;
}
long v2c(double v, const BatteryParams& p){ return std::lround((v-p.batt_b)/p.batt_k); }
struct BOut{ uint8_t led; bool land; double V; };
BOut bstep(double v, const BatteryParams& p){
    BOut o; battery_step((double)v2c(v,p),&p,&o.led,&o.land,&o.V); return o;
}
} // namespace

TEST(Battery, B1_ColdStart_NoFalseTrip) {
    auto p=make_batt(); battery_reset();
    auto o=bstep(15.0,p);
    EXPECT_FALSE(o.land); EXPECT_EQ(o.led,0); EXPECT_NEAR(o.V,15.0,0.05);
}
TEST(Battery, B2_Ramp_Escalation) {
    auto p=make_batt(); battery_reset();
    const int N=4001; double w=NAN,c=NAN,f=NAN;
    for(int i=0;i<N;++i){
        double V=16.8+(11.5-16.8)*i/(N-1);
        auto o=bstep(V,p);
        if(std::isnan(w)&&o.led>=1) w=o.V;
        if(std::isnan(c)&&o.led>=2) c=o.V;
        if(std::isnan(f)&&o.land)   f=o.V;
    }
    EXPECT_NEAR(w,14.0,0.15); EXPECT_NEAR(c,13.4,0.15); EXPECT_NEAR(f,12.0,0.15);
    EXPECT_TRUE(w>c && c>f);
}
TEST(Battery, B3_Hysteresis_NoChatter) {
    auto p=make_batt(); battery_reset();
    bstep(13.95,p);
    std::mt19937 rng(0); std::uniform_real_distribution<double> U(-0.06,0.06);
    for(int i=0;i<500;++i){ auto o=bstep(13.90+U(rng),p); EXPECT_EQ(o.led,1); }
}
TEST(Battery, B4_FloorLatch_Sticky) {
    auto p=make_batt(); battery_reset();
    for(int i=0;i<50;++i)  bstep(12.5,p);
    bool land=false;
    for(int i=0;i<400;++i) land=bstep(11.5,p).land;
    EXPECT_TRUE(land);
    for(int i=0;i<300;++i) land=bstep(12.6,p).land;  // V-Erholung im Descent
    EXPECT_TRUE(land);                                // Latch haelt -> kein Grenzzyklus
}
TEST(Battery, B5_EMA_FiltersShortSag) {
    auto p=make_batt(); battery_reset();
    for(int i=0;i<200;++i) bstep(13.0,p);
    bool land=false;
    for(int i=0;i<5;++i) land=bstep(11.0,p).land;     // 50 ms Sag
    EXPECT_FALSE(land);
}
TEST(Battery, B6_CountRange) {
    auto p=make_batt();
    EXPECT_EQ(v2c(13.2,p),901);
    EXPECT_EQ(v2c(16.8,p),1147);
}
