#include "AtomicHashMap.h"

int main()
{
    const std::uint32_t k_HashSize(2048);
    std::unique_ptr<Utils::AtomicHashIntMap> hashMap = std::unique_ptr<Utils::AtomicHashIntMap>(new Utils::AtomicHashIntMap(k_HashSize));

    hashMap->SetItem(10, 3);

    hashMap->SetItem(10, 0);

    hashMap->SetItem(10, 4);

    const auto value = hashMap->GetItem(10);

    const auto val = hashMap->GetPrivateCopyOfContent();

    return 0;
}