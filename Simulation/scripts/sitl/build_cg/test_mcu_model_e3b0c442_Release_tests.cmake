add_test([=[McuGolden.RotorCmdMatchesGolden]=]  C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg/Release/test_mcu_model.exe [==[--gtest_filter=McuGolden.RotorCmdMatchesGolden]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[McuGolden.RotorCmdMatchesGolden]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\Rakete\Documents\Drohnenversuchsstand\DROMA\Simulation\scripts\sitl\test\test_mcu_model.cpp:42]==]
    WORKING_DIRECTORY [==[C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[McuGolden.DeterministicAcrossFreshInstances]=]  C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg/Release/test_mcu_model.exe [==[--gtest_filter=McuGolden.DeterministicAcrossFreshInstances]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[McuGolden.DeterministicAcrossFreshInstances]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\Rakete\Documents\Drohnenversuchsstand\DROMA\Simulation\scripts\sitl\test\test_mcu_model.cpp:103]==]
    WORKING_DIRECTORY [==[C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[McuFailsafe.Estop2KillsThrottleAndRotor]=]  C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg/Release/test_mcu_model.exe [==[--gtest_filter=McuFailsafe.Estop2KillsThrottleAndRotor]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[McuFailsafe.Estop2KillsThrottleAndRotor]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\Rakete\Documents\Drohnenversuchsstand\DROMA\Simulation\scripts\sitl\test\test_mcu_model.cpp:129]==]
    WORKING_DIRECTORY [==[C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[McuOverspeed.KillHoldsAndReArmsOnBusAckEdge]=]  C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg/Release/test_mcu_model.exe [==[--gtest_filter=McuOverspeed.KillHoldsAndReArmsOnBusAckEdge]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[McuOverspeed.KillHoldsAndReArmsOnBusAckEdge]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\Rakete\Documents\Drohnenversuchsstand\DROMA\Simulation\scripts\sitl\test\test_mcu_model.cpp:152]==]
    WORKING_DIRECTORY [==[C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[McuButton.EdgeKillsHeldBlocksRearmBusAckClears]=]  C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg/Release/test_mcu_model.exe [==[--gtest_filter=McuButton.EdgeKillsHeldBlocksRearmBusAckClears]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[McuButton.EdgeKillsHeldBlocksRearmBusAckClears]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\Rakete\Documents\Drohnenversuchsstand\DROMA\Simulation\scripts\sitl\test\test_mcu_model.cpp:211]==]
    WORKING_DIRECTORY [==[C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[McuTilt.SustainedTiltKills]=]  C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg/Release/test_mcu_model.exe [==[--gtest_filter=McuTilt.SustainedTiltKills]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[McuTilt.SustainedTiltKills]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\Rakete\Documents\Drohnenversuchsstand\DROMA\Simulation\scripts\sitl\test\test_mcu_model.cpp:259]==]
    WORKING_DIRECTORY [==[C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[McuBattery.RampEscalatesLed]=]  C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg/Release/test_mcu_model.exe [==[--gtest_filter=McuBattery.RampEscalatesLed]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[McuBattery.RampEscalatesLed]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\Rakete\Documents\Drohnenversuchsstand\DROMA\Simulation\scripts\sitl\test\test_mcu_model.cpp:291]==]
    WORKING_DIRECTORY [==[C:/Users/Rakete/Documents/Drohnenversuchsstand/DROMA/Simulation/scripts/sitl/build_cg]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
set(test_mcu_model_TESTS [==[McuGolden.RotorCmdMatchesGolden]==] [==[McuGolden.DeterministicAcrossFreshInstances]==] [==[McuFailsafe.Estop2KillsThrottleAndRotor]==] [==[McuOverspeed.KillHoldsAndReArmsOnBusAckEdge]==] [==[McuButton.EdgeKillsHeldBlocksRearmBusAckClears]==] [==[McuTilt.SustainedTiltKills]==] [==[McuBattery.RampEscalatesLed]==])
