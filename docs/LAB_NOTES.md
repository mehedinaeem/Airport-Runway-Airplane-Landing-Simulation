# Lab implementation notes

## Scene and coordinates

The origin is the top-left corner; x increases right and y increases downward. The 1200 × 700 canvas reserves the top 64 pixels for a header and the bottom 41 for controls. The airport sits near the horizon at y = 402. A shallow trapezoidal runway provides perspective while retaining a readable side view of the aircraft. Wheels travel along y = 538, inside the runway plane.

The runway is aligned with travel from left to right and carries the requested illustrative `27` marking. This is a stylized lab scene, not a georeferenced airport diagram.

The daytime sky is a line-by-line gradient. Night adds deterministic stars, a crescent moon, glowing terminal windows and larger runway-light halos. Primitive-drawn buildings include a glazed terminal with jet bridges, a control tower and a hangar. Clouds wrap horizontally using modulo arithmetic; vehicle motion and radar rotation use the scene clock.

## Main functions

| Function | Responsibility |
| --- | --- |
| `transform()` | Scale, rotate and translate an aircraft-local point |
| `Simulation::start()` / `restart()` | Enter approach or reset to the title screen |
| `Simulation::update()` / `step()` | Integrate time and move through landing stages independently of rendering |
| `drawBackground()` | Compose sky, ground, airport buildings, vehicles and runway |
| `drawSky()` / `drawCloud()` | Gradient, day/night celestial body, skyline and wrapping clouds |
| `drawGround()` / `drawRunway()` | Airport apron, taxiway, runway surface and markings |
| `drawRunwayLights()` | Perspective spacing, edge lights and center lights |
| `drawTerminal()` / `drawHangar()` | Airport architecture made from rectangles and polygons |
| `drawControlTower()` / `drawRadar()` | Control cab and continuously rotating antenna |
| `drawAirplane(x,y,scale,angle)` | Reusable airplane built from transformed polygons and lines |
| `drawLandingGear()` | Extend struts and wheels using the same aircraft transformation |
| `drawSmoke()` | Short-lived expanding touchdown puffs |
| `drawInfoPanel()` | Simulated altitude, speed, remaining runway distance, stage timeline and success message |
| `drawStartScreen()` / `drawChrome()` | Title, instructions, mode and speed display |
| `handleInput()` | Read queued WinBGIm key events and apply commands |
| `main()` | Create the window, update state, draw hidden page, present and pace frames |

Drawing methods read the simulation through a const reference. They do not move the aircraft or change state. Only the small drawing helper functions are free functions; animation data is contained in `Simulation` rather than spread across globals.

## Translation, rotation and scaling

Each aircraft point is defined once in a local model centered near the fuselage. Given a local point `(x,y)`, uniform scale `s`, angle `theta` and translation `(tx,ty)`, the program computes:

```text
sx = s*x
sy = s*y
rx = sx*cos(theta) - sy*sin(theta)
ry = sx*sin(theta) + sy*cos(theta)
worldX = rx + tx
worldY = ry + ty
```

Angles are converted from degrees to radians before calling trigonometric functions. Positive angles look clockwise because screen y points down. Every wing, window, door, engine, light and landing-gear endpoint goes through this mapping. Circular wheel radii are scaled too; a circle's appearance is unchanged by rotation.

Translation carries the plane from x = 100 to touchdown at x = 525, and then to its parking position at x = 985. Scaling grows it from 0.72 to 1.0 on approach. Rotation eases from a slight nose-down attitude to zero degrees for wheel contact. Radar rotation separately demonstrates trigonometric projection of an antenna onto the screen.

## Landing state machine

```text
WAITING --Enter--> APPROACH --> DESCENT --> FINAL_APPROACH
                                                |
                                                v
LANDED <----------- BRAKING <-------------- TOUCHDOWN
```

The airborne part lasts 12 simulated seconds. It uses normalized time `t` and the smoothstep curve `3*t*t - 2*t*t*t`. Its derivative is zero at both ends, so descent levels smoothly before contact. Gear extends between 30% and 58% of airborne time, well before final approach.

At touchdown the model is level, full scale, and its origin is at `538 - 29 = 509`. Each wheel center is 25 units below that origin, with radius 4. Both wheel bottoms are therefore exactly at y = 538. `wheelHeight()` applies the real aircraft transform to both wheel centers before computing telemetry, including the scale and rotation.

Touchdown coasts for about 0.7 simulated seconds and shows smoke. Braking uses the stopping-distance equation:

```text
a = v*v / (2*remainingDistance)
x = x + v*dt - 0.5*a*dt*dt
v = max(0, v - a*dt)
```

This produces smooth deceleration and a defined stop. When the remaining travel fits inside one integration step, position is clamped exactly to the parking point and velocity is set to zero. It then displays `LANDING COMPLETED SUCCESSFULLY`.

The telemetry converts pixels to illustrative feet/meters/knots. It is not intended to model real aircraft performance. Altitude measures the lowest wheel bottom above the runway contact line; distance is the remaining horizontal travel to the touchdown point and stays zero after touchdown.

## Timing, buffering and interaction

A monotonic `steady_clock` supplies elapsed time. Speed keys multiply the simulation time step; they do not change frame delay. The state advances in substeps of at most 1/120 simulated second, including at 4× speed. A 100 ms real-time stall clamp prevents window dragging or a breakpoint from jumping the scene forward. Frames are paced to approximately 20 ms (50 FPS).

`setactivepage()` selects the hidden page, the complete scene is redrawn, and `setvisualpage()` presents the finished image. The two pages alternate. The sky and ground cover the canvas, so a visible clear step is unnecessary. No console keyboard polling or extra input library is needed: WinBGIm provides `kbhit()` and `getch()`.

Pause freezes the common scene clock as well as the aircraft, so clouds, smoke, lights, vehicles and radar all pause together. Day/night commands still work while paused. After landing, the airport animation keeps running. Restart resets the aircraft, smoke, clock and stage but keeps the selected lighting mode and simulation rate.

## Suggested lab explanation

1. Start in day mode and identify each primitive used in the airport scene.
2. Point to `Point` and `transform()` and explain the order of transformations.
3. Start a landing; pause during gear extension and explain the state machine.
4. Switch to night to show lighting and the same scene composition.
5. Resume at 4× and explain time-based movement versus frame-based movement.
6. Observe wheel contact, smoke and decreasing speed during braking.
7. Show the success message, restart, and demonstrate a slower landing.
