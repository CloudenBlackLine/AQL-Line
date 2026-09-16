AQL-1 BASE BODY V0 — VISUAL / APPROXIMATE CAD PACKAGE
=========================================================

PURPOSE
This package is a scale-conscious visual/prototype model based on the 90 mm ApisQueen EDF discussed in chat. It is NOT dimension-verified for the physical EDF yet.

FILES
- AQL1_CoreBody_V0_VISUAL.stl / .step
- AQL1_EDF_Carrier_V0_VISUAL.stl / .step
- AQL1_Lower_Module_Interface_V0_VISUAL.stl / .step
- AQL1_BaseBody_Assembly_V0_VISUAL.step
- AQL1_BaseBody_Assembly_V0_VISUAL.stl  (assembled preview; do not print as one piece)
- AQL1_Reference_Assembly_with_EDF_Envelope.step (includes simplified EDF reference envelope)
- AQL1_BaseBody_V0_VISUAL.scad (parametric inspection/edit file)
- dimensions.json

CURRENT APPROXIMATE DIMENSIONS (mm)
- Main core OD: 108.0
- Main core ID: 96.0
- Main core height: 170.0
- 4 support fins, 90 deg apart
- Fin thickness: 6.0
- Fin radial reach beyond core: 30.0
- Fin height: 140.0
- Fin mounting holes: 4.4 dia
- Core top/bottom collar OD: 118.0
- Core/carrier/module bolt pattern: 6 x 3.4 dia on 108.0 PCD
- EDF carrier nominal ID: 96.5
- Lower module connection: 4 x 4.5 dia on 110.0 PCD (45 deg offset)

REFERENCE EDF ASSUMPTIONS
- Nominal EDF size: 90 mm
- Estimated main housing OD: 94.0 mm
- Estimated body length: 100.0 mm
- Estimated bellmouth OD: 110.0 mm

ARCHITECTURE
- Fan intake is on TOP.
- Fan exhaust is on BOTTOM, feeding the removable thrust-vectoring module.
- 4 straight vertical support fins are integral to the core body, like external missile fins.
- Battery / ESC / controller / IMU mounts are intended to bolt onto those fins, not directly to the curved core.
- EDF carrier is removable so the fan can be reused in other projects.
- Lower module interface is removable / standardized for Mod 1, Mod 3, etc.

IMPORTANT
Do not use this approximate model for powered EDF operation. Measure the actual EDF housing, mounting tabs, battery, ESC, controller, IMU and desired fasteners before producing a functional version. In particular, the fan carrier and structural interfaces must be revised to the real hardware dimensions before a high-thrust test.
