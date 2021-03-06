#!/usr/bin/env python3

dim = 7
radius = 1.0
spacing = 2.2

print("""
[camera]
type = "pinhole"
hfov = 60

position = [ 30.0, 16.0, 30.0 ]
lookat = [ 5.0, 4.0, 5.0 ]
up = [ 0.0, 1.0, 0.0 ]

[sensor]
pixelwidth = 1200
pixelheight = 800

#[envmap]
#type = "gradient"
#low = [ 0.0, 0.0, 0.0 ]
#high = [ 1.0, 1.0, 1.0 ]
#direction = [ 1.0, 0.2, 0.0 ]

[[pointlights]]
position = [ 25.0, 25.0, 21.0 ]
intensity = [ 100.0, 100.0, 100.0 ]

# Floor
[[slabs]]
min = [ -100.0, -0.5, -100.0 ]
max = [  100.0, -0.04, 100.0 ]

    [slabs.material]
    type = "diffuse"
    diffuse = [ 0.5, 0.5, 0.5 ]
""")

for xi in range(0, dim):
    x = spacing * xi
    for yi in range(0, dim):
        y = spacing * yi + 2.5
        for zi in range(0, dim):
            z = spacing * zi
            print(f"[[spheres]]")
            print(f"radius = {radius}")
            print(f"position = [ {x}, {y}, {z} ]")


