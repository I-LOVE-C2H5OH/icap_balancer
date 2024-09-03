#include <yaml-cpp/yaml.h>
#include "string"
class Setting {
public:
    Setting();
    Setting(const std::string& filepath);
    int sourcePort() const;
    int distancePort() const;
    unsigned long maxActiveDistanceConnections() const;
    std::string distanceHost() const;
private:
    int m_sourcePort;
    int m_distancePort;
    unsigned long m_maxActiveDistanceConnections;
    std::string m_distanceHost;
};