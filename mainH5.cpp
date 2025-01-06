#include "pch.h"
#include "mainH5.h"
#include <iostream>
#include <fstream>

std::vector<assembly_patch> assembly_patches;

void init_patches(pugi::xml_document& doc) {
    ArcaneRenewalFix_init(doc);
    EmpoweredArmageddon_init(doc);
    EnlightenmentForBarbsFix_init(doc);
    ImbueBalista_init(doc);
    SnareFix_init(doc);
    RuneOfTheDragonForm_init(doc);
    CombatAIFix_init(doc);
    AgilityFix_init(doc);
    OneStackSplit_init(doc);
    BookOfPowerLevelUp_init(doc);
    MasterOfFireFix_init(doc);
    EncourageFix_init(doc);
    EliteCasters_init(doc);
}

int main() {
    initLog();
    writeLog(INFO, "Opening game files...");

    // ########### Get mod settings
    pugi::xml_document h5_stats;

    // ########### Apply assembly patches
    writeLog(INFO, "Initialize patches");
    init_patches(h5_stats);
    writeLog(INFO, "Commence patching...");
    for (const auto& patch : assembly_patches) {
        unsigned char* byteArray = new unsigned char[100];
        switch (patch.type) {
        case PATCH_HOOK:
            JumpToFunction((void*)patch.address, patch.hookedFunction, patch.size, 0xE9);
            break;
        case HOOK_JE:
            ConditionalJumpToFunction((void*)patch.address, patch.hookedFunction, patch.size, 0x84);
            break;
        case HOOK_JNE:
            ConditionalJumpToFunction((void*)patch.address, patch.hookedFunction, patch.size, 0x85);
            break;
        case PATCH_BYTE:
            assignByteToAddress(patch.address, &patch.value_int);
            break;
        case PATCH_INT:
            assignValueToAddress((int*)patch.address, patch.value_int);
            break;
        case PATCH_NOP:
            AssignNopToAddressRange((int*)patch.address, patch.size);
            break;
        case PATCH_FLOAT:
            assignValueToAddress((float*)patch.address, patch.value_float);
            break;
        case PATCH_FLOAT_PTR:
            assignFloatPtrToAddress(reinterpret_cast<float**>(patch.address), &patch.value_float);
            break;
        case PATCH_CALL:
            JumpToFunction((void*)patch.address, patch.hookedFunction, patch.size, 0xE8);
            break;
        case PATCH_DOUBLE_PTR:
            assignDoublePtrToAddress(reinterpret_cast<double**>(patch.address), &patch.value_double);
            break;
        case PATCH_WRTE:
            DWORD oldProtect;
            if (!VirtualProtect(reinterpret_cast<LPVOID>(patch.address), patch.size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                writeLog(ERRROR, "Cannot enable the current protection settings!");
                return false;
            }
            for (size_t i = 0; i < patch.size; i++) {
                sscanf_s(patch.value_bytes + 2 * i, "%2hhx", &byteArray[i]);
                void* memoryAddress = reinterpret_cast<void*>(patch.address);
                unsigned char* targetAddress = static_cast<unsigned char*>(memoryAddress);
                for (size_t i = 0; i < patch.size; i++) {
                    targetAddress[i] = byteArray[i];
                }
            }
            break;
        default:
            writeLog(ERRROR, " Unsupported patch type!");
            break;
        }
        delete[] byteArray;
    }
    writeLog(INFO, "Patching completeled!");
}