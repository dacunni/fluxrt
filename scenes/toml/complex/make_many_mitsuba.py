#!/usr/bin/env python3

#dim = 2
dim = 5
scale = 2.0
spacing = 2.5

print("""
[camera]
type = "pinhole"
hfov = 60

position = [ 40.0, 25.0, 40.0 ]
lookat = [ 5.0, 4.0, 5.0 ]
up = [ 0.0, 1.0, 0.0 ]

[sensor]
pixelwidth = 1200
pixelheight = 800

[envmap]
type = "gradient"
low = [ 0.0, 0.0, 0.0 ]
high = [ 1.0, 1.0, 1.0 ]
direction = [ 1.0, 0.2, 0.0 ]

[[pointlights]]
position = [ 45.0, 45.0, 31.0 ]
intensity = [ 3000.0, 3000.0, 3000.0 ]

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
        y = spacing * yi + 0.5
        for zi in range(0, dim):
            z = spacing * zi
            print(f"[[meshes]]")
            print(f'file = "casual-effects.com/mitsuba/mitsuba-sphere.obj"')
            print(f"[[meshes.transform]]")
            print(f"translate = [ {x}, {y}, {z} ]")
            print(f"[[meshes.transform]]")
            print(f"scale = [ {scale}, {scale}, {scale} ]")


