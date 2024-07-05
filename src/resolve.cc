#include "resolve.h"

#include <iostream>

#define FOUND_ALL_DEF 0
#define MULTI_DEF 1
#define NO_DEF 2

std::string errSymName;

int callResolveSymbols(std::vector<ObjectFile> &allObjects);

void resolveSymbols(std::vector<ObjectFile> &allObjects) {
    int ret = callResolveSymbols(allObjects);
    if (ret == MULTI_DEF) {
        std::cerr << "multiple definition for symbol " << errSymName << std::endl;
        abort();
    } else if (ret == NO_DEF) {
        std::cerr << "undefined reference for symbol " << errSymName << std::endl;
        abort();
    }
}

static bool isStrong(Symbol *sym)
{
    return sym->bind == STB_GLOBAL && sym->index != SHN_UNDEF && sym->index != SHN_COMMON;
}

static bool isWeak(Symbol * sym){
    return sym->index == SHN_COMMON;
}

static bool isWeakOrStrong(Symbol *sym)
{
    return isStrong(sym) || isWeak(sym);
}

/* bind each undefined reference (reloc entry) to the exact valid symbol table entry
 * Throw correct errors when a reference is not bound to definition,
 * or there is more than one definition.
 */
int callResolveSymbols(std::vector<ObjectFile> &allObjects)
{
    /* Your code here */
    // if found multiple definition, set the errSymName to problematic symbol name and return MULTIDEF;
    // if no definition is found, set the errSymName to problematic symbol name and return NODEF;

    // pre-process the symbols

    auto um = std::unordered_map<std::string, Symbol *>();

    for (auto &obj : allObjects)
    {
        for (auto &sym : obj.symbolTable)
        {
            if (um.find(sym.name) == um.end())
            {
                if (isWeakOrStrong(&sym)){
                    um.insert({sym.name, &sym});
                    }
            }
            else
            {
                if (isWeak(&sym))
                {
                    continue;
                }
                else if (isStrong(&sym))
                {
                    Symbol *cur = um[sym.name];
                    if (cur->bind == STB_GLOBAL)
                    {
                        errSymName = cur->name;
                        return MULTI_DEF;
                    }
                }
                else
                {
                    // do nothing for now
                }
            }
        }
    }

    for (auto &obj : allObjects)
    {
        for (auto &re : obj.relocTable)
        {
            if (um.find(re.name) == um.end())
            {
                errSymName = re.name;
                return NO_DEF;
            }
            // resolve the symbol here
            re.sym = um[re.name];
        }
    }

    return FOUND_ALL_DEF;
}
