#include "assetsManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "raymob.h"

using std::unordered_map;

vector<string> customSplit(string &str, char separator)
{
    vector<string> strings;

    int startIndex = 0, endIndex = 0;
    for (size_t i = 0; i <= str.size(); i++)
    {
        // If we reached the end of the word or the end of the input.
        if (str[i] == separator || i == str.size())
        {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            strings.push_back(temp);
            startIndex = endIndex + 1;
        }
    }

    return strings;
}

void loadKanas(vector<Kana> &kanas)
{
    //need to use raylib native file text for load files in android
    char *hiraText = LoadFileText("img/hiraganas.txt");
    std::stringstream hiraStream(hiraText);
    UnloadFileText(hiraText);

    string audioPath = "sounds/";
    string audioExtension = ".mp3";

// std::map uses a red-black tree internally → extra heap allocations per insert.
// You don’t need ordering, just lookups by key → better to use unordered_map.
    unordered_map<string, Sound> sounds;
    sounds.reserve(71);

    for (string line; getline(hiraStream, line);)
    {
        auto list = customSplit(line, ',');

        string name = list[0];
        int x = stoi(list[1]);
        int y = stoi(list[2]);
        int width = stoi(list[3]);
        int height = stoi(list[4]);

        Rectangle bounds = {(float)x, (float)y, (float)width, (float)height};

        string actualAudioPath = audioPath + name + audioExtension;
        Sound actualSound = LoadSound(actualAudioPath.c_str());
        SetSoundVolume(actualSound, 0.8);

        sounds[name] = actualSound;

        kanas.push_back({name, bounds, actualSound});
    }

    // --- Load katakana ---
    char *kataText = LoadFileText("img/katakanas.txt");
    std::stringstream kataStream(kataText);
    UnloadFileText(kataText);

    for (string line; getline(kataStream, line);)
    {
        auto list = customSplit(line, ',');

        string name = list[0];
        int x = stoi(list[1]);
        int y = stoi(list[2]);
        int width = stoi(list[3]);
        int height = stoi(list[4]);

        Rectangle bounds = {(float)x, (float)y, (float)width, (float)height};

        kanas.push_back({name, bounds, sounds[name]});
    }
}