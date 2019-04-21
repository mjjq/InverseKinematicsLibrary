#include "JSONSkeletonReader.h"
#include "Skeleton.h"

#include <fstream>

const std::string JSONSkeletonReader::FILEPATH = "./";

Skeleton2D JSONSkeletonReader::readFromFile(std::string const & filename, float scaleFactor)
{
    using json = nlohmann::json;
    std::ifstream initInput(FILEPATH + filename);
    if(json::accept(initInput))
    {
        Skeleton2D skeleton;

        std::ifstream input(FILEPATH + filename);
        json j;
        input >> j;

        for(json &currJ : j["bones"])
        {
            assert(!currJ["name"].is_null());

            BoneData data;

            data.name = currJ["name"];
            data.parent = currJ.value("parent", "");
            data.offset.x = currJ.value("x", 0.0f);
            data.offset.y = currJ.value("y", 0.0f);
            data.length = scaleFactor * currJ.value("length", 0.0f);
            data.rotation = currJ.value("rotation", 0.0f);

            skeleton.addBone(data);

            std::cout << data.name << ": rotation = " << data.rotation << "\n";
        }

        return skeleton;
    }

    return Skeleton2D();
}


