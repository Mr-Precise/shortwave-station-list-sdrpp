#include <imgui.h>
#include <config.h>
#include <core.h>
#include <gui/style.h>
#include <module.h>
#include <gui/gui.h>
#include <gui/widgets/symbol_diagram.h>

#include <fstream>
#include <curl/curl.h>
#include <iostream>
#include <json.hpp>

SDRPP_MOD_INFO{
    /* Name:            */ "Shortwave Staton List",
    /* Description:     */ "Plugin to show data from shortwave-station-list in SDR++",
    /* Author:          */ "Otto Pattemore",
    /* Version:         */ 0, 1, 0,
    /* Max instances    */ 1};

size_t gotdata(char *buffer, size_t itemSize, size_t nitems, void *f){
    size_t bytes = itemSize*nitems;
    for (uint32_t i = 0; i < bytes; i++){
        std::cout << buffer[i];
    }
    return bytes;
}

class ShortwaveStationList : public ModuleManager::Instance
{
public:
    ShortwaveStationList(std::string name)
    {
        gui::menu.registerEntry(name, menuHandler, this, NULL);

        fftRedrawHandler.ctx = this;
        fftRedrawHandler.handler = fftRedraw;
        gui::waterfall.onFFTRedraw.bindHandler(&fftRedrawHandler);
        CURL* curl = curl_easy_init();
        if(!curl){
            spdlog::error("[ Shortwave Station List ] Failed to init curl!");
            return;
        }

        // Download data
        curl_easy_setopt(curl, CURLOPT_URL, "https://ottopattemore.github.io/shortwave-station-list/sw.json");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gotdata);

        CURLcode result  = curl_easy_perform(curl);
        if(result != CURLE_OK){
            spdlog::error("[ Shortwave Station List ] Failed to download list!");
        }

        // Cleanup
        curl_easy_cleanup(curl);
        spdlog::info("[ Shortwave Station List ] Downloaded station list!");
    }

    ~ShortwaveStationList()
    {
    }

    void postInit() {}

    void enable()
    {
    }

    void disable()
    {
    }

    bool isEnabled()
    {
        return enabled;
    }

private:
    static void menuHandler(void *ctx)
    {
        ShortwaveStationList * _this = (ShortwaveStationList *)ctx;
        ImGui::Text("Hello!");
    }
    static void fftRedraw(ImGui::WaterFall::FFTRedrawArgs args, void *ctx)
    {
        ShortwaveStationList *_this = (ShortwaveStationList *)ctx;
        std::function<void(std::string name, int freq)> draw = [&args](std::string name, int freq)
        {
            double centerXpos = args.min.x + std::round((freq - args.lowFreq) * args.freqToPixelRatio);
            if (freq >= args.lowFreq && freq <= args.highFreq)
            {
                args.window->DrawList->AddLine(ImVec2(centerXpos, args.min.y), ImVec2(centerXpos, args.max.y), IM_COL32(255, 255, 0, 255));
            }
            ImVec2 nameSize = ImGui::CalcTextSize(name.c_str());
            ImVec2 rectMin = ImVec2(centerXpos - (nameSize.x / 2) - 5, args.min.y);
            ImVec2 rectMax = ImVec2(centerXpos + (nameSize.x / 2) + 5, args.min.y + nameSize.y);
            ImVec2 clampedRectMin = ImVec2(std::clamp<double>(rectMin.x, args.min.x, args.max.x), rectMin.y);
            ImVec2 clampedRectMax = ImVec2(std::clamp<double>(rectMax.x, args.min.x, args.max.x), rectMax.y);
            if (clampedRectMax.x - clampedRectMin.x > 0)
            {
                args.window->DrawList->AddRectFilled(clampedRectMin, clampedRectMax, IM_COL32(255, 255, 0, 255));
            }
            if (rectMin.x >= args.min.x && rectMax.x <= args.max.x)
            {
                args.window->DrawList->AddText(ImVec2(centerXpos - (nameSize.x / 2), args.min.y), IM_COL32(0, 0, 0, 255), name.c_str());
            }
        };
        draw("Test1", 100000000);
        draw("Test2", 101000000);
    }
    json stationList;
    std::string name;
    bool enabled = true;
    EventHandler<ImGui::WaterFall::FFTRedrawArgs> fftRedrawHandler;
};

MOD_EXPORT void _INIT_()
{
    // Nothing
}

MOD_EXPORT ModuleManager::Instance *_CREATE_INSTANCE_(std::string name)
{
    return new ShortwaveStationList(name);
}

MOD_EXPORT void _DELETE_INSTANCE_(void *instance)
{
    delete (ShortwaveStationList *)instance;
}

MOD_EXPORT void _END_()
{
    // Nothing either
}