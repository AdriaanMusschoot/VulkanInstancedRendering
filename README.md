# Instanced Rendering 

Welcome to this Vulkan application showcasing instanced rendering for 2D as well as 3D. Made following the vulkan tutorial, but converted to the C++ implementation.

![Alt text](Readme/Overview.png)
# Journey
Let me take you on a journey and explain some of the issues I ran into to get to the final result.
## Step 1: Rendering that first triangle!
![Alt text](Readme/RenderingTriangles.png)

This first step actually took the longest, although it was probably the simplest one. Vulkan just requires a lot of setup. What I found usefull in this part is that it required me to follow a strict set of rules, there were no visual queues yet if something was applied correctly or incorrectly. Exceptions played an important role here, although they were mostly for null checks they allowed me to find easily where mistakes were made.  
## Step 2: 3D rendering and texturing!
![Alt text](Readme/NotInstanceRendering.png)

### Data issues
This was supposed to be a simple step. But some silly issues arose that I couldn't have foreseen. The first issue was that I was missing one third of my vertices. It took some time before I realised this made a lot of sense. When I started transforming the pipeline from 2D to 3D, I forgot to change the size value of a vertex. Thus litterly transferring only two thirds of my data to the GPU. Another issue and a way more pressing issue was that it seemed I had an inverted depth buffer. I started looking into the depth pass and everything else related to depth to see if I messed some values up. Little did I know, I was looking in the wrong place. The issue was actually already present since I was rendering 2D, at the time I had no clue, I just thought my values were wrong back then and just changed those. 
This also caused the camera movement to be incorrect.
### Renderdoc to the rescue
After going through my code multiple times, I was suggested to use renderdoc. I then found relatively quickly that the issue was in the viewport, the minDepth and maxDepth were inverted. So I was looking for the correct issue, just not in the right place. For about 2 weeks I was stuck with this inverted depth buffer, once that issue was gone I was able to finish up basic 3D rendering and using single textures for objects. However as you could see in the image above performance really took a hit when I was trying to render the same mesh a couple times. My camera issues were also fixed after inverting the inverted depth buffer.
## Step3: Instanced Rendering!
![Alt text](Readme/Thumbnail.png)
