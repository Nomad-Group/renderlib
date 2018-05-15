#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11Texture.h"
#include "png.h"
#include <sstream>

bool D3D11Texture::LoadFromPNG(const std::string& path)
{
    Release();

    // Open File
    FILE* pFile = nullptr;
    fopen_s(&pFile, path.c_str(), "rb");

    if (!pFile)
        return false;

    png_byte header[8];
    fread(header, 1, 8, pFile);
    if (png_sig_cmp(header, 0, 8))
	{
        fclose(pFile);
        return false;
    }

    // Setup libpng
    png_structp pPNG = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pPNG)
		return false;

    png_infop pInfo = png_create_info_struct(pPNG);
    if (!pInfo)
	{
        png_destroy_read_struct(&pPNG, nullptr, nullptr);
        fclose(pFile);
        return false;
    }

    // IO
    png_init_io(pPNG, pFile);
    png_set_sig_bytes(pPNG, 8);
    png_read_info(pPNG, pInfo);

    // Size
    const uint32_t uiWidth = png_get_image_width(pPNG, pInfo);
    const uint32_t uiHeight = png_get_image_height(pPNG, pInfo);

    // Color & Depth
    const png_byte iColorType = png_get_color_type(pPNG, pInfo);
    const png_byte iBitDepth = png_get_bit_depth(pPNG, pInfo);

    if (iColorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(pPNG);

    if (iColorType == PNG_COLOR_TYPE_GRAY && iBitDepth < 8)
        png_set_expand_gray_1_2_4_to_8(pPNG);

    if (iColorType == PNG_COLOR_TYPE_RGB || iColorType == PNG_COLOR_TYPE_GRAY || iColorType == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(pPNG, 0xFF, PNG_FILLER_AFTER);

    if (iColorType == PNG_COLOR_TYPE_GRAY || iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(pPNG);

    if (iBitDepth == 16)
        png_set_strip_16(pPNG);

    if (png_get_valid(pPNG, pInfo, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(pPNG);

    png_read_update_info(pPNG, pInfo);

    // Image Data
    png_bytep* ppRows = static_cast<png_bytep*>(malloc(sizeof(png_bytep) * uiHeight));
    for (uint32_t y = 0; y < uiHeight; y++)
        ppRows[y] = static_cast<png_byte*>(malloc(png_get_rowbytes(pPNG, pInfo)));

    png_read_image(pPNG, ppRows);

    // Convert Data
    uint8_t* pImageData = static_cast<uint8_t*>(malloc(uiWidth * uiHeight * 4));

    uint32_t offset = 0;
    for (uint32_t y = 0; y < uiHeight; y++) {
        for (uint32_t x = 0; x < uiWidth; x++) {
            pImageData[offset + 0] = ppRows[y][x * 4 + 0];
            pImageData[offset + 1] = ppRows[y][x * 4 + 1];
            pImageData[offset + 2] = ppRows[y][x * 4 + 2];
            pImageData[offset + 3] = ppRows[y][x * 4 + 3];
            offset += 4;
        }
    }

    // Cleanup libpng
    png_destroy_read_struct(&pPNG, &pInfo, nullptr);
    for (uint32_t y = 0; y < uiHeight; y++)
         free(ppRows[y]);
     
    free(ppRows);
    fclose(pFile);

    // Create Texture
    const bool success = LoadFromMemory(pImageData, uiWidth, uiHeight, ColorFormat::RGBA);
     
    free(pImageData);

    return success;
}
