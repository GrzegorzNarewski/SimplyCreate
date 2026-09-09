#include "Helper.h"

namespace gll
{
    float randomiseFloat(float LO, float HI)
    {
        return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
    }
    int randomiseInt(int LO, int HI)
    {
        return LO + rand() / (RAND_MAX / (HI - LO));
    }
}