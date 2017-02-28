include <rounded_cube.scad>
include <dimensions.scad>

width = case_width-2*wall_thickness-0.5;
length = case_length-2*wall_thickness-0.5;

module bottom_cover_base(){
    rounded_cube(width,length,wall_thickness,8);
    translate([(width-pcb_width)/2+27,(length-pcb_lenght)/2+41.5+0.8,14/2+wall_thickness]) cube([8,6,14],center=true);
    translate([(width-pcb_width)/2+27,(length-pcb_lenght)/2-1.5,(14+2*1.8)/2+wall_thickness]) cube([8,6,14+2*1.8],center=true);
};

module bottom_cover_cuts(){
    translate([6-0.25,6-0.25,0]) cylinder(r1=6/2,r2=3/2,h=wall_thickness,$fn=32);
    translate([6-0.25,length-6+0.25,0]) cylinder(r1=6/2,r2=3/2,h=wall_thickness,$fn=32);
    translate([width-6+0.25,6-0.25,0]) cylinder(r1=6/2,r2=3/2,h=wall_thickness,$fn=32);
    translate([width-6+0.25,length-6+0.25,0]) cylinder(r1=6/2,r2=3/2,h=wall_thickness,$fn=32);
    translate([(width-pcb_width)/2+27,(length-pcb_lenght)/2+41.5,wall_thickness+5]) cylinder(r=1.25,h=10,$fn=32);

    translate([(width-pcb_width)/2+27,(length-pcb_lenght)/2+1.5/2,14+wall_thickness+1.8/2]) cube([8-0.5,1.5,1.8],center=true);
};

module bottom_cover(){
    difference() {
        bottom_cover_base();
        bottom_cover_cuts();
    }
};

bottom_cover();
