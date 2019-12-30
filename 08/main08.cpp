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
    std::vector<int> imagePixels(dim);
    int count = 0;

    for (int j = 0; j < dim; j++) {
        // get j'th character of i'th image string, convert char to int
        imagePixels[j] = imageLayer[j] - '0';
        if (imagePixels[j] == intLookedFor) { count++; }
    }

    return count;
}


int getImLayer(const std::vector<std::string> & images, const int dim,
               const int intLookedFor, const std::string & minMax)
{
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

    for (size_t i = 0; i < images.size(); i++) {
        int count = getPixelNumber(images[i], dim, intLookedFor);

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


void checkImage(const std::vector<std::string> & images, const int dim) {
    int layerFewest0 = getImLayer(images, dim, 0, "min");
    int count1 = getPixelNumber(images[layerFewest0], dim, 1);
    int count2 = getPixelNumber(images[layerFewest0], dim, 2);

    std::cout << "Image layer " << layerFewest0 << " contains fewest 0s (zeros)\n";
    std::cout << "(number of 1-pixels) x (number of 2-pixels) on that layer: ";
    std::cout << count1 * count2 << "\n";
}



int main() {
    std::string data_path = "in08.txt";

    int nx = 25;
    int ny = 6;
    int dim = nx*ny;

    std::vector<std::string> images;

    readImageData(data_path, images, dim);

    checkImage(images, dim);
}
