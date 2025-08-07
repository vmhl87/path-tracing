![rendered image](image.png)

uses the same geometry as demo2,3,4,5 and similar lighting setup to demo4.

however the lighting computations have been fixed, removing pseudo-subsurface scattering.

the setup uses four lights instead of one: the main laser is (near) identical to previous, but two glow lights are added for environmental effects: the ground is slightly illuminated, bringing out the shadow, and additionally the white sphere has a little bit of bloom around the light spot. lastly, a very dim global skylight adds a bit more texture to the backdrop.
