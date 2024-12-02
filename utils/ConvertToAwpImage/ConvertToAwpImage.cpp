// ConvertToAwpImage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "LFFileUtils.h"
#include "LFImage.h"
//#include ""
#include <iostream>

std::unique_ptr<TLFImage> load_image(const TLFString& path)
{
    std::unique_ptr<TLFImage> img = std::make_unique<TLFImage>();
    if (!img->LoadFromFile(path.c_str())) {
        std::cerr << "Can't load image " << path << std::endl;
        return nullptr;
    }
    return img;
}

bool save_image(const TLFString& path, awpImage* pImage) {
    auto err = awpSaveImage(path.c_str(), pImage);
    if (err != AWP_OK) {
        std::cerr << "awpSaveImage return error " << err << std::endl;
        return false;
    }
    return true;
}



int main(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++)
        std::cout << "Argument " << i << " = " << argv[i] << std::endl;
    if (argc != 3) {
        std::cout << "ERROR!!! argc != 3, use:" << std::endl << "ConvertToAwpImage.exe path_to_image_jpg conv_path_image_awp" << std::endl;
        return -1;
    }

    std::cout << "start!\n";
    const std::string& image_path = argv[1];//"d:\\work\\test1.jpg";
    awpImage* img_rgb = NULL;
    if (awpLoadImage(image_path.c_str(), &img_rgb) != AWP_OK) {
        return -1;
    }
        
    awpImage* img_hsv = NULL;
    if (awpRGBtoHSVImage(img_rgb, &img_hsv) != AWP_OK) {
        return -2;
    }
    auto img = load_image(image_path);
    img->SetImage(img_hsv);
    
    const std::string& conv_image_path = argv[2];//"d:\\work\\image\\test1_red_conv.jpg";
    auto blue_image = img->GetBlueImage();
    if (blue_image == NULL) {
        return -2;
    }
    if (!save_image(conv_image_path, blue_image)) {        
        return -3;
    }
    std::cout << "end!\n";
    return 0;

    /*
    {
        auto green_image = img->GetGreenImage();
        if (green_image == NULL) {
            return -2;
        }
        const std::string& green_conv_image_path = "d:\\work\\image\\test1_green_conv.jpg";
        if (!save_image(green_conv_image_path, green_image)) {
            return -3;
        }
    }
    {
        auto blue_image = img->GetBlueImage();
        if (blue_image == NULL) {
            return -2;
        }
        const std::string& blue_conv_image_path = "d:\\work\\image\\test1_blue_conv.jpg";
        if (!save_image(blue_conv_image_path, blue_image)) {
            return -3;
        }
    }
    */
    return 0;
}
