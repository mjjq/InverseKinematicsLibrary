#ifndef SKELETON_READER_H
#define SKELETON_READER_H

#include "../extern/json.hpp"
#include "Skeleton2D.h"

class JSONSkeletonReader
{

    static const std::string FILEPATH;

    static void parseBoneData(nlohmann::json const & j,
                              Skeleton2D & skeleton);
    static void parseIKConstraints(nlohmann::json const & j,
                                   Skeleton2D & skeleton);
    static void parseAnimationData(nlohmann::json const & j,
                                   Skeleton2D & skeleton);
    static void parseSlotData(nlohmann::json const & j,
                              Skeleton2D & skeleton);
    static void parseSkinData(nlohmann::json const & j,
                              Skeleton2D & skeleton);
public:
    static Skeleton2D readFromFile(std::string const & filename, float scaleFactor=1.0f);
};

#endif // SKELETON_READER_H
