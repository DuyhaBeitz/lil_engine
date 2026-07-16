// #pragma once

// #include "ResourceManager.hpp"
// #include <nfd.h>

// inline void BrowseTexture() {
    
//     NFD_Init();

//     nfdu8char_t *outPath;
//     nfdu8filteritem_t filters[1] = { { "Image", "png,jpeg" } };
//     nfdopendialogu8args_t args = {0};
//     args.filterList = filters;
//     args.filterCount = 1;
//     nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);

//     if (result == NFD_OKAY)
//     {
//         std::string filename = outPath;
//         NFD_FreePathU8(outPath);
//         Lil::Resources().AddTexture(NameFromPath(filename), filename);
//     }
//     else if (result == NFD_CANCEL)
//     {
//         LOG_INFO("User pressed cancel.");
//     }
//     else 
//     {
//         LOG_ERROR(NFD_GetError());
//     }

//     NFD_Quit();
// }

// inline void BrowseModel() {
    
//     NFD_Init();

//     nfdu8char_t *outPath;
//     nfdu8filteritem_t filters[1] = { { "Model", "glb" } };
//     nfdopendialogu8args_t args = {0};
//     args.filterList = filters;
//     args.filterCount = 1;
//     nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);

//     if (result == NFD_OKAY)
//     {
//         std::string filename = outPath;
//         NFD_FreePathU8(outPath);
//         Lil::Resources().AddModel(NameFromPath(filename), filename);
//     }
//     else if (result == NFD_CANCEL)
//     {
//         LOG_INFO("User pressed cancel.");
//     }
//     else 
//     {
//         LOG_ERROR(NFD_GetError());
//     }

//     NFD_Quit();
// }