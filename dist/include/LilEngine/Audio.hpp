#pragma once

class Audio {
private:
    float m_sfx_volume = 1.0f;

public:

    float GetSFXVolume();
    void SetSFXVolume(float sfx_volume);
};