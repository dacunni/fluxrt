# Scene File Format (TOML)

Scenes are described in [TOML](https://toml.io/) files. Each file defines a sensor, camera, environment map, geometry, materials, and lights. Multiple files can be composed via `[[include]]`.

## Path Resolution

Several keys accept file paths. Relative paths are resolved against environment variables set before launching the renderer:

| Variable | Used for |
|---|---|
| `MESH_PATH` | Mesh files in `[[meshes]]` |
| `ENV_MAP_PATH` | Environment map files in `[envmap]` |
| `TEXTURE_PATH` | Textures in materials and `[[namedmaterials]]` |
| `SCENE_PATH` | Included scene files in `[[include]]` |

Absolute paths bypass these prefixes.

---

## `[sensor]`

Defines the output image resolution.

```toml
[sensor]
pixelwidth  = 1920
pixelheight = 1080
```

| Key | Type | Default | Description |
|---|---|---|---|
| `pixelwidth` | int | 100 | Output image width in pixels |
| `pixelheight` | int | 100 | Output image height in pixels |

---

## `[camera]`

### Pinhole (perspective)

```toml
[camera]
type     = "pinhole"
hfov     = 45.0           # horizontal field of view in degrees
position = [0.0, 1.0, 5.0]
lookat   = [0.0, 0.5, 0.0]  # alternative to direction
direction = [0.0, 0.0, -1.0]
up       = [0.0, 1.0, 0.0]

# Depth of field (optional)
focus_distance  = 5.0
focus_divergence = 0.02
```

| Key | Type | Default | Description |
|---|---|---|---|
| `hfov` | float | 45.0 | Horizontal field of view in degrees. Vertical FOV is derived from the sensor aspect ratio. |
| `position` | [x,y,z] | [0,0,0] | Camera position in world space |
| `direction` | [x,y,z] | [0,0,-1] | View direction (normalized automatically) |
| `lookat` | [x,y,z] | — | Point to aim at; overrides `direction` if present |
| `up` | [x,y,z] | [0,1,0] | Up vector |
| `focus_distance` | float | — | Distance to the focal plane; enables depth of field |
| `focus_divergence` | float | — | Lens aperture size; controls blur amount |

### Orthographic

```toml
[camera]
type     = "ortho"
hsize    = 4.0            # horizontal extent in world units
position = [0.0, 0.0, 5.0]
direction = [0.0, 0.0, -1.0]
up       = [0.0, 1.0, 0.0]
```

| Key | Type | Default | Description |
|---|---|---|---|
| `hsize` | float | 2.0 | Horizontal extent of the orthographic view in world units. Vertical is derived from the sensor aspect ratio. |

---

## `[envmap]`

Sets the environment light and background. Three types are supported.

### Gradient

A linear gradient between two radiance values along a direction vector.

```toml
[envmap]
type      = "gradient"
low       = [0.0, 0.0, 0.0]
high      = [1.0, 1.0, 1.0]
direction = [0.0, 1.0, 0.0]  # gradient axis
```

| Key | Type | Default | Description |
|---|---|---|---|
| `low` | [R,G,B] | [0,0,0] | Radiance at the negative end of the axis |
| `high` | [R,G,B] | [1,1,1] | Radiance at the positive end of the axis |
| `direction` | [x,y,z] | [0,1,0] | Axis direction |

### Lat/lon (equirectangular HDR)

```toml
[envmap]
type        = "latlon"
file        = "sky.hdr"          # relative to ENV_MAP_PATH
scalefactor = 1.0
```

| Key | Type | Default | Description |
|---|---|---|---|
| `file` | string | required | Equirectangular image file (HDR or LDR) |
| `scalefactor` | float | 1.0 | Multiplier applied to all sampled radiance values |

### Cubemap

Six individual face images, one per direction.

```toml
[envmap]
type        = "cubemap"
xneg        = "faces/negx.jpg"
xpos        = "faces/posx.jpg"
yneg        = "faces/negy.jpg"
ypos        = "faces/posy.jpg"
zneg        = "faces/negz.jpg"
zpos        = "faces/posz.jpg"
scalefactor = 1.0
```

All six face keys are required. Paths are relative to `ENV_MAP_PATH`.

---

## `[[spheres]]`

Each `[[spheres]]` entry creates one sphere.

```toml
[[spheres]]
radius   = 1.0
position = [0.0, 0.0, 0.0]

    [spheres.material]
    type    = "diffuse"
    diffuse = [0.8, 0.2, 0.2]

    [[spheres.transform]]
    translate = [1.0, 0.0, 0.0]
```

| Key | Type | Default | Description |
|---|---|---|---|
| `radius` | float | 1.0 | Sphere radius |
| `position` | [x,y,z] | [0,0,0] | Center position in world space |
| `material` | table | — | Inline material (see [Materials](#materials)) |
| `transform` | array of tables | — | Transforms applied to the object (see [Transforms](#transforms)) |

---

## `[[slabs]]`

An axis-aligned box defined by minimum and maximum corners.

```toml
[[slabs]]
min = [-5.0, -0.5, -5.0]
max = [ 5.0,  0.0,  5.0]

    [slabs.material]
    type    = "diffuse"
    diffuse = [0.5, 0.5, 0.5]
```

| Key | Type | Default | Description |
|---|---|---|---|
| `min` | [x,y,z] | [0,0,0] | Minimum corner |
| `max` | [x,y,z] | [0,0,0] | Maximum corner |
| `material` | table | — | Inline material |
| `transform` | array of tables | — | Transforms |

---

## `[[meshes]]`

Loads a triangle mesh from a file. Supported formats: `.obj`, `.stl`, `.ply`.

```toml
[[meshes]]
file        = "mymodel.obj"    # relative to MESH_PATH
name        = "hero"           # optional label
scaletocube = 2.0              # optional: scale to fit a centered cube of this side length
accelerator = "octree"         # "octree" (default) or "none"

    [meshes.material]
    type    = "diffuse"
    diffuse = [1.0, 0.8, 0.6]

    [[meshes.transform]]
    translate = [0.0, 1.0, 0.0]
```

| Key | Type | Default | Description |
|---|---|---|---|
| `file` | string | required | Mesh file path, relative to `MESH_PATH` |
| `name` | string | `""` | Optional label (informational only) |
| `scaletocube` | float | — | Uniformly scale and center the mesh to fit within a cube of this side length |
| `accelerator` | string | `"octree"` | Intersection accelerator: `"octree"` or `"none"` |
| `material` | table | — | Inline material override; overrides per-face materials from the file |
| `transform` | array of tables | — | Transforms |

The mesh data cache deduplicates repeated loads of the same file path.

---

## Materials

Materials appear as subtables of objects (`[spheres.material]`, `[meshes.material]`, etc.) or as named materials.

### Common fields

| Key | Type | Description |
|---|---|---|
| `type` | string | required; one of the types below |
| `alpha` | float | Opacity in [0, 1]. Values below 1 produce stochastic transparency. |
| `normalmap` | string | Path to a tangent-space normal map texture (relative to `TEXTURE_PATH`) |

### `diffuse`

Lambertian diffuse surface.

```toml
type    = "diffuse"
diffuse = [0.8, 0.2, 0.2]        # RGB reflectance, or:
diffuse = "albedo.png"            # texture path (relative to TEXTURE_PATH)
alpha   = 0.5                     # optional
```

### `diffusespecular`

Combined diffuse and Phong specular. Probability of a specular bounce is proportional to the mean specular value.

```toml
type             = "diffusespecular"
diffuse          = [0.6, 0.6, 0.6]   # or texture path
specular         = [0.4, 0.4, 0.4]   # or texture path
specular_exponent = 100.0             # Phong exponent; 0 = mirror-like
normalmap        = "normals.png"      # optional
```

| Key | Type | Default | Description |
|---|---|---|---|
| `diffuse` | [R,G,B] or string | [0,0,0] | Diffuse reflectance or texture |
| `specular` | [R,G,B] or string | [0,0,0] | Specular reflectance or texture |
| `specular_exponent` | float | 0.0 | Phong exponent. 0 = perfectly specular mirror. Higher = sharper highlight. |

### `mirror`

Perfect specular mirror. No additional fields.

```toml
type = "mirror"
```

### `refractive`

Dielectric (glass-like) refractive material with Fresnel and optional Beer's law absorption.

```toml
type = "refractive"
ior  = 1.5             # index of refraction (default: 1.333)
beer = [0.5, 0.5, 0.0] # per-channel absorption coefficients (default: no absorption)
```

| Key | Type | Default | Description |
|---|---|---|---|
| `ior` | float | 1.333 | Index of refraction. Common values: water 1.333, glass 1.5, diamond 2.42. |
| `beer` | [R,G,B] | [0,0,0] | Beer's law absorption coefficients. Higher values absorb more light per unit distance. Zero means no absorption. |

### `emissive`

Self-illuminating surface. Emits radiance directly; does not scatter incoming light.

```toml
type     = "emissive"
emissive = [5.0, 0.0, 0.0]   # RGB radiance
```

---

## Named Materials

Define a material once and reference it by name in multiple objects. Named materials must be declared before the objects that use them, or in an included file loaded earlier.

**Declaration:**
```toml
[[namedmaterials]]
name    = "white-plastic"
type    = "diffusespecular"
diffuse = [1.0, 1.0, 1.0]
specular = [0.04, 0.04, 0.04]
```

**Use:**
```toml
[[meshes]]
file = "model.obj"
    [meshes.material]
    include = "white-plastic"
```

Named materials support all the same fields as inline materials.

---

## Transforms

Objects (`[[spheres]]`, `[[slabs]]`, `[[meshes]]`) accept an array of transforms. Each element specifies exactly one of `translate`, `rotate_axis`/`rotate_angle`, or `scale`. Transforms are applied in the order they appear.

```toml
[[meshes.transform]]
translate = [0.0, 1.0, 0.0]

[[meshes.transform]]
rotate_axis  = [0.0, 1.0, 0.0]
rotate_angle = 45.0              # degrees

[[meshes.transform]]
scale = [2.0, 2.0, 2.0]
```

| Key | Type | Description |
|---|---|---|
| `translate` | [x,y,z] | Translation vector |
| `rotate_axis` | [x,y,z] | Axis of rotation (must be paired with `rotate_angle`) |
| `rotate_angle` | float | Rotation angle in degrees |
| `scale` | [x,y,z] | Per-axis scale factors |

Each `[[object.transform]]` block must contain exactly one of these operations.

---

## `[[pointlights]]`

Punctual point lights (no area, no falloff beyond inverse-square law).

```toml
[[pointlights]]
position  = [0.0, 5.0, 0.0]
intensity = [3.0, 3.0, 3.0]   # RGB radiant intensity
```

| Key | Type | Default | Description |
|---|---|---|---|
| `position` | [x,y,z] | [0,0,0] | Light position |
| `intensity` | [R,G,B] | [1,1,1] | Radiant intensity |

---

## `[[disklights]]`

Disk-shaped area lights. Cast soft shadows; are sampled directly by the renderer.

```toml
[[disklights]]
position  = [0.0, 3.0, 0.0]
direction = [0.0, -1.0, 0.0]  # normal pointing away from the emitting face
radius    = 0.5
intensity = [10.0, 10.0, 10.0]
```

| Key | Type | Default | Description |
|---|---|---|---|
| `position` | [x,y,z] | [0,0,0] | Center of the disk |
| `direction` | [x,y,z] | [0,-1,0] | Disk normal (normalized automatically) |
| `radius` | float | 1.0 | Disk radius |
| `intensity` | [R,G,B] | [1,1,1] | Emitted radiance |

---

## `[[include]]`

Includes another TOML scene file. All objects, lights, named materials, and the sensor/camera/envmap defined in the included file are merged into the current scene. Includes are processed in order before other tables in the file.

```toml
[[include]]
source = "standard/scenes/base-scene.toml"   # relative to SCENE_PATH

[[include]]
source = "standard/materials/smooth-plastic.toml"
```

Includes can nest arbitrarily. Use this to share common camera setups, base scenes, and material libraries across multiple scene files.

---

## Complete Example

```toml
[sensor]
pixelwidth  = 800
pixelheight = 600

[camera]
type      = "pinhole"
hfov      = 45.0
position  = [0.0, 2.0, 6.0]
lookat    = [0.0, 0.5, 0.0]
up        = [0.0, 1.0, 0.0]

[envmap]
type      = "gradient"
low       = [0.0, 0.0, 0.0]
high      = [0.8, 0.9, 1.0]
direction = [0.0, 1.0, 0.0]

[[namedmaterials]]
name    = "floor-mat"
type    = "diffusespecular"
diffuse = [0.7, 0.7, 0.7]
specular = [0.1, 0.1, 0.1]

[[meshes]]
file = "mymodel.obj"
    [meshes.material]
    type = "refractive"
    ior  = 1.5
    [[meshes.transform]]
    translate = [0.0, 1.0, 0.0]

[[spheres]]
radius   = 0.5
position = [2.0, 0.5, 0.0]
    [spheres.material]
    type   = "mirror"

[[slabs]]
min = [-10.0, -0.5, -10.0]
max = [ 10.0,  0.0,  10.0]
    [slabs.material]
    include = "floor-mat"

[[disklights]]
position  = [0.0, 4.0, 1.0]
direction = [0.0, -1.0, 0.0]
radius    = 0.4
intensity = [8.0, 8.0, 8.0]
```
