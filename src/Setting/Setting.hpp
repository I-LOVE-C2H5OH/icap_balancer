#include <yaml-cpp/yaml.h>
#include "string"
class Setting {
public:
    Setting();
    Setting(const std::string& filepath);
    int sourcePort();
    int distancePort();
    unsigned long maxActiveDistanceConnections();
    std::string distanceHost();
private:
    int m_sourcePort;
    int m_distancePort;
    unsigned long m_maxActiveDistanceConnections;
    std::string m_distanceHost;
};