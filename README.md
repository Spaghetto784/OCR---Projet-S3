# OCR Word Search Solver

## Group: **SegFaultless**

- **Matthieu Greiner**
- **Julien Teixeira**
- **Dylan Barros**
- **Pierre-Louis Chambon**

---

## Project Description

The goal of this project is to develop software capable of solving word search puzzles from an image. The project utilizes Optical Character Recognition (OCR) to identify letters in a grid and then employs a grid-solving algorithm in C to search for words.

The software takes an image representing a word search grid as input and returns the resolved grid. This application combines several technologies, including:
- **Image Preprocessing**: Converting images to grayscale and straightening them to improve image quality.
- **Neural Network**: Recognizing letters present in the grid from images of the letters.
- **Grid Solver**: A C algorithm that searches for a word in a grid and displays its position.

## Dependencies

Before compiling the project, ensure you have the necessary dependencies installed:

### SDL
- **Ubuntu/Debian**: 
  ```bash
  sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev 
  ```

### GTK 3.0
- **Ubuntu/Debian**:
  ```
  sudo apt-get install libgtk-4-dev
  ```
## Installation

To compile the project, run the following command in the terminal at the root of the repository:

```bash
make all
```

The project uses a Makefile that allows you to compile and execute different components of the project. Here are the main commands:

To run the solver:

```bash
make solver
```
This generates the solver executable, which allows you to search for words in a grid.

To run the neural network:

``` bash
make neural
```
This compiles and runs tests for the neural network.

To run the main OCR application with Grayscale treatment: 

```bash
make det
```

This will generate the image_Det executable, the file to use to test the image loading with detection treatments.

```bash
./bin/ocr_gui
```
This will run the application on GTK and you will be able to use everything above in the application.

## License

This project is licensed under the MIT License.

### MIT License

Copyright (c) 2024 SegFaultless

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

- The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
