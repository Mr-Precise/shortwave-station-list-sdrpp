#pragma once

struct Settings
{
    bool useLocalHost = false;
    bool showStations = true;

    bool calculateDistances = false;
    float lat = 0;
    float lon = 0;
    float displayR = 0.312;
    float displayG = 0.558;
    float displayB = 0.717;
    float displayA = 1;
    float markerTextColorR = 1;
    float markerTextColorG = 1;
    float markerTextColorB = 1;
    float markerTextColorA = 1;
};
void initSettings();
Settings loadSettings();
void saveSettings(const Settings& settings);