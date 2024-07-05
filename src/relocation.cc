#include "relocation.h"

#include <sys/mman.h>
#include <iostream>
void handleRela(std::vector<ObjectFile> &allObject, ObjectFile &mergedObject, bool isPIE)
{
    /* When there is more than 1 objects,
     * you need to adjust the offset of each RelocEntry
     */
    /* Your code here */
    for (auto &obj : allObject)
    {
    }

    /* in PIE executables, user code starts at 0xe9 by .text section */
    /* in non-PIE executables, user code starts at 0xe6 by .text section */
    uint64_t userCodeStart = isPIE ? 0xe9 : 0xe6;
    uint64_t textOff = mergedObject.sections[".text"].off + userCodeStart;
    uint64_t textAddr = mergedObject.sections[".text"].addr + userCodeStart;
    for (auto &re : allObject[0].relocTable)
    {
        std::cout << allObject[0].relocTable.size() << std::endl;
        std::cout << "[Relocation] Symbol name " << re.name << std::endl;

        uint64_t addr;
        uint32_t diff;
        switch (re.type)
        {
        case R_X86_64_PC32:
            addr = re.offset + textAddr + uint64_t(mergedObject.baseAddr);
            std::cout << "ready to write on addr " << std::hex << addr << std::endl;
            diff = re.sym->value - (re.offset + textAddr) + re.addend;
            std::cout << "diff is " << diff << std::endl;
            *reinterpret_cast<uint32_t *>(addr) = diff;
            break;
        case R_X86_64_32:
            break;
        default:
            break;
        }
    }

    /* Your code here */
}