#include "JSONSkeletonReader.h"
#include "Skeleton2D.h"
#include "SkeletonAnimation.h"
#include "BoneAnimation.h"

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

        if(!j["animations"].is_null()) parseAnimationData(j, skeleton);

        if(!j["slots"].is_null()) parseSlotData(j, skeleton);

        if(!j["skins"].is_null()) parseSkinData(j, skeleton);

        if(!j["skeleton"].is_null()) parseGeneralData(j, skeleton);

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

void JSONSkeletonReader::parseAnimationData(nlohmann::json const & j,
                                            Skeleton2D & skeleton)
{
    for(auto &animationIter : j["animations"].items())
    {
        SkeletonAnimation skelAnimation(animationIter.key());

        nlohmann::json animation = animationIter.value();
        for(auto &boneJsonIter : animation["bones"].items())
        {
            BoneAnimationData animData;
            animData.boneName = boneJsonIter.key();

            float animationDurationTime = 0.0f;

            nlohmann::json boneJsonData = boneJsonIter.value();
            if(boneJsonData.find("rotate") != boneJsonData.end())
                for(auto &rotationalData : boneJsonData["rotate"])
                {
                    float time = rotationalData.value("time", 0.0f);
                    float angle = rotationalData.value("angle", 0.0f);

                    //be wary of abs(angles) >180.0 as they may interpolate wrong
                    if(angle > 180.0f) angle -= 360.0f;
                    if(angle < -180.0f) angle += 360.0f;
                    animData.rotationData.push_back({time, -angle});

                    if(time > animationDurationTime) animationDurationTime = time;
                }
            if(boneJsonData.find("translate") != boneJsonData.end())
                for(auto &translationData : boneJsonData["translate"])
                {
                    float time = translationData["time"];
                    sf::Vector2f position = {translationData["x"], translationData["y"]};
                    position.y = -position.y;
                    animData.translationData.push_back({time, position});

                    if(time > animationDurationTime) animationDurationTime = time;
                }

            animData.duration = animationDurationTime;

            skelAnimation.addBoneAnimation(BoneAnimation(animData));
        }

        skeleton.addAnimation(skelAnimation);
    }
}


void JSONSkeletonReader::parseSlotData(nlohmann::json const & j,
                              Skeleton2D & skeleton)
{
    SlotData data;
    for(auto &currJ : j["slots"])
    {
        data.name = currJ.value("name", "");
        data.bone = currJ.value("bone", "");
        data.attachment = currJ.value("attachment", "");

        skeleton.addSlot(data);
    }
}

void JSONSkeletonReader::parseSkinData(nlohmann::json const & j,
                          Skeleton2D & skeleton)
{
    SkinData data;

    auto currJ = j["skins"];
    for(auto &boneJsonIter : currJ["default"].items())
    {
        data.name = boneJsonIter.key();
        nlohmann::json bone = boneJsonIter.value();

        for(auto &attachmentIter : bone.items())
        {
            data.attachment = attachmentIter.key();
            nlohmann::json attachmentParams = attachmentIter.value();

            data.offset.x = attachmentParams.value("x", 0.0f);
            data.offset.y = attachmentParams.value("y", 0.0f);

            data.rotation = attachmentParams.value("rotation", 0.0f);

            data.size.x = attachmentParams.value("width", 0.0f);
            data.size.y = attachmentParams.value("height", 0.0f);

            skeleton.addSkin(data);
        }
    }
}

void JSONSkeletonReader::parseGeneralData(nlohmann::json const & j,
                                          Skeleton2D & skeleton)
{
    GeneralData data;

    auto currJ = j["skeleton"];

    data.imageSubfolder = currJ.value("images", "");
    data.audioSubfolder = currJ.value("audio", "");

    skeleton.setGeneralData(data);
}
