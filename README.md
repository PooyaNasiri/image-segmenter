# README: Image Segmentation using OpenCV

This program demonstrates an interactive image segmentation technique using OpenCV. It performs image processing operations like masking, removing small areas, adding small areas, and dilation to segment regions of an image based on user input. The segmentation is controlled by mouse events and trackbars, which allow the user to fine-tune the parameters for better segmentation.

## Dependencies

The code relies on the following libraries:

- OpenCV 4.x (Core, Imgproc, HighGUI modules)
- C++ Standard Library (iostream)

## Code Explanation

### Global Variables and Constants

```cpp
int CT = 100, T1 = 1;
const int addArae = 100000;
cv::Scalar color;
cv::Mat src;
int mouse_x, mouse_y;
```
These global variables control the segmentation process. CT is the color threshold, T1 is an object size threshold, and addArae defines the maximum area for adding regions.


### RemoveAreas Function
```cpp
cv::Mat RemoveAreas(cv::Mat mask)
```
This function removes small connected components from the binary mask image. It uses cv::connectedComponentsWithStats() to find connected components and their statistics. If a component's area is smaller than a threshold, it is removed from the mask.


### AddAreas Function
```cpp
cv::Mat AddAreas(cv::Mat mask)
```
This function adds small connected areas back to the mask. It inverts the mask, finds connected components, and sets pixels of small components to 1. The mask is then inverted back.


### Mask Function
```cpp
cv::Mat Mask(cv::Mat img, cv::Scalar mean, int T, int size)
```
The Mask function creates a binary mask based on pixel color similarity. It iterates over a specific region around the mouse position and sets pixels in the mask to 255 if their color difference from the specified mean is less than a threshold.


## Usage Instructions

1. Compile the code with OpenCV. Example:
   ```bash
   g++ -o segmenter segmenter.cpp `pkg-config --cflags --libs opencv4`
   ```
2. Run the compiled program:
   ```bash
   ./segmenter image.jpg
   ```

3. If no image is provided, the camera feed will be used.
4. Use the trackbars to adjust segmentation parameters.
5. Click on the image to select a color for segmentation.

## Conclusion
This program provides a basic interactive image segmentation tool using OpenCV, with customization through trackbars and mouse inputs. It can be extended for various image processing applications, including object detection and image analysis.

