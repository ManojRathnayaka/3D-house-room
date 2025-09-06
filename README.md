# 3D Bedroom Scene

A fully interactive 3D bedroom environment built with OpenGL and GLUT, featuring realistic lighting, textures, animations, and FPS-style camera controls.

## Features

### 🎮 Interactive Controls
- **FPS Camera Movement**: WASD for movement, QE for vertical movement, mouse for looking around
- **Animated Door**: Press F to open/close the bedroom door with smooth animation
- **Dynamic Lighting**: Control three different light sources with individual ambient/diffuse/specular settings
- **Mouse Capture**: ESC toggles between captured mouse (for FPS movement) and free cursor

### 🏠 Room Objects
- Furnished bedroom with bed, pillows, and blanket
- Wooden wardrobe and cupboard with detailed handles
- Dressing table with tri-fold mirror
- Bedside table with animated table lamp
- Animated pendulum wall clock
- Framed posters on walls
- Window with realistic frame and bars
- Textured carpet and wooden floor
- Rotating ceiling fan
- Wooden stool

### 💡 Advanced Lighting System
- **Light 1 & 2**: Ceiling-mounted point lights with full ambient/diffuse/specular control
- **Spot Light**: Directional lamp light with focused beam
- Individual control over each lighting component
- Realistic light positioning and shadows

### 🎨 Visual Features
- High-quality textures for wood, carpet, floor, and poster images
- Realistic material properties with proper shininess values
- Smooth animations for door, clock pendulum, and ceiling fan
- Detailed object modeling with proper normals for realistic lighting

## Controls

### Movement
- `W/A/S/D`: Move forward/left/backward/right
- `Q/E`: Move up/down
- `Mouse`: Look around (FPS style)
- `ESC`: Toggle mouse capture
- `R`: Reset camera to default position

### Door
- `F`: Open/close door

### Lighting Controls
**Light Source 1** (right ceiling light):
- `1`: Toggle light on/off
- `4`: Toggle ambient component
- `5`: Toggle diffuse component
- `6`: Toggle specular component

**Light Source 2** (left ceiling light):
- `2`: Toggle light on/off
- `7`: Toggle ambient component
- `8`: Toggle diffuse component
- `9`: Toggle specular component

**Lamp Light** (spot light):
- `3`: Toggle lamp on/off
- `0`: Toggle ambient component
- `-`: Toggle diffuse component
- `=`: Toggle specular component

## Dependencies

- OpenGL
- GLUT/FreeGLUT
- SOIL2 (for texture loading)

## Required Textures

Place these image files in the same directory as the executable:
- `wood.jpg` - Wood texture for door and furniture
- `carpet.jpg` - Carpet texture for floor covering
- `floor.jpg` - Floor texture
- `image.jpg` - First poster image
- `image2.jpg` - Second poster image

## Compilation

```bash
g++ bedroom.cpp -lGL -lGLU -lglut -lSOIL2 -lm -o bedroom
```

## Usage

Run the executable to enter the 3D bedroom environment. The mouse is captured by default for FPS-style movement. Press ESC to release the mouse cursor if needed.

The scene includes realistic physics simulation for the pendulum clock and smooth interpolated animations for the door and ceiling fan rotation.
