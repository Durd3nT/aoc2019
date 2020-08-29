#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

void readImageData(const std::string & data_path,
                   std::vector<std::string> & data, const int dim)
{
    // store image layers in vector. sizeof(char) = 1, sizeof(int) = 4,
    // therefore storing image data as one string per image layer is 4 times
    // cheaper than storing each pixel as integer
    // ----------------

    std::ifstream infile(data_path);

    std::string line, image;
    size_t pos = 0;

    while (getline(infile, line, '\n')) {
        std::istringstream lineStream(line);

        while (pos < line.size()) { // end of line
            image = line.substr(pos, dim);
            data.push_back(image);
            pos += dim;
        }
    }
}


int getPixelNumber(const std::string & imageLayer, const int dim,
                   const int intLookedFor)
{
    // in imageLayer, get number of pixels equal to intLookedFor

    std::vector<int> imagePixels(dim);
    int count = 0;

    for (int j = 0; j < dim; j++) {
        if ((imageLayer[j] - '0') == intLookedFor) { count++; }
    }

    return count;
}


int getImLayer(const std::vector<std::string> & layers, const int dim,
               const int intLookedFor, const std::string & minMax)
{
    // get image layer (index in std::vector layers) that contains fewest/most
    // (specified by minMax) number of pixels equal to intLookedFor

    std::vector<int> imagePixels(dim);
    int layerIndex = 0;

    int minMaxCount;
    if (minMax == "min")
        minMaxCount = dim+1;
    else if (minMax == "max")
        minMaxCount = 0;
    else {
        std::cout << "ERROR: specify 'min' or 'max'\n";
        return 1;
    }

    for (size_t i = 0; i < layers.size(); i++) {
        int count = getPixelNumber(layers[i], dim, intLookedFor);

        if (minMax == "min" && count < minMaxCount) {
            minMaxCount = count;
            layerIndex = i;
        }
        else if (minMax == "max" && count > minMaxCount) {
            minMaxCount = count;
            layerIndex = i;
        }
    } // FOR: image layers

    return layerIndex;
}


void checkImage(const std::vector<std::string> & layers, const int dim) {
    int layerFewest0 = getImLayer(layers, dim, 0, "min");
    int count1 = getPixelNumber(layers[layerFewest0], dim, 1);
    int count2 = getPixelNumber(layers[layerFewest0], dim, 2);

    std::cout << "Image layer " << layerFewest0 << " contains fewest 0s (zeros)\n";
    std::cout << "(number of 1-pixels) x (number of 2-pixels) on that layer: ";
    std::cout << count1 * count2 << "\n";
}


void render(const std::vector<std::string> & layers, const int dim,
            std::vector<int> & finalImage)
{
    // get finalImage by combining all image layers with pixel ints
    // 2 = transparent, 1 = black, 0 = white.
    // looping through all layers, the first pixel at position i containing
    // a 1 or a 0 (i.e., different from 2) is copied to the i'th pixel in
    // finalImage.

    finalImage.resize(dim, 2);

    for (int j = 0; j < dim; j++) {
        int i = 0;
        while (finalImage[j] == 2 && i < layers.size()) {
            if (layers[i][j] != '2') { finalImage[j] = layers[i][j] - '0'; }
            i++;
        }
    }
}


void printImage(const std::vector<int> & finalImage, const int nx, const int ny)
{
    for (int i = 0; i < ny; i++) {
        for (int j = 0; j < nx; j++) {
            if (finalImage[j + i*nx] == 0 || finalImage[j + i*nx] == 2)
                std::cout << " ";
            else if (finalImage[j + i*nx] == 1)
                std::cout << "0";
        }
        std::cout << "\n";
    }
}



int main() {
    std::string data_path = "./input_files/in08.txt";

    int nx = 25;
    int ny = 6;
    int dim = nx*ny;

    std::vector<std::string> layers;

    readImageData(data_path, layers, dim);

    std::cout << "\n - - - PART 1 - - - \n";
    std::cout << "check for image corrption:\n";
    checkImage(layers, dim);

    std::cout << "\n - - - PART 2 - - - \n";
    std::cout << "final (rendered) image reads:\n\n";
    std::vector<int> finalImage;
    render(layers, dim, finalImage);

    printImage(finalImage, nx, ny);
}
