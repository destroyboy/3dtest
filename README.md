# 3dtest
triangle rasterizer

Based on [this article.](https://software.intel.com/en-us/articles/rasterization-on-larrabee)
I first started the rasterizer in javascript [here](http://biotagon.com/3dtest/)

## build

Code is in C. There is a script to build it on linux using gcc, otherwise it should be easy to build on another platform.

##run

There is a script which runs the test. It renders a teddy bear from teddy.obj into a file called teddy1024x1024.raw, then uses imagemagick to display the teddy.

##output

A file called teddy1024x1024.raw which is 1024x1024x8bit grayscale. If you change the dimensions of the render buffer then the filename changes i.e. teddy512x512.raw

The test outputs some timings to stdout. It fills the buffer 100 times with pixels and outputs this time. Which on my little haswell netbook is 0.01 seconds. It's important to do absolute timings. And this test gives an idea just how fast it's possible to draw pixels. In this case the maximum is 104857600 pixels / second.

The teddy takes about 2 seconds to render 100 times. Which is 200 times slower than the just filling the screen with a single color.

##options

There are a couple of defines in test_model.c so you can optionally draw a cube or turn on the SIMD function that I wrote that is slower than the non-simd function!

##bugs

No Z buffer! Due to limited time, I didn't implement a z-buffer even though it was asked for. To be honest I didn't see it was so important for such a simple renderer. Maybe that's an automatic fail, but the article talked a lot about vectorization and I spent a long time investigating that. If I had a z-buffer maybe I could have done perspective, but I didn't do perspective anyway. So... I just sort the triangles in z-order and draw back to front. So I never have to read from any buffers only write. This should increase the speed tremendously.

No binning! This would improve the speed a lot normally but as there is no z-buffer and the triangles are drawn one at a time, then I don't think so.

If you turn on the cube test you can see there is a gap between the triangles. If the larabee article is correct then this is fixable. I think normally you sort the edges and order them in a certain way to make sure there are no gaps/overdraws but again this didn't seem to be mega important for this exercise. The test is there to show the problem.

I took a lot of time trying to understand the vectorization. There is a huge setup cost on intel for using simd. You can't even multiply a vector by a scaler you must load the scaler multiple times into a vector. I think really you need to use assembly language to get the most benefit from simd, like in the article he gets a whole bunch of flags into one register. That was good although I kind of suspect that he overplayed the usefulness of the vector instructions. Also you can't make a vector of length 3 which would have been useful for what I used it for which was to do three edge calculations at once.

##further work.

When you get down to drawing say a 16x16 square then it would be good to stop recursing. I factored out a function, but the function just recurses. It needs to be implemented without recursion and by taking separate code paths for different cases.

Pipeline, well exactly, there is really no pipeline. Drawing one triangle at a time is stupid.
