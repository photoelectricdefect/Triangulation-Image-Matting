###Blue Screen Matting by Smith & Blinn

For those interested, paper is in the repo along with the necessary files to build the VS Solution.

Instructions: 4 images are needed, two with the object in the image, each with a different background, and one of each background. It is important that the camera, object and the background are completely still in between shots. Press ESC to exit the program.

Requirements:
    1.OpenCV 3.4.9, find an installation guide for MS VS if you have not used it before
    2.Eigen, a matrix algebra c++ library
    

####Solution

![Matting Equation](TriangulationMatting\Images\mattingeq.jpg)

Solve the linear least squares problem and get the solution.  

```c++
X = A.colPivHouseholderQr().solve(b);
alert(s);
```

####Results

![Alpha](TriangulationMatting\Images\alpha.jpg)


 
