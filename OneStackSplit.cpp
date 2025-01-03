#include "pch.h"
// Stack splits distrubite 1 creature to the new stack instead of half of the original size

void OneStackSplit_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_WRTE, 0x007866A6, 6, nullptr, 0, 0, 0, "B801000000C3" });
    assembly_patches.push_back({ PATCH_WRTE, 0x00786A3B, 8, nullptr, 0, 0, 0, "B801000000909090" });
}
