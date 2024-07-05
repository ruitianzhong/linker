#include "relocation.h"

#include <sys/mman.h>
void handleRela(std::vector<ObjectFile> &allObject, ObjectFile &mergedObject, bool isPIE)
{
    /* When there is more than 1 objects,
     * you need to adjust the offset of each RelocEntry
     */
    /* Your code here */
    uint64_t totalSize = 0;
    for (auto &obj : allObject)
    {
        for (auto &re : obj.relocTable)
        {
            re.offset += totalSize;
        }
        totalSize += obj.sections[".text"].size;
    }

    /* in PIE executables, user code starts at 0xe9 by .text section */
    /* in non-PIE executables, user code starts at 0xe6 by .text section */
    uint64_t userCodeStart = isPIE ? 0xe9 : 0xe6;
    uint64_t textOff = mergedObject.sections[".text"].off + userCodeStart;
    uint64_t textAddr = mergedObject.sections[".text"].addr + userCodeStart;
    for (auto &obj : allObject)
    {

        for (auto &re : obj.relocTable)
        {
            uint64_t addr;
            uint32_t diff;
            switch (re.type)
            {
            case R_X86_64_PC32:
            case R_X86_64_PLT32:
                addr = re.offset + textOff + uint64_t(mergedObject.baseAddr);
                diff = re.sym->value - (re.offset + textAddr) + re.addend;
                *reinterpret_cast<uint32_t *>(addr) = diff;
                break;
            case R_X86_64_32:
                addr = re.offset + textOff + uint64_t(mergedObject.baseAddr);
                diff = re.sym->value + re.addend;
                *reinterpret_cast<uint32_t *>(addr) = diff;
                break;
            default:
                break;
            }
        }
    }

    /* Your code here */
}