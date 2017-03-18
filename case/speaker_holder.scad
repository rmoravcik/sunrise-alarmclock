include <rounded_cube.scad>
include <dimensions.scad>

width = case_width-2*wall_thickness-0.5;
length = case_length-2*wall_thickness-0.5;

module speaker_holder_base(){
    rounded_cube(width,length,2*wall_thickness,8);
};

module speaker_holder_cuts(){
    difference() {
        translate([wall_thickness,wall_thickness,wall_thickness]) rounded_cube(width-2*wall_thickness,length-2*wall_thickness,wall_thickness,6);
    translate([(width)/2,(length)/2,wall_thickness]) cylinder(r=(41+2*wall_thickness)/2,h=wall_thickness,$fn=32);
    }

    translate([(width)/2,(length)/2,wall_thickness+2]) cylinder(r=(41+2*wall_thickness)/2,h=wall_thickness,$fn=32);

    translate([(width)/2,(length)/2,0]) cylinder(r=38/2,h=wall_thickness,$fn=32);
    translate([(width)/2,(length)/2,wall_thickness]) cylinder(r=41/2,h=wall_thickness,$fn=32);

    translate([(width)/2,(length)/2,0]) cylinder(r=38/2,h=wall_thickness,$fn=32);
    translate([12,12,0]) cylinder(r=11/2,h=wall_thickness,$fn=32);
};

module speaker_holder(){
    difference() {
        speaker_holder_base();
        speaker_holder_cuts();
    }
};

speaker_holder();