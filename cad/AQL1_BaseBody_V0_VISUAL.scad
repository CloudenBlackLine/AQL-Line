// AQL-1 Base Body V0 VISUAL / APPROXIMATE ONLY
// Units: mm
// This is a visual/prototype model based on estimated 90 mm EDF dimensions.
// Do not power-test until real EDF + hardware dimensions are substituted.

$fn = 128;
core_od = 108.0;
core_id = 96.0;
core_h = 170.0;
collar_od = 118.0;
collar_h = 8.0;
fin_t = 6.0;
fin_ext = 30.0;
fin_h = 140.0;
fin_z = 15.0;

module ring(od,id,h) { difference() { cylinder(d=od,h=h); translate([0,0,-0.5]) cylinder(d=id,h=h+1); } }

module core_body() {
  difference() {
    union() {
      ring(core_od, core_id, core_h);
      ring(collar_od, core_id, collar_h);
      translate([0,0,core_h-collar_h]) ring(collar_od, core_id, collar_h);
      for(a=[0:90:270]) rotate([0,0,a]) translate([core_od/2-2,-fin_t/2,fin_z]) cube([fin_ext+2,fin_t,fin_h]);
    }
    // fin holes
    for(z=[38,72,106,140]) {
      translate([core_od/2+fin_ext*0.62,-10,z]) rotate([-90,0,0]) cylinder(d=4.4,h=20);
      translate([-core_od/2-fin_ext*0.62,-10,z]) rotate([-90,0,0]) cylinder(d=4.4,h=20);
      translate([-10,core_od/2+fin_ext*0.62,z]) rotate([0,90,0]) cylinder(d=4.4,h=20);
      translate([-10,-core_od/2-fin_ext*0.62,z]) rotate([0,90,0]) cylinder(d=4.4,h=20);
    }
  }
}

module edf_carrier() {
  difference() {
    union() {
      ring(118,96.5,14);
      for(a=[45:90:315]) rotate([0,0,a]) translate([58,-8,8]) cube([11,16,6]);
    }
    for(a=[0:60:300]) translate([54*cos(a),54*sin(a),-1]) cylinder(d=3.4,h=16);
    for(a=[45:90:315]) translate([64.5*cos(a),64.5*sin(a),7]) cylinder(d=4.5,h=8);
  }
}

module lower_interface() {
  difference() {
    union() { ring(118,96,14); translate([0,0,14]) ring(104,96,6); }
    for(a=[0:60:300]) translate([54*cos(a),54*sin(a),-1]) cylinder(d=3.4,h=17);
    for(a=[45:90:315]) translate([55*cos(a),55*sin(a),-1]) cylinder(d=4.5,h=17);
  }
}

// Display assembled
translate([0,0,0]) core_body();
translate([0,0,core_h]) edf_carrier();
translate([0,0,-20]) lower_interface();
