#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

void computeHistogram(const vector<unsigned char>& image, vector<int>& histogram);
void computeCumulativeHistogram(const vector<int>& histogram, vector<int>& cumulativeHistogram);
void histogramEqualization(const vector<unsigned char>& inputImage, vector<unsigned char>& outputImage);
void divideAndConquerHistogramEqualization(const vector<unsigned char>& inputImage, vector<unsigned char>& outputImage, int left, int right, int threshold);


// Define image parameters
// Change these parameters according to the image being tested
const int IMAGE_SIZE = 452 * 307;

// Define histogram parameters
const int HISTOGRAM_SIZE = 256;



int main() {
    // Read the input image
    const char* inputFileName = "test1.raw"; // Change the name of input file here

    ifstream inputFile(inputFileName, ios::binary);
    if (!inputFile) {
        cerr << "Error: Could not open input image file." << endl;
        return 1;
    }

    vector<unsigned char> inputImage(IMAGE_SIZE);
    inputFile.read(reinterpret_cast<char*>(inputImage.data()), IMAGE_SIZE);
    inputFile.close();

    // Set the threshold for divide-and-conquer (adjust as needed)
    int threshold = 1000;

    // Create an output image buffer
    vector<unsigned char> equalizedImage(IMAGE_SIZE);

    // Perform divide-and-conquer histogram equalization
    divideAndConquerHistogramEqualization(inputImage, equalizedImage, 0, IMAGE_SIZE, threshold);

    // Save the equalized image
    const char* outputFileName = "test1_equalized_image.raw"; // Change the name of output file here
    ofstream outputFile(outputFileName, ios::binary);
    if (!outputFile) {
        cerr << "Error: Could not create output image file." << endl;
        return 1;
    }

    outputFile.write(reinterpret_cast<const char*>(equalizedImage.data()), IMAGE_SIZE);
    outputFile.close();

    cout << "Histogram equalization using divide-and-conquer algorithm completed. Equalized image saved as " << outputFileName << endl;

    return 0;
}

// Function to compute the histogram of an image (PDF)
void computeHistogram(const vector<unsigned char>& image, vector<int>& histogram) {
    histogram.assign(HISTOGRAM_SIZE, 0);

    for (unsigned char pixel : image) {
        histogram[pixel]++;
    }
}

// Function to compute the cumulative histogram (CDF)
void computeCumulativeHistogram(const vector<int>& histogram, vector<int>& cumulativeHistogram) {
    cumulativeHistogram.resize(HISTOGRAM_SIZE);
    cumulativeHistogram[0] = histogram[0];

    for (int i = 1; i < HISTOGRAM_SIZE; ++i) {
        cumulativeHistogram[i] = cumulativeHistogram[i - 1] + histogram[i];
    }
}

// Function to perform histogram equalization using divide-and-conquer algorithm
void histogramEqualization(const vector<unsigned char>& inputImage, vector<unsigned char>& outputImage) {
    vector<int> histogram;
    computeHistogram(inputImage, histogram);

    vector<int> cumulativeHistogram;
    computeCumulativeHistogram(histogram, cumulativeHistogram);

    int imageSize = inputImage.size();
    outputImage.resize(imageSize);

    for (int i = 0; i < imageSize; ++i) {
        unsigned char pixel = inputImage[i];
        int newValue = (cumulativeHistogram[pixel] * 255) / imageSize;
        outputImage[i] = static_cast<unsigned char>(newValue);
    }
}

// Function to divide and conquer the image for efficient histogram equalization
void divideAndConquerHistogramEqualization(const vector<unsigned char>& inputImage, vector<unsigned char>& outputImage, int left, int right, int threshold) {
    if (right - left < threshold) {
        // Apply histogram equalization to the sub-image
        vector<unsigned char> subImage(inputImage.begin() + left, inputImage.begin() + right);
        vector<unsigned char> equalizedSubImage;
        histogramEqualization(subImage, equalizedSubImage);

        // Copy the equalized sub-image back to the output
        copy(equalizedSubImage.begin(), equalizedSubImage.end(), outputImage.begin() + left);
    }
    else {
        // Divide the image into two halves and process recursively
        int middle = (left + right) / 2;
        divideAndConquerHistogramEqualization(inputImage, outputImage, left, middle, threshold);
        divideAndConquerHistogramEqualization(inputImage, outputImage, middle, right, threshold);
    }
}