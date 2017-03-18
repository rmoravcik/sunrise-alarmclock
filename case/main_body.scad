include <rounded_cube.scad>
include <dimensions.scad>

display_height = top_cover_height;
speaker_hole_height = display_height;

module main_body_base(){
    rounded_cube(case_width,case_length,main_body_height,10);
};

module main_body_cuts(){
    translate([wall_thickness,wall_thickness,0]) rounded_cube(case_width-2*wall_thickness,case_length-2*wall_thickness,main_body_height-wall_thickness,8);
    translate([wall_thickness/2, wall_thickness/2,main_body_height-wall_thickness]) rounded_cube(case_width-wall_thickness,case_length-wall_thickness,wall_thickness,9);

    // sound holes
    translate([case_width/2-20,case_length-wall_thickness,speaker_hole_height]) rotate([-90,0,0]) cylinder(r=5/2,h=10,$fn=32);
    translate([case_width/2-10,case_length-wall_thickness,speaker_hole_height]) rotate([-90,0,0]) cylinder(r=5/2,h=10,$fn=32);
    translate([case_width/2,case_length-wall_thickness,speaker_hole_height]) rotate([-90,0,0]) cylinder(r=5/2,h=10,$fn=32);
    translate([case_width/2+10,case_length-wall_thickness,speaker_hole_height]) rotate([-90,0,0]) cylinder(r=5/2,h=10,$fn=32);
    translate([case_width/2+20,case_length-wall_thickness,speaker_hole_height]) rotate([-90,0,0]) cylinder(r=5/2,h=10,$fn=32);

    // display hole
    translate([case_width/2,wall_thickness/2,display_height]) cube([25,wall_thickness,9.4],center=true);

    // touch sensor hole
    translate([case_width/2,2,20]) cube([25,1,25],center=true);

    // touch sensor hole
    translate([case_width/2,case_length-wall_thickness/2,2*wall_thickness]) cube([4,wall_thickness,2.8],center=true);
};

module mounting_base(){
    translate([-2,-2,wall_thickness+0.3]) rounded_cube(16,16,15,5);
    translate([case_width-16+2,-2,wall_thickness+0.3]) rounded_cube(16,16,15,5);
    translate([-2,case_length-16+2,wall_thickness+0.3]) rounded_cube(16,16,15,5);
    translate([case_width-16+2,case_length-16+2,wall_thickness+0.3]) rounded_cube(16,16,15,5);
    };

module mounting_cuts(){
    color([1,0,0]) translate([6+wall_thickness,6+wall_thickness,wall_thickness+0.3]) cylinder(r=1.25,h=10,$fn=32);
    color([1,0,0]) translate([6+wall_thickness,case_width-wall_thickness-6,wall_thickness+0.3]) cylinder(r=1.25,h=10,$fn=32);
    color([1,0,0]) translate([case_width-wall_thickness-6,6+wall_thickness,wall_thickness+0.3]) cylinder(r=1.25,h=10,$fn=32);
    color([1,0,0]) translate([case_width-wall_thickness-6,case_width-wall_thickness-6,wall_thickness+0.3]) cylinder(r=1.25,h=10,$fn=32);
};

module main_body(){
    difference() {
        main_body_base();
        main_body_cuts();
    }

    difference() {
        intersection(){
            translate([wall_thickness,wall_thickness,wall_thickness]) rounded_cube(case_width-2*wall_thickness,case_length-2*wall_thickness,15,8);
            mounting_base();
        }
        mounting_cuts();
    }

    translate([(case_width-38)/2,wall_thickness,display_height+(9.4/2)]) cube([38,3,2]);
    translate([(case_width-38)/2,wall_thickness,display_height+(9.4/2)-14]) cube([38,3,2]);
};

main_body();