#include "gtest/gtest.h"
#include "texconv.h"
#include <filesystem>

TEST(test_non_dds, png_to_dds) {
    wchar_t* argv[] = {
        L"-f", L"BGRA", L"-y",
        L"-srgb",
        L"--", L"test.png",
    };
    int argc = sizeof(argv) / sizeof(char*);
    wchar_t err_buf[128] = {0};
    int err_buf_size = sizeof(err_buf) / sizeof(wchar_t);
    int ret = texconv(argc, argv, 1, 1, 0, err_buf, err_buf_size);
    ASSERT_EQ(0, ret) << "Failed to convert test.png to dds";

    HRESULT hr;
    TexMetadata info;
    DDS_FLAGS ddsFlags = DDS_FLAGS_ALLOW_LARGE_FILES | DDS_FLAGS_IGNORE_MIPS;
    std::unique_ptr<ScratchImage> image(new (std::nothrow) ScratchImage);
    hr = LoadFromDDSFile(L"test.dds", ddsFlags, &info, *image);
    ASSERT_EQ(S_OK, hr) << "Failed to load test.dds";
    uint8_t *pixels = image->GetPixels();
    int height = (int)info.height;
    int width = (int)info.width;
    #define COORD(x, y) "(" << x << ", " << y << ")"
    for(int y = 0; y < height; y++){
        for (int x = 0; x < width; x++) {
            int b = (int)pixels[y * 4 * width + x * 4 + 0];
            int g = (int)pixels[y * 4 * width + x * 4 + 1];
            int r = (int)pixels[y * 4 * width + x * 4 + 2];
            int a = (int)pixels[y * 4 * width + x * 4 + 3];
            ASSERT_EQ((int)((double)y / (double)height * 255), b)
                << "Blue at " << COORD(x, y) << " has an unexpected value.";
            ASSERT_EQ(255, g);
            ASSERT_EQ((int)((double)x / (double)width * 255), r)
                << "Red at " << COORD(x, y) << " has an unexpected value.";
            ASSERT_EQ(255, a);
        }
    }
}

TEST(test_non_dds, png_to_jpg) {
    if (!std::filesystem::exists("jpg")) {
        std::filesystem::create_directory("jpg");
    }
    wchar_t* argv[] = {
        L"-ft", L"jpg",
        L"-o", L"jpg",
        L"-srgb",
        L"-y",
        L"--", L"test.png",
    };
    int argc = sizeof(argv) / sizeof(char*);
    wchar_t err_buf[128] = {0};
    int err_buf_size = sizeof(err_buf) / sizeof(wchar_t);
    int ret = texconv(argc, argv, 1, 1, 0, err_buf, err_buf_size);
    ASSERT_EQ(0, ret) << "Failed to convert test.png to jpg";

    wchar_t* argv2[] = {
        L"-f", L"BGRA",
        L"-o", L"jpg",
        L"-srgb",
        L"-y",
        L"--", L"jpg/test.jpg",
    };
    argc = sizeof(argv) / sizeof(char*);
    ret = texconv(argc, argv2, 1, 1, 0, err_buf, err_buf_size);
    ASSERT_EQ(0, ret) << "Failed to convert jpg/test.jpg to dds";

    HRESULT hr;
    TexMetadata info;
    DDS_FLAGS ddsFlags = DDS_FLAGS_ALLOW_LARGE_FILES | DDS_FLAGS_IGNORE_MIPS;
    std::unique_ptr<ScratchImage> image(new (std::nothrow) ScratchImage);
    hr = LoadFromDDSFile(L"jpg/test.dds", ddsFlags, &info, *image);
    ASSERT_EQ(S_OK, hr) << "Failed to load jpg/test.dds";
    uint8_t *pixels = image->GetPixels();
    int height = (int)info.height;
    int width = (int)info.width;
    #define COORD(x, y) "(" << x << ", " << y << ")"
    for(int y = 0; y < height; y++){
        for (int x = 0; x < width; x++) {
            double b = (double)pixels[y * 4 * width + x * 4 + 0];
            double g = (double)pixels[y * 4 * width + x * 4 + 1];
            double r = (double)pixels[y * 4 * width + x * 4 + 2];
            double a = (double)pixels[y * 4 * width + x * 4 + 3];
            double err = 5.0f;
            ASSERT_NEAR((double)y / (double)height * 255.0f, b, err)
                << "Blue at " << COORD(x, y) << " has an unexpected value.";
            ASSERT_NEAR(255.0f, g, err);
            ASSERT_NEAR((double)x / (double)width * 255.0f, r, err)
                << "Red at " << COORD(x, y) << " has an unexpected value.";
            ASSERT_NEAR(255.0f, a, err);
        }
    }
}
