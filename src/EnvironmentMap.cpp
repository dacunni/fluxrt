#include "EnvironmentMap.h"

RadianceRGB EnvironmentMap::sampleRay(const Ray & ray)
{
    return RadianceRGB();
}

std::string EnvironmentMap::lookupPath(const std::string & fileName)
{
    // TODO: Move these to a utility module
    auto getEnvVar = [](const char * name) {
        const char * value = getenv(name);
        if(value)
            return std::string(value);
        return std::string("");
    };
    auto applyPathPrefix = [](const std::string & prefix, const std::string & path) {
            // If prefix is defined and path is not absolute
            if(!prefix.empty() && path.front() != '/') {
                    return std::string(prefix) + '/' + path;
            }
            return path;
    };
    std::string envMapPath = getEnvVar("ENV_MAP_PATH");
    return applyPathPrefix(envMapPath, fileName);
}

