#include "JSONSkeletonReader.h"
#include "Skeleton2D.h"

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

        parseBoneData(j, skeleton);

        if(!j["ik"].is_null()) parseIKConstraints(j, skeleton);

        return skeleton;
    }

    return Skeleton2D();
}

void JSONSkeletonReader::parseBoneData(nlohmann::json const & j,
                                       Skeleton2D & skeleton)
{
    for(auto &currJ : j["bones"])
    {
        assert(!currJ["name"].is_null());

        BoneData data;

        data.name = currJ["name"];
        data.parent = currJ.value("parent", "");
        data.offset.x = currJ.value("x", 0.0f);
        data.offset.y = currJ.value("y", 0.0f);
        data.length = currJ.value("length", 0.0f);
        data.rotation = -currJ.value("rotation", 0.0f);

        skeleton.addBone(data);

        //std::cout << data.name << ": rotation = " << data.rotation << "\n";
    }
}

void JSONSkeletonReader::parseIKConstraints(nlohmann::json const & j,
                                            Skeleton2D & skeleton)
{
    for(auto &currJ : j["ik"])
    {
        assert(!currJ["name"].is_null());

        IKConstraintData ikData;

        ikData.name = currJ["name"];

        if(currJ["bones"].size() == 1)
            ikData.firstBone = currJ["bones"][0];
        else if(currJ["bones"].size() == 2)
        {
            ikData.firstBone = currJ["bones"][0];
            ikData.lastBone = currJ["bones"][1];
        }

        ikData.target = currJ.value("target", "");

        skeleton.addIKConstraint(ikData);
    }
}
