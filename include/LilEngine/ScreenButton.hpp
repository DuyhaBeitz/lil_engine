#pragma once

#include <algorithm>
#include <string>
#include <raylib.h>
#include <raymath.h>

/*
 * Circular touch button with an icon, laid out relative to the screen
 * (same convention as ScreenJoystick):
 *
 *   position : 0..1 of screen width / height
 *   radius   : fraction of the SHORTER screen side
 *
 * Call Update() once per frame (before reading the state), then Draw().
 * Multi-touch safe: a finger that was already down elsewhere last frame
 * (e.g. dragging the joystick) and slides over the button does NOT press it.
 * Once pressed, the button stays down until that same finger is lifted.
 *
 * The icon is drawn with 'font', which must contain the Font Awesome glyphs:
 *
 *   const char* icons = ICON_FA_ARROW_UP ICON_FA_CROSSHAIRS;      // every icon you use
 *   int count; int* cps = LoadCodepoints(icons, &count);
 *   Font font = LoadFontEx("fa-solid-900.ttf", 128, cps, count);   // FONT_ICON_FILE_NAME_FAS
 *   UnloadCodepoints(cps);
 *   SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
 *
 *   ScreenButton jump({ 0.9f, 0.75f }, 0.08f, ICON_FA_ARROW_UP, font);
 *
 *   jump.Update();
 *   if (jump.IsPressed()) Jump();      // true on the first frame only
 *   if (jump.IsDown())    Charge();    // true every frame while held
 *   if (jump.IsReleased()) Release();  // true on the frame the finger lifts
 *   jump.Draw();
 */
class ScreenButton {
private:
    static constexpr int MAX_TOUCHES = 10;

    Vector2 m_position;             // 0..1
    float m_radius;                 // fraction of the shorter side
    std::string m_icon;
    Font m_font;
    float m_icon_scale = 0.9f;      // icon font size as a fraction of the radius

    int m_touch_id = -1;
    bool m_down = false;
    bool m_was_down = false;

    int m_prev_ids[MAX_TOUCHES];
    int m_prev_count = 0;

    static float Unit() {
        return (float) std::max(1, std::min(GetScreenWidth(), GetScreenHeight()));
    }
    Vector2 CenterPx() const {
        return { m_position.x * (float) GetScreenWidth(), m_position.y * (float) GetScreenHeight() };
    }
    float RadiusPx() const { return m_radius * Unit(); }
    bool WasDownLastFrame(int id) const;
    void RememberFingers();

public:
    ScreenButton(Vector2 position, float radius, std::string icon, Font font) :
            m_position(position), m_radius(radius), m_icon(std::move(icon)), m_font(font) {}

    int GetTouchId() const { return m_touch_id; }   // finger held by this widget, or -1

    void Update();
    void Draw();

    // Releases the button and treats every finger that is down right now as "already down",
    // so none of them can press the button until they are lifted and land again.
    void Reset();

    bool IsDown() const     { return m_down; }
    bool IsPressed() const  { return m_down && !m_was_down; }
    bool IsReleased() const { return !m_down && m_was_down; }

    void SetIcon(const std::string& icon) { m_icon = icon; }
    void SetFont(Font font) { m_font = font; }
    void SetIconScale(float scale) { m_icon_scale = scale; }
};