#pragma once

class Audio {
private:
    float m_sfx_volume = 1.0f;

public:

    float GetSFXVolume() { return m_sfx_volume; }
    void SetSFXVolume(float sfx_volume) { m_sfx_volume = sfx_volume; }
};