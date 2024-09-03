# include "Setting.hpp"
#include <iostream>
#include <fstream>
Setting::Setting(): m_distanceHost("127.0.0.1"), m_sourcePort(1344), m_distancePort(1345), m_maxActiveDistanceConnections(10) {}

Setting::Setting(const std::string& filepath)
{
    try {
            YAML::Node config = YAML::LoadFile(filepath);
            if (config["Distance_host"]) {
                m_distanceHost = config["Distance_host"].as<std::string>();
            }
            if (config["Disctance_port"]) {
                m_distancePort = config["Disctance_port"].as<int>();
            }
            if (config["Source_port"]) {
                m_sourcePort = config["Source_port"].as<int>();
            }
            if (config["Max_active_dist_connections"]) {
                m_maxActiveDistanceConnections = config["Max_active_dist_connections"].as<unsigned long>();
            }
        } catch (const std::exception &e) {
            std::cerr << "Ошибка загрузки файла: " << e.what() << std::endl;
        }
}

int Setting::sourcePort() const
{
    return m_sourcePort;
}

int Setting::distancePort() const
{
    return m_distancePort;
}

unsigned long Setting::maxActiveDistanceConnections() const
{
    return m_maxActiveDistanceConnections;
}

std::string Setting::distanceHost() const
{
    return m_distanceHost;
}
