This a a short game written by C++, OpenGL, and GLSL.
********************************************************************
OVERVIEW

1. All the objects(ground, bear, trees, cars, honey cans) in the game are created in Blender where the bear object could be counted as a "Complicated" 3-D object with about 500+ vertices. The texture used in this game are created by "Texture Paint" in Blender.
2. Implemented the Catmull-Rom Spline, so that the movements of bear object and camera are nice and smooth. And, the camera is set to be perspective.
3. Implemented Billboard and Local Illumination, such as diffuse, specular, and ambient light, by using GLSL in fragment and vertex shaders.
********************************************************************
GAME INSTRUCTION

The game allows players to use arrow keys to control the charactor which is a bear runing around the forest to collect honey cans. While collecting, player need to avoid being hit by cars driving through the forest. 
Players will win the game, when the bar above bear's head is full, which means all honey cans are collected.
If get hit by the car, press"R" to restart the game.
********************************************************************
SOME OTHER DETAILS

NOTE: "Modeling.hpp", "Modeling.cpp", "Myshader.fragmentshader", and "Myshader.vertexshader" are just used for test. They are not actually part of game.

1. The Movement of Bear and Camera
The movement of bear is approched by implementing Catmull-Rom spline. Each time getting move input, the bear will jump a certain distance. Function calculate_control and getSpline in "Creature.cpp" will be called to generate 4 control points and put all the points calculated by the algorithm to an a vector. Then, the scene will be update and draw. The movement of camera is just to follow the bear.

2. The Random Generation of Cars
Cars that run through the forest are random generated. By creating a timer, the random_vehicle function are called every 0.4 second to generate random cars with different velocity in each line.

3. Collision Detection
Since the game is not really a 3D game, the collision detection is simply approched by bounding square around the object. For each object in the game, creating a bounding square around it by using four vertices. Those vertices will move along the object to detect whether there is a collision happening.

4. Local Illumination 
The diffuse, specular, and ambient lights are implemented by using "StandardShading.fragmentshader" and "StandardShading.vertexshader" which are basically from the OpenGL tutorial. The light source in the game is at (-4,6,-7). Everything in the scene gets local illumination except the billboard.

5. Billboard
The billboard is implemented by using "Billboard.fragmentshader", "Billboard.vertexshader", and some other OpenGL code. It in the game is like a bar to show how many honey cans are collected by the player while they are playing. The number of the collected honey cans will be set to zero when game get restarted.