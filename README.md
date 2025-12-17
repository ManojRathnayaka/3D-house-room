# 3D Bedroom Scene

A fully interactive 3D bedroom environment built with OpenGL and GLUT, featuring realistic lighting, textures, animations, FPS-style camera controls, and festive decorations.

## Features

### 🎮 Interactive Controls
- **FPS Camera Movement**: WASD for movement, QE for vertical movement, mouse for looking around
- **Animated Door**: Press F to open/close the bedroom door with smooth animation
- **Dynamic Curtains**: Press C to open/close window curtains with realistic bunching effect
- **Ceiling Fan**: Press R to toggle fan rotation on/off
- **Advanced Lighting System**: Multiple controllable light sources including natural sunlight through window
- **Mouse Capture**: ESC toggles between captured mouse (for FPS movement) and free cursor

### 🏠 Room Objects
- **Furniture**:
  - Bed with pillows and blanket
  - Wooden wardrobe with multiple drawers
  - Wooden cupboard with detailed handles and compartments
  - Dressing table with tri-fold decorative mirrors
  - Bedside drawer with knob
  - Modern circular table with cylindrical base
  - Cushioned chair with wooden frame
  
- **Decorative Items**:
  - Animated pendulum wall clock
  - Framed poster with wooden frame
  - Textured carpet overlay
  - Glass display box with stencil-based rendering
  - Fresh apple with realistic torus-based modeling
  - Decorative vase with colorful flowers
  - **Christmas tree** with animated blinking ornaments and glowing star

- **Architectural Elements**:
  - Textured walls, floor, and ceiling
  - Functional window with frame, bars, and animated curtains
  - Opening door with realistic hinges and handle
  - Ceiling-mounted light bulb
  - Rotating ceiling fan with four blades

### 💡 Advanced Lighting System
- **Light 0** (Ceiling Light): Main overhead illumination - Toggle with `1`
- **Light 1** (Sunlight): Dynamic sunlight through window that responds to curtain position
  - Intensity automatically adjusts based on curtain state (closed = dim, open = bright)
  - Creates realistic day/night atmosphere
- **Light 2** (Lamp Spotlight): Bedside lamp with focused directional beam - Toggle with `2`
- Emission effects on light bulb and lamp shade when active
- Realistic material properties with proper ambient, diffuse, and specular components

### 🎨 Visual Features
- **Textures**: Wood, carpet, floor, poster, and ceiling textures using SOIL2 library
- **Advanced Rendering**:
  - Stencil buffer operations for glass display box
  - Alpha blending for transparent materials
  - Proper normal calculations for realistic lighting
  - Smooth shading model
- **Animations**:
  - Smooth door rotation (0-90 degrees)
  - Curtain scaling effect (bunching when open)
  - Ceiling fan rotation
  - Pendulum clock swing
  - Discrete blinking Christmas lights with color cycling
- **Special Effects**:
  - Glowing Christmas star
  - Light emission from active bulbs
  - Dynamic sky brightness in window

## Controls

### Movement
- `W`: Move forward
- `S`: Move backward
- `A`: Move left
- `D`: Move right
- `Q`: Move up
- `E`: Move down
- `Mouse`: Look around (FPS style)
- `ESC`: Toggle mouse capture on/off

### Interactive Elements
- `F`: Open/close door with smooth animation
- `C`: Open/close curtains (affects sunlight intensity)
- `R`: Toggle ceiling fan rotation
- `1`: Toggle main ceiling light (Light 0)
- `2`: Toggle bedside lamp spotlight (Light 2)

## Technical Implementation

### Rendering Techniques
- **3D Modeling**: Custom geometric shapes (cubes, trapezoids, spheres, cylinders, torus)
- **Transformations**: Hierarchical modeling with push/pop matrix operations
- **Camera System**: First-person perspective with mouse-look and WASD movement
- **Lighting**: Multi-light setup with point lights, spotlights, and ambient illumination
- **Texturing**: UV-mapped textures on walls, floors, and furniture
- **Transparency**: Alpha blending for glass objects
- **Stencil Operations**: Advanced rendering for glass display box

### Animation Systems
- Door: Interpolated rotation with open/close states
- Curtains: Scaling transformation with automatic sunlight adjustment
- Fan: Continuous rotation when active
- Clock: Sinusoidal pendulum swing
- Christmas Lights: Time-based color cycling with unique offsets per bulb

## Dependencies

- **OpenGL**: Core graphics library
- **GLUT/FreeGLUT**: Window management and input handling
- **GLU**: OpenGL Utility library for quadrics and perspective
- **SOIL2**: Texture loading library

## Required Textures

Place these image files in the project directory:
- `wood.jpg` - Wood texture for door and furniture
- `carpet.jpg` - Carpet texture for floor covering
- `floor.jpg` - Main floor texture
- `ceiling.jpg` - Ceiling texture
- `image.jpg` - Poster artwork

## Compilation

### Linux/Unix:
```bash
g++ bedroom.cpp -o bedroom -lGL -lGLU -lglut -lSOIL2 -lm
```

### Windows (MinGW):
```bash
g++ bedroom.cpp -o bedroom.exe -lopengl32 -lglu32 -lfreeglut -lSOIL2
```

## Usage

1. Ensure all texture files are in the same directory as the executable
2. Run the program:
```bash
   ./bedroom
```
3. The mouse is captured by default for FPS-style camera control
4. Press ESC to release the mouse cursor when needed
5. Use keyboard controls to interact with room elements
6. Try opening the curtains (C) to see sunlight flood into the room!

## Project Highlights

- **Real-time Rendering**: Smooth 60 FPS animation loop
- **Interactive Environment**: Multiple controllable elements responding to user input
- **Realistic Physics**: Pendulum simulation using sine wave oscillation
- **Dynamic Lighting**: Light intensity responds to environmental changes (curtain state)
- **Festive Atmosphere**: Christmas tree with time-based animated decorations
- **Advanced Graphics**: Stencil buffer usage, alpha blending, emission effects

## Educational Value

This project demonstrates core computer graphics concepts including:
- 3D geometric modeling and transformations
- Camera systems and perspective projection
- Phong lighting model implementation
- Texture mapping and UV coordinates
- Animation and interpolation techniques
- User interaction and input handling
- Scene graph management
- Advanced rendering techniques (stencils, blending)

---

*Built as part of a Computer Graphics course project to demonstrate interactive 3D visualization techniques using OpenGL.*
