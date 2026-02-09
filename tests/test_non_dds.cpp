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
    ScratchImage image;
    hr = LoadFromDDSFile(L"jpg/test.dds", ddsFlags, &info, image);
    ASSERT_EQ(S_OK, hr) << "Failed to load jpg/test.dds";
    uint8_t *pixels = image.GetPixels();
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

TEST(test_non_dds, exr_to_dds) {
    if (!std::filesystem::exists("exr")) {
        std::filesystem::create_directory("exr");
    }

    // Save an HDR texture as DDS
    ScratchImage hdr;
    hdr.Initialize2D(
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        512,
        512,
        1,
        1
    );

    const Image* img = hdr.GetImage(0, 0, 0);
    auto pixels = reinterpret_cast<XMFLOAT4*>(img->pixels);

    for (size_t x = 0; x < img->width; x++) {
        for (size_t y = 0; y < img->height; y++) {
            float r = 2.0f * (float)x / (float)img->width;
            float g = 1.0f;
            float b = 2.0f - 2.0f * (float)y / (float)img->height;
            float a = 1.0f;
            pixels[y * img->width + x] = XMFLOAT4(r, g, b, a);
        }
    }

    HRESULT hr;
    hr = SaveToDDSFile(
        hdr.GetImages(),
        hdr.GetImageCount(),
        hdr.GetMetadata(),
        DDS_FLAGS_NONE,
        L"exr/hdr.dds"
    );
    EXPECT_EQ(S_OK, hr) << "Failed to generate exr/hdr.dds";

    // Convert hdr.dds to exr
    wchar_t* argv[] = {
        L"-ft", L"exr",
        L"-o", L"exr",
        L"-y",
        L"--", L"exr/hdr.dds",
    };
    int argc = sizeof(argv) / sizeof(char*);
    wchar_t err_buf[128] = {0};
    int err_buf_size = sizeof(err_buf) / sizeof(wchar_t);
    int ret = texconv(argc, argv, 1, 1, 0, err_buf, err_buf_size);
    ASSERT_EQ(0, ret) << "Failed to convert exr/hdr.dds to exr";

    // Convert hdr.exr to dds again
    wchar_t* argv2[] = {
        L"-f", L"R32G32B32A32_FLOAT",
        L"-o", L"exr",
        L"-y",
        L"--", L"exr/hdr.exr",
    };
    argc = sizeof(argv) / sizeof(char*);
    ret = texconv(argc, argv, 1, 1, 0, err_buf, err_buf_size);
    ASSERT_EQ(0, ret) << "Failed to convert exr/hdr.exr to dds";

    // Check the result
    TexMetadata info;
    DDS_FLAGS ddsFlags = DDS_FLAGS_ALLOW_LARGE_FILES | DDS_FLAGS_IGNORE_MIPS;
    ScratchImage image;
    hr = LoadFromDDSFile(L"exr/hdr.dds", ddsFlags, &info, image);
    ASSERT_EQ(S_OK, hr) << "Failed to load exr/hdr.dds";
    pixels = reinterpret_cast<XMFLOAT4*>(image.GetPixels());
    int height = (int)info.height;
    int width = (int)info.width;
    for(int y = 0; y < height; y++){
        for (int x = 0; x < width; x++) {
            XMFLOAT4 pixel = pixels[y * width + x];
            float expected_r = 2.0f * (float)x / (float)img->width;
            float expected_g = 1.0f;
            float expected_b = 2.0f - 2.0f * (float)y / (float)img->height;
            float expected_a = 1.0f;
            float err = 0.01f;
            ASSERT_NEAR(expected_r, pixel.x, err);
            ASSERT_NEAR(expected_g, pixel.y, err);
            ASSERT_NEAR(expected_b, pixel.z, err);
            ASSERT_NEAR(expected_a, pixel.w, err);
        }
    }
}
