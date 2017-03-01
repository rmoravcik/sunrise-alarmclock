include <rounded_cube.scad>
include <dimensions.scad>

width = case_width-2*wall_thickness-0.5;
length = case_length-2*wall_thickness-0.5;

module led_holder_base(){
    rounded_cube(width,length,2*wall_thickness,8);
};

module led_holder_cuts(){
    translate([wall_thickness,wall_thickness,wall_thickness]) rounded_cube(width-2*wall_thickness,length-2*wall_thickness,wall_thickness,6);

    translate([(width)/2,(length)/2,0]) cylinder(r=6,h=wall_thickness,$fn=32);

    translate([(width)/2-3.4/2,10,0]) cylinder(r=1/2,h=wall_thickness,$fn=32);
    translate([(width)/2+3.4/2,10,0]) cylinder(r=1/2,h=wall_thickness,$fn=32);
};

module led_holder(){
    difference() {
        led_holder_base();
        led_holder_cuts();
    }
};

led_holder();
