include <rounded_cube.scad>
include <dimensions.scad>

module top_cover_base(){
    rounded_cube(case_width,case_length,top_cover_height,10);
    translate([wall_thickness/2, wall_thickness/2,top_cover_height]) rounded_cube(case_width-wall_thickness,case_length-wall_thickness,wall_thickness,9);
};

module top_cover_cuts(){
    translate([wall_thickness,wall_thickness,wall_thickness]) rounded_cube(case_width-2*wall_thickness,case_length-2*wall_thickness,top_cover_height,8);
};

module top_cover(){
    difference() {
        top_cover_base();
        top_cover_cuts();
    }
};

top_cover();
