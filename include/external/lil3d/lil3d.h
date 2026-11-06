/*
 * lil3d.h - Lil3D: a tiny, fully portable lighting/shadow layer for raylib 6.0
 *
 * Desktop, Web (emscripten) and Mobile (Android/iOS) using ONLY raylib/rlgl.
 * API is modelled on r3d v0.11.0 (https://github.com/Bigfoot71/r3d), prefix LIL3D_.
 *
 * Features
 *   - Directional / spot / omni lights (integer handles, 0 = invalid)
 *   - Shadow maps for all light types (packed-depth RGBA8, no depth-texture ext.)
 *   - Model/mesh instancing (hardware on GL3.3/ES3, automatic fallback on GL2.1/ES2)
 *   - Cubemap sky (cross/line/panorama layouts + procedural gradient sky)
 *   - All shaders embedded; one GLSL source, adapted at runtime to the GL flavour
 *   - Simple PBR (metallic-roughness, GGX) with sky-based ambient + reflections
 *   - Skeletal animation: animation libs + animation player (blending, crossfade),
 *     per-animation FPS read from the asset, portable CPU skinning
 *   - Per-material UV offset/scale (uvOffset / uvScale)
 *   - Heightmap mesh generator (centred on X/Z like r3d), light debug shapes
 *
 * Usage (in exactly ONE .c file):
 *
 *     #define LIL3D_IMPLEMENTATION
 *     #include "lil3d/lil3d.h"
 *
 *     InitWindow(...);
 *     LIL3D_Init(w, h);
 *     ...
 *     BeginDrawing();
 *         LIL3D_Begin(camera);
 *             LIL3D_DrawMesh(mesh, material, position, 1.0f);
 *         LIL3D_End();
 *         DrawFPS(10, 10);                    // 2D drawing after End()
 *     EndDrawing();
 *
 * PBR (metallic-roughness):
 *
 *     LIL3D_Material mat = LIL3D_GetDefaultMaterial();
 *     mat.albedo.color = GOLD;
 *     mat.orm.metalness = 1.0f;            // 0 = dielectric, 1 = metal
 *     mat.orm.roughness = 0.25f;           // 0 = polished, 1 = matte
 *     // Optional packed glTF-style map (R occlusion, G roughness, B metalness):
 *     // mat.orm.texture = LoadTexture("orm.png");  (factors above multiply G and B)
 *     LIL3D_GetEnvironment()->background.sky = LIL3D_GenGradientSky(64, BLUE, WHITE, DARKGRAY);
 *     // -> metals reflect the sky; without a sky they reflect the ambient colour
 *
 * Animation:
 *
 *     LIL3D_Model model = LIL3D_LoadModel("character.glb");
 *     LIL3D_AnimationLib lib = LIL3D_LoadAnimationLib("character.glb");   // FPS read per clip
 *     LIL3D_AnimationPlayer player = LIL3D_LoadAnimationPlayer(model, lib);
 *     LIL3D_PlayAnimationByName(&player, "Idle", true);
 *     ...
 *     LIL3D_UpdateAnimationPlayer(&player, GetFrameTime());               // real seconds
 *     LIL3D_CrossfadeAnimation(&player, LIL3D_GetAnimationIndex(lib, "Run"), true, 0.25f);
 *     LIL3D_Begin(camera);
 *         LIL3D_DrawAnimatedModel(model, &player, position, 1.0f);
 *     LIL3D_End();
 *
 * Debug (r3d style, after LIL3D_End):
 *
 *     BeginMode3D(camera);
 *         LIL3D_DrawLightShape(light);
 *     EndMode3D();
 *
 * Configuration macros (define before including):
 *   LIL3D_MAX_LIGHTS    simultaneous lights per frame   (default 8)
 *   LIL3D_MAX_SHADOWS   simultaneous shadow casters 1-4 (default 4)
 *   LIL3D_DEBUG_SHADERS print the final GLSL sources to the log
 *   LIL3D_ANIM_RESAMPLE_FPS  FPS of glTF/M3D clips as resampled by raylib (default 1000/17)
 *   LIL3D_NO_ANIMATION_NAME  define if your raylib ModelAnimation has no 'name' field
 *
 * License: zlib
 */

#ifndef LIL3D_H
#define LIL3D_H

#include "raylib.h"
#include <stdbool.h>

#define LIL3D_VERSION_MAJOR 0
#define LIL3D_VERSION_MINOR 3
#define LIL3D_VERSION_PATCH 0
#define LIL3D_VERSION       "0.3.0"

#ifndef LIL3D_MAX_LIGHTS
#   define LIL3D_MAX_LIGHTS 8
#endif
#ifndef LIL3D_MAX_SHADOWS
#   define LIL3D_MAX_SHADOWS 4
#endif
#if (LIL3D_MAX_SHADOWS < 1) || (LIL3D_MAX_SHADOWS > 4)
#   error "LIL3D_MAX_SHADOWS must be in [1, 4]"
#endif
#if (LIL3D_MAX_LIGHTS < LIL3D_MAX_SHADOWS)
#   error "LIL3D_MAX_LIGHTS must be >= LIL3D_MAX_SHADOWS"
#endif

#ifndef LIL3D_API
#   define LIL3D_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/* Types                                                                      */
/* ========================================================================== */

/** Light handle. 0 is the invalid handle (same convention as r3d >= 0.11). */
typedef unsigned int LIL3D_Light;

typedef enum LIL3D_LightType {
    LIL3D_LIGHT_DIR  = 0,   /**< Sun-like. 'range' = shadow radius around the camera focus */
    LIL3D_LIGHT_SPOT = 1,   /**< Cone light                                               */
    LIL3D_LIGHT_OMNI = 2    /**< Point light                                              */
} LIL3D_LightType;

typedef enum LIL3D_BlendMode {
    LIL3D_BLEND_OPAQUE = 0,
    LIL3D_BLEND_ALPHA,      /**< Drawn after opaque + sky, sorted back-to-front */
    LIL3D_BLEND_ADDITIVE
} LIL3D_BlendMode;

typedef enum LIL3D_CullMode {
    LIL3D_CULL_NONE = 0,
    LIL3D_CULL_BACK,
    LIL3D_CULL_FRONT
} LIL3D_CullMode;

typedef enum LIL3D_ShadowCastMode {
    LIL3D_SHADOW_CAST_ON = 0,
    LIL3D_SHADOW_CAST_OFF
} LIL3D_ShadowCastMode;

typedef enum LIL3D_CubemapLayout {
    LIL3D_CUBEMAP_LAYOUT_AUTO_DETECT = 0,
    LIL3D_CUBEMAP_LAYOUT_LINE_VERTICAL,
    LIL3D_CUBEMAP_LAYOUT_LINE_HORIZONTAL,
    LIL3D_CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR,
    LIL3D_CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE,
    LIL3D_CUBEMAP_LAYOUT_PANORAMA            /**< Equirectangular (LDR, converted on the CPU) */
} LIL3D_CubemapLayout;

/** Pre-init configuration, see LIL3D_SetHint(). */
typedef enum LIL3D_Hint {
    LIL3D_HINT_SHADOW_MAP_SIZE = 0,     /**< Dir/spot shadow map size in px (default 1024)   */
    LIL3D_HINT_OMNI_SHADOW_MAP_SIZE,    /**< Omni shadow *face* size in px (default 512)     */
    LIL3D_HINT_SHADOW_FILTER,           /**< 0 = hard, 1 = 3x3 PCF (default), 2 = 5x5 PCF    */
    LIL3D_HINT_COUNT
} LIL3D_Hint;

typedef struct LIL3D_Material {
    struct {
        Texture2D texture;              /**< Defaults to raylib's 1x1 white texture */
        Color color;
    } albedo;
    struct {
        Color color;                    /**< Default BLACK (no emission) */
        float energy;                   /**< Default 1.0                 */
    } emission;
    struct {
        Texture2D texture;              /**< glTF-style packed map: R occlusion, G roughness, B metalness.
                                             Defaults to the 1x1 white texture (= factors only)          */
        float occlusion;                /**< AO strength [0,1], default 1 (only matters with a texture)   */
        float roughness;                /**< Multiplies texture G, default 0.5                            */
        float metalness;                /**< Multiplies texture B, default 0                              */
    } orm;
    Vector2 uvOffset;                   /**< Added to the UVs after scaling. Default (0,0)  */
    Vector2 uvScale;                    /**< Multiplies the UVs (tiling). Default (1,1)     */
    float alphaCutoff;                  /**< > 0 enables alpha test (also in shadows) */
    LIL3D_BlendMode blendMode;
    LIL3D_CullMode cullMode;
    LIL3D_ShadowCastMode shadowCastMode;
} LIL3D_Material;

/** Thin wrapper around a raylib Mesh (GPU-uploaded) + local AABB. */
typedef struct LIL3D_Mesh {
    Mesh mesh;
    BoundingBox aabb;
} LIL3D_Mesh;

/** Bind-pose skeleton (model space, same layout as raylib's Model). Owned by the model. */
typedef struct LIL3D_Skeleton {
    int boneCount;
    BoneInfo* bones;                    /**< name + parent index                     */
    Transform* bindPose;                /**< model-space bind transforms             */
    Matrix* invBind;                    /**< inverse bind matrices (precomputed)     */
} LIL3D_Skeleton;

typedef struct LIL3D_Model {
    LIL3D_Skeleton skeleton;            /**< boneCount == 0 for static models */
    LIL3D_Mesh* meshes;
    int meshCount;
    LIL3D_Material* materials;
    int materialCount;
    int* meshMaterials;                 /**< meshMaterials[i] = index into materials */
    BoundingBox aabb;
} LIL3D_Model;

/** One animation clip. Poses are stored per frame, per bone, in model space. */
typedef struct LIL3D_Animation {
    char name[32];
    int boneCount;
    int frameCount;
    float fps;                          /**< Frames per second, retrieved from the asset */
    float duration;                     /**< frameCount/fps: the loop period, seconds    */
    Transform** framePoses;             /**< [frame][bone] (owned by the lib)            */
} LIL3D_Animation;

typedef struct LIL3D_AnimationLib {
    LIL3D_Animation* animations;
    int count;
    ModelAnimation* raw;                /**< internal: raylib storage backing framePoses */
} LIL3D_AnimationLib;

/** Playback state of one animation inside a player (one state per animation). */
typedef struct LIL3D_AnimationState {
    float time;                         /**< Seconds                                      */
    float speed;                        /**< Multiplier, negative plays backwards (def 1) */
    float weight;                       /**< Blend weight; weights are normalised         */
    bool playing;
    bool loop;
    float targetWeight;                 /**< Used by LIL3D_CrossfadeAnimation()           */
    float fadeRate;                     /**< Weight units per second towards the target   */
} LIL3D_AnimationState;

typedef struct LIL3D_AnimationPlayer {
    LIL3D_AnimationLib lib;             /**< Not owned                                    */
    LIL3D_AnimationState* states;       /**< lib.count entries, public: edit freely       */
    int stateCount;
    float speed;                        /**< Global speed multiplier (default 1)          */

    /* internal */
    LIL3D_Skeleton skeleton;            /**< Not owned (borrowed from the model)          */
    Transform* pose;                    /**< Current blended model-space pose             */
    Transform* scratch;
    Matrix* skin;                       /**< invBind * pose, one per bone                 */
    LIL3D_Mesh* skinMeshes;             /**< CPU-skinned dynamic copies (vertexCount 0 = rigid) */
    int skinMeshCount;
    bool dirty;
} LIL3D_AnimationPlayer;

/** CPU-side array of instance transforms; uploaded when drawn. */
typedef struct LIL3D_InstanceBuffer {
    Matrix* transforms;
    int capacity;
} LIL3D_InstanceBuffer;

typedef struct LIL3D_Cubemap {
    unsigned int texture;               /**< GL cubemap id, 0 = none */
    int size;
    Color average;                      /**< Average colour (used by ambient.fromSky) */
} LIL3D_Cubemap;

typedef struct LIL3D_Environment {
    struct {
        Color color;                    /**< Framebuffer clear colour               */
        float energy;                   /**< Sky brightness multiplier              */
        LIL3D_Cubemap sky;              /**< texture == 0 -> no sky                 */
    } background;
    struct {
        Color color;
        float energy;
        bool fromSky;                   /**< Use sky.average instead of 'color'     */
    } ambient;
} LIL3D_Environment;

/* ========================================================================== */
/* Core / frame control                                                       */
/* ========================================================================== */

/** Call after InitWindow(). Returns false on unsupported GL or shader failure.
 *  resWidth/resHeight are accepted for r3d API parity; Lil3D renders straight
 *  into the active framebuffer (keeps raylib's MSAA working on every platform). */
LIL3D_API bool LIL3D_Init(int resWidth, int resHeight);
LIL3D_API void LIL3D_Close(void);

/** Set a hint. Must be called BEFORE LIL3D_Init(). */
LIL3D_API void LIL3D_SetHint(LIL3D_Hint hint, int value);

/** True when instancing uses real GPU instancing (GL3.3+/ES3). Otherwise it is
 *  emulated with one draw call per instance, so keep instance counts small. */
LIL3D_API bool LIL3D_HasHardwareInstancing(void);

/** Begin a frame; submit draws; LIL3D_End() renders shadows, scene and sky into
 *  the current framebuffer. The target is CLEARED with environment background.color.
 *  Call between BeginDrawing()/EndDrawing(), outside BeginTextureMode(). */
LIL3D_API void LIL3D_Begin(Camera3D camera);
/** Same as LIL3D_Begin(), but renders into 'target' (a raylib RenderTexture). */
LIL3D_API void LIL3D_BeginEx(Camera3D camera, RenderTexture2D target);
LIL3D_API void LIL3D_End(void);

LIL3D_API LIL3D_Environment* LIL3D_GetEnvironment(void);

/* ========================================================================== */
/* Lights                                                                     */
/* ========================================================================== */

LIL3D_API LIL3D_Light LIL3D_CreateLight(LIL3D_LightType type);
LIL3D_API void LIL3D_DestroyLight(LIL3D_Light light);
LIL3D_API bool LIL3D_IsLightExist(LIL3D_Light light);
LIL3D_API LIL3D_LightType LIL3D_GetLightType(LIL3D_Light light);

LIL3D_API bool LIL3D_IsLightActive(LIL3D_Light light);
LIL3D_API void LIL3D_SetLightActive(LIL3D_Light light, bool active);
LIL3D_API void LIL3D_ToggleLight(LIL3D_Light light);

LIL3D_API Color LIL3D_GetLightColor(LIL3D_Light light);
LIL3D_API void LIL3D_SetLightColor(LIL3D_Light light, Color color);
LIL3D_API Vector3 LIL3D_GetLightColorV(LIL3D_Light light);              /* 0..1 */
LIL3D_API void LIL3D_SetLightColorV(LIL3D_Light light, Vector3 color);  /* 0..1 */

LIL3D_API Vector3 LIL3D_GetLightPosition(LIL3D_Light light);
LIL3D_API void LIL3D_SetLightPosition(LIL3D_Light light, Vector3 position);
LIL3D_API Vector3 LIL3D_GetLightDirection(LIL3D_Light light);
LIL3D_API void LIL3D_SetLightDirection(LIL3D_Light light, Vector3 direction);
LIL3D_API void LIL3D_LightLookAt(LIL3D_Light light, Vector3 position, Vector3 target);

LIL3D_API float LIL3D_GetLightEnergy(LIL3D_Light light);
LIL3D_API void LIL3D_SetLightEnergy(LIL3D_Light light, float energy);
LIL3D_API float LIL3D_GetLightSpecular(LIL3D_Light light);
LIL3D_API void LIL3D_SetLightSpecular(LIL3D_Light light, float specular);

/** Spot/omni: max reach. Directional: shadow radius around the camera focus. */
LIL3D_API float LIL3D_GetLightRange(LIL3D_Light light);
LIL3D_API void LIL3D_SetLightRange(LIL3D_Light light, float range);
/** Falloff exponent applied to (1 - dist/range). Default 2. */
LIL3D_API float LIL3D_GetLightAttenuation(LIL3D_Light light);
LIL3D_API void LIL3D_SetLightAttenuation(LIL3D_Light light, float attenuation);

/** Spot cone angles in degrees (half-angles). Defaults 22.5 / 45. */
LIL3D_API float LIL3D_GetLightInnerCutOff(LIL3D_Light light);
LIL3D_API void LIL3D_SetLightInnerCutOff(LIL3D_Light light, float degrees);
LIL3D_API float LIL3D_GetLightOuterCutOff(LIL3D_Light light);
LIL3D_API void LIL3D_SetLightOuterCutOff(LIL3D_Light light, float degrees);

LIL3D_API bool LIL3D_IsShadowEnabled(LIL3D_Light light);
LIL3D_API void LIL3D_EnableShadow(LIL3D_Light light);
LIL3D_API void LIL3D_DisableShadow(LIL3D_Light light);
/** Depth bias, normalised to the shadow depth range. Default 0.0005. */
LIL3D_API float LIL3D_GetShadowBias(LIL3D_Light light);
LIL3D_API void LIL3D_SetShadowBias(LIL3D_Light light, float bias);
/** PCF kernel scale. 1 = sharp-ish, 2+ = blurrier. Default 1. */
LIL3D_API float LIL3D_GetShadowSoftness(LIL3D_Light light);
LIL3D_API void LIL3D_SetShadowSoftness(LIL3D_Light light, float softness);

/* ========================================================================== */
/* Materials / meshes / models / instances                                    */
/* ========================================================================== */

LIL3D_API LIL3D_Material LIL3D_GetDefaultMaterial(void);

/** Takes ownership of a raylib Mesh (uploads it if needed). */
LIL3D_API LIL3D_Mesh LIL3D_LoadMesh(Mesh mesh);
LIL3D_API void LIL3D_UnloadMesh(LIL3D_Mesh mesh);

LIL3D_API LIL3D_Mesh LIL3D_GenMeshPlane(float width, float length, int resX, int resZ);
LIL3D_API LIL3D_Mesh LIL3D_GenMeshCube(float width, float height, float length);
LIL3D_API LIL3D_Mesh LIL3D_GenMeshSphere(float radius, int rings, int slices);
LIL3D_API LIL3D_Mesh LIL3D_GenMeshHemiSphere(float radius, int rings, int slices);
LIL3D_API LIL3D_Mesh LIL3D_GenMeshCylinder(float radius, float height, int slices);
LIL3D_API LIL3D_Mesh LIL3D_GenMeshCone(float radius, float height, int slices);
LIL3D_API LIL3D_Mesh LIL3D_GenMeshTorus(float radius, float size, int radSeg, int sides);
LIL3D_API LIL3D_Mesh LIL3D_GenMeshKnot(float radius, float size, int radSeg, int sides);
/** Heightmap terrain. Positioned like r3d: centred on the origin in X/Z, base at y = 0,
 *  top at y = size.y (white pixel). Spans size.x by size.z. Images that would need more
 *  than 65535 vertices are sampled with a stride. */
LIL3D_API LIL3D_Mesh LIL3D_GenMeshHeightmap(Image heightmap, Vector3 size);

/** Loads through raylib's LoadModel() (OBJ/GLTF/IQM/VOX/M3D). Skeleton + bone weights
 *  are kept; animations are loaded separately with LIL3D_LoadAnimationLib(). */
LIL3D_API LIL3D_Model LIL3D_LoadModel(const char* filePath);
LIL3D_API LIL3D_Model LIL3D_LoadModelFromMesh(LIL3D_Mesh mesh);
LIL3D_API void LIL3D_UnloadModel(LIL3D_Model model, bool unloadMaterials);

LIL3D_API LIL3D_InstanceBuffer LIL3D_LoadInstanceBuffer(int capacity);
LIL3D_API void LIL3D_UnloadInstanceBuffer(LIL3D_InstanceBuffer buffer);
/** Copies 'count' transforms (clamped to capacity). */
LIL3D_API void LIL3D_UpdateInstanceBuffer(LIL3D_InstanceBuffer buffer, const Matrix* transforms, int count);
/** Direct write access to the transforms array (capacity elements). */
LIL3D_API Matrix* LIL3D_MapInstances(LIL3D_InstanceBuffer buffer);

/* ========================================================================== */
/* Draw (valid between LIL3D_Begin* and LIL3D_End)                            */
/* ========================================================================== */

LIL3D_API void LIL3D_DrawMesh(LIL3D_Mesh mesh, LIL3D_Material material, Vector3 position, float scale);
LIL3D_API void LIL3D_DrawMeshEx(LIL3D_Mesh mesh, LIL3D_Material material, Vector3 position, Quaternion rotation, Vector3 scale);
LIL3D_API void LIL3D_DrawMeshPro(LIL3D_Mesh mesh, LIL3D_Material material, Matrix transform);

/** The instance buffer must stay valid and unchanged until LIL3D_End(). */
LIL3D_API void LIL3D_DrawMeshInstanced(LIL3D_Mesh mesh, LIL3D_Material material, LIL3D_InstanceBuffer instances, int count);
LIL3D_API void LIL3D_DrawMeshInstancedEx(LIL3D_Mesh mesh, LIL3D_Material material, LIL3D_InstanceBuffer instances, int count, Matrix transform);

LIL3D_API void LIL3D_DrawModel(LIL3D_Model model, Vector3 position, float scale);
LIL3D_API void LIL3D_DrawModelEx(LIL3D_Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale);
LIL3D_API void LIL3D_DrawModelPro(LIL3D_Model model, Matrix transform);
LIL3D_API void LIL3D_DrawModelInstanced(LIL3D_Model model, LIL3D_InstanceBuffer instances, int count);
LIL3D_API void LIL3D_DrawModelInstancedEx(LIL3D_Model model, LIL3D_InstanceBuffer instances, int count, Matrix transform);

/** Skinned draws (valid between LIL3D_Begin* and LIL3D_End). The player must have been
 *  created for this model. Skinning is done on the CPU (portable to GLES2/WebGL1),
 *  so each player owns its own vertex buffers and two players may share a model. */
LIL3D_API void LIL3D_DrawAnimatedModel(LIL3D_Model model, LIL3D_AnimationPlayer* player, Vector3 position, float scale);
LIL3D_API void LIL3D_DrawAnimatedModelEx(LIL3D_Model model, LIL3D_AnimationPlayer* player, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale);
LIL3D_API void LIL3D_DrawAnimatedModelPro(LIL3D_Model model, LIL3D_AnimationPlayer* player, Matrix transform);

/* ========================================================================== */
/* Animation                                                                  */
/* ========================================================================== */

/** Loads every animation of a file (glTF/GLB, IQM, M3D), which may be the model file
 *  itself. The FPS of each animation is retrieved from the asset:
 *    - IQM:       the per-animation 'framerate' stored in the file.
 *    - glTF, M3D: raylib resamples these at a fixed 17 ms step (~58.8 FPS), so that
 *                 step is used (LIL3D_ANIM_RESAMPLE_FPS). Not 60!
 *  Never hardcode an FPS yourself: use LIL3D_Animation.fps / the player. */
LIL3D_API LIL3D_AnimationLib LIL3D_LoadAnimationLib(const char* filePath);
LIL3D_API void LIL3D_UnloadAnimationLib(LIL3D_AnimationLib lib);
LIL3D_API bool LIL3D_IsAnimationLibValid(LIL3D_AnimationLib lib);
/** Index of the animation called 'name', or -1. */
LIL3D_API int LIL3D_GetAnimationIndex(LIL3D_AnimationLib lib, const char* name);
/** NULL when the index is out of range. */
LIL3D_API const LIL3D_Animation* LIL3D_GetAnimation(LIL3D_AnimationLib lib, int index);

/** Index of a bone by name, or -1. */
LIL3D_API int LIL3D_GetBoneIndex(LIL3D_Skeleton skeleton, const char* name);

/** The player borrows the model's skeleton and the lib: keep both alive while it exists.
 *  Starts in bind pose with every state stopped (weight 0, loop on, speed 1). */
LIL3D_API LIL3D_AnimationPlayer LIL3D_LoadAnimationPlayer(LIL3D_Model model, LIL3D_AnimationLib lib);
LIL3D_API void LIL3D_UnloadAnimationPlayer(LIL3D_AnimationPlayer player);
LIL3D_API bool LIL3D_IsAnimationPlayerValid(LIL3D_AnimationPlayer player);

/** Advances time by dt seconds (scaled by speeds), blends the states, computes the pose. */
LIL3D_API void LIL3D_UpdateAnimationPlayer(LIL3D_AnimationPlayer* player, float dt);

/** Exclusive play: the animation starts from 0 at weight 1, all others stop. */
LIL3D_API void LIL3D_PlayAnimation(LIL3D_AnimationPlayer* player, int animation, bool loop);
LIL3D_API void LIL3D_PlayAnimationByName(LIL3D_AnimationPlayer* player, const char* name, bool loop);
/** Smooth transition: the animation fades in while the others fade out over 'seconds'.
 *  Does nothing if it is already the (only) target. */
LIL3D_API void LIL3D_CrossfadeAnimation(LIL3D_AnimationPlayer* player, int animation, bool loop, float seconds);
LIL3D_API void LIL3D_StopAnimations(LIL3D_AnimationPlayer* player);
/** False for a finished one-shot (or a stopped state). */
LIL3D_API bool LIL3D_IsAnimationPlaying(const LIL3D_AnimationPlayer* player, int animation);
/** Model-space matrix of a bone in the current pose (identity if out of range). */
LIL3D_API Matrix LIL3D_GetBoneMatrix(const LIL3D_AnimationPlayer* player, int bone);

/* ========================================================================== */
/* Debug                                                                      */
/* ========================================================================== */

/** Draws the light's area of influence as wires (omni: sphere, spot: cone, dir: arrows),
 *  tinted with the light colour (faded when inactive). Like r3d, call it AFTER
 *  LIL3D_End(), inside BeginMode3D()/EndMode3D(): the scene depth buffer is kept. */
LIL3D_API void LIL3D_DrawLightShape(LIL3D_Light light);

/* ========================================================================== */
/* Cubemaps / sky                                                             */
/* ========================================================================== */

LIL3D_API LIL3D_Cubemap LIL3D_LoadCubemap(const char* fileName, LIL3D_CubemapLayout layout);
LIL3D_API LIL3D_Cubemap LIL3D_LoadCubemapFromImage(Image image, LIL3D_CubemapLayout layout);
/** Procedural 3-colour vertical gradient sky (no assets needed). */
LIL3D_API LIL3D_Cubemap LIL3D_GenGradientSky(int size, Color top, Color horizon, Color bottom);
LIL3D_API void LIL3D_UnloadCubemap(LIL3D_Cubemap cubemap);

#ifdef __cplusplus
}
#endif

#endif /* LIL3D_H */

/* ========================================================================== */
/* ========================================================================== */
/*                              IMPLEMENTATION                                */
/* ========================================================================== */
/* ========================================================================== */

#ifdef LIL3D_IMPLEMENTATION

#include "rlgl.h"
#include "raymath.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LIL3D_ANIM_RESAMPLE_FPS
/* raylib samples glTF and M3D animations every 17 ms (GLTF_ANIMDELAY / M3D_ANIMDELAY).
 * NOTE: verified for 5.5; if 6.0 resamples differently, override LIL3D_ANIM_RESAMPLE_FPS. */
#   define LIL3D_ANIM_RESAMPLE_FPS (1000.0f/17.0f)
#endif

#define LIL3D_LOG_TAG       "LIL3D: "
#define LIL3D_SHADOW_SLOTS  4       /* sampler slots baked into the lit shader */
#define LIL3D_MIN(a, b)     (((a) < (b)) ? (a) : (b))
#define LIL3D_MAX(a, b)     (((a) > (b)) ? (a) : (b))

/* -------------------------------------------------------------------------- */
/* Internal types                                                             */
/* -------------------------------------------------------------------------- */

typedef struct lil3d_light {
    bool exists, active, shadow;
    LIL3D_LightType type;
    Vector3 position, direction;
    Color color;
    float energy, specular, range, attenuation, innerCutOff, outerCutOff;
    float shadowBias, shadowSoftness;
    RenderTexture2D shadowMap;          /* id == 0 -> not allocated */
    int shadowMapSize;
} lil3d_light;

typedef struct lil3d_lit_program {
    Shader shader;
    Material material;                  /* raylib Material used to feed DrawMesh */
    int locViewPos, locAmbient, locEnv, locMat, locEmission, locLightCount;
    int locLightPos, locLightDir, locLightColor, locLightParams, locLightShadow;
    int locShadowMat[LIL3D_SHADOW_SLOTS];
    int locInstanceBase, locUV;
    float lastMat[4], lastEmission[4], lastUV[4];
} lil3d_lit_program;

typedef struct lil3d_depth_program {
    Shader shader;
    Material material;
    int locLight, locAlpha, locInstanceBase, locUV;
    float lastAlpha, lastUV[4];
} lil3d_depth_program;

typedef struct lil3d_sky_program {
    Shader shader;
    Material material;
    int locEnergy;
} lil3d_sky_program;

typedef struct lil3d_drawcall {
    LIL3D_Mesh mesh;
    LIL3D_Material material;
    Matrix transform;
    const Matrix* instances;            /* NULL -> single draw */
    int instanceCount;
    BoundingBox bounds;                 /* world AABB, valid if hasBounds */
    bool hasBounds;
    float sortKey;                      /* squared distance to camera */
} lil3d_drawcall;

typedef struct lil3d_frame {
    int lightCount, shadowCount;
    lil3d_light* lights[LIL3D_MAX_LIGHTS];
    Vector4 pos[LIL3D_MAX_LIGHTS];
    Vector4 dir[LIL3D_MAX_LIGHTS];
    Vector4 color[LIL3D_MAX_LIGHTS];
    Vector4 params[LIL3D_MAX_LIGHTS];
    Vector4 shadow[LIL3D_MAX_LIGHTS];
    Matrix shadowVP[LIL3D_MAX_SHADOWS];
    bool shadowValid[LIL3D_MAX_SHADOWS];
    Vector4 ambient;
} lil3d_frame;

typedef struct lil3d_context {
    bool initialized, inFrame, hasTarget, hwInstancing;
    int glVersion;
    LIL3D_Environment env;

    lil3d_light* lights;  int lightCap;
    lil3d_light** candidates;  int candidateCap;

    lil3d_lit_program lit, litInst;
    lil3d_depth_program depth, depthInst;
    lil3d_sky_program sky;
    Mesh skyCube;

    Camera3D camera;
    RenderTexture2D target;

    lil3d_drawcall* draws;  int drawCount, drawCap;
    lil3d_drawcall** sortList;  int sortCap;

    int curBlend, curCull;              /* cached GL state during End() */
} lil3d_context;

static lil3d_context lil3d_ctx;
static int lil3d_hints[LIL3D_HINT_COUNT] = { 1024, 512, 1 };

/* -------------------------------------------------------------------------- */
/* Small helpers                                                              */
/* -------------------------------------------------------------------------- */

static void* lil3d_grow_array(void* ptr, int* cap, int needed, size_t elem)
{
    if (needed <= *cap) return ptr;
    int newCap = (*cap > 0) ? *cap : 8;
    while (newCap < needed) newCap *= 2;
    void* p = realloc(ptr, (size_t)newCap*elem);
    if (p == NULL) return NULL;
    memset((char*)p + (size_t)(*cap)*elem, 0, (size_t)(newCap - *cap)*elem);
    *cap = newCap;
    return p;
}

static Vector3 lil3d_safe_normalize(Vector3 v, Vector3 fallback)
{
    float len = Vector3Length(v);
    return (len < 1e-6f) ? fallback : Vector3Scale(v, 1.0f/len);
}

static float lil3d_to_linear(unsigned char c)
{
    return powf((float)c/255.0f, 2.2f);
}

static Matrix lil3d_trs(Transform t)
{
    return MatrixMultiply(MatrixMultiply(MatrixScale(t.scale.x, t.scale.y, t.scale.z), QuaternionToMatrix(t.rotation)),
                          MatrixTranslate(t.translation.x, t.translation.y, t.translation.z));
}

static Texture2D lil3d_white_texture(void)
{
    Texture2D t = { rlGetTextureIdDefault(), 1, 1, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
    return t;
}

static BoundingBox lil3d_transform_aabb(BoundingBox b, Matrix m)
{
    Vector3 mn = { FLT_MAX, FLT_MAX, FLT_MAX };
    Vector3 mx = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
    for (int i = 0; i < 8; i++)
    {
        Vector3 c = {
            (i & 1) ? b.max.x : b.min.x,
            (i & 2) ? b.max.y : b.min.y,
            (i & 4) ? b.max.z : b.min.z
        };
        c = Vector3Transform(c, m);
        mn = Vector3Min(mn, c);
        mx = Vector3Max(mx, c);
    }
    BoundingBox out = { mn, mx };
    return out;
}

static bool lil3d_aabb_sphere(BoundingBox b, Vector3 c, float r)
{
    float d = 0.0f;
    if (c.x < b.min.x) d += (b.min.x - c.x)*(b.min.x - c.x); else if (c.x > b.max.x) d += (c.x - b.max.x)*(c.x - b.max.x);
    if (c.y < b.min.y) d += (b.min.y - c.y)*(b.min.y - c.y); else if (c.y > b.max.y) d += (c.y - b.max.y)*(c.y - b.max.y);
    if (c.z < b.min.z) d += (b.min.z - c.z)*(b.min.z - c.z); else if (c.z > b.max.z) d += (c.z - b.max.z)*(c.z - b.max.z);
    return d <= r*r;
}

/* Cube face basis shared by the omni shadow renderer (C) and sampler (GLSL).
 * Order: +X, -X, +Y, -Y, +Z, -Z. Atlas cell = (face % 3, face / 3). */
static const Vector3 lil3d_face_fwd[6] = {
    {  1, 0, 0 }, { -1, 0, 0 }, { 0,  1, 0 }, { 0, -1, 0 }, { 0, 0,  1 }, { 0, 0, -1 }
};
static const Vector3 lil3d_face_up[6] = {
    { 0, 1, 0 }, { 0, 1, 0 }, { 0, 0, -1 }, { 0, 0, 1 }, { 0, 1, 0 }, { 0, 1, 0 }
};

/* -------------------------------------------------------------------------- */
/* Embedded shaders                                                           */
/*                                                                            */
/* Authored in the GLSL ES 1.00 subset (attribute/varying/texture2D/          */
/* gl_FragColor, float literals only, no dynamic uniform indexing inside      */
/* functions). A per-GL-flavour header maps it to 330 / 300 es / 100 / 120.   */
/* The bodies are stringified by the preprocessor, so they must not contain   */
/* '#' directives; knobs come from #defines in the generated prefix.          */
/* -------------------------------------------------------------------------- */

#define LIL3D_GLSL(...) #__VA_ARGS__

/* ---- lit: vertex ---- */
static const char* const lil3d_vs_lit_common = LIL3D_GLSL(
    attribute vec3 vertexPosition;
    attribute vec2 vertexTexCoord;
    attribute vec3 vertexNormal;
    uniform mat4 mvp;
    uniform vec4 uUV;           // xy offset, zw scale
    varying vec3 vWorldPos;
    varying vec3 vNormal;
    varying vec2 vUV;
);

static const char* const lil3d_vs_lit_plain = LIL3D_GLSL(
    uniform mat4 matModel;
    uniform mat4 matNormal;
    void main()
    {
        vec4 wp = matModel*vec4(vertexPosition, 1.0);
        vWorldPos = wp.xyz;
        vNormal = vec3(matNormal*vec4(vertexNormal, 0.0));
        vUV = vertexTexCoord*uUV.zw + uUV.xy;
        gl_Position = mvp*vec4(vertexPosition, 1.0);
    }
);

/* Instanced: raylib's DrawMeshInstanced() feeds 'instanceTransform' per instance
 * and 'mvp' = view*projection (draw-level model matrix is identity). */
static const char* const lil3d_vs_lit_inst = LIL3D_GLSL(
    attribute mat4 instanceTransform;
    uniform mat4 uInstanceBase;
    void main()
    {
        mat4 m = uInstanceBase*instanceTransform;
        vec4 wp = m*vec4(vertexPosition, 1.0);
        vWorldPos = wp.xyz;
        vNormal = vec3(m*vec4(vertexNormal, 0.0));
        vUV = vertexTexCoord*uUV.zw + uUV.xy;
        gl_Position = mvp*wp;
    }
);

/* ---- lit: fragment (metallic-roughness PBR, up to LIL3D_MAX_LIGHTS, 4 shadow slots) ----
 * GGX distribution, height-correlated Smith visibility (Hammon approximation), Schlick
 * Fresnel. Light 'energy' is artist-friendly: a white energy-1 light facing a white
 * diffuse surface gives white (the usual 1/pi is folded into the light). Ambient: diffuse
 * from the environment ambient colour, specular from the sky cubemap (blended towards the
 * ambient colour as roughness rises) with Karis' analytic environment BRDF. */
static const char* const lil3d_fs_lit = LIL3D_GLSL(
    varying vec3 vWorldPos;
    varying vec3 vNormal;
    varying vec2 vUV;

    uniform sampler2D texture0;
    uniform sampler2D uORM;     // R occlusion, G roughness, B metalness
    uniform samplerCube uSky;
    uniform vec4 colDiffuse;
    uniform vec4 uMat;          // x metalness, y roughness, z alphaCutoff, w occlusion strength
    uniform vec4 uEmission;     // rgb (linear, premultiplied by energy)
    uniform vec4 uAmbient;      // rgb (linear, premultiplied by energy)
    uniform vec4 uEnv;          // x sky available (0/1), y sky energy
    uniform vec3 uViewPos;
    uniform int uLightCount;
    uniform vec4 uLightPos[LIL3D_MAX_LIGHTS];     // xyz position, w type (0 dir, 1 spot, 2 omni)
    uniform vec4 uLightDir[LIL3D_MAX_LIGHTS];     // xyz direction of light travel
    uniform vec4 uLightColor[LIL3D_MAX_LIGHTS];   // rgb linear*energy, w specular
    uniform vec4 uLightParams[LIL3D_MAX_LIGHTS];  // x cosInner, y cosOuter, z attenuation, w range
    uniform vec4 uLightShadow[LIL3D_MAX_LIGHTS];  // x enabled, y bias, z normal offset, w texel step
    uniform sampler2D uShadow0;
    uniform sampler2D uShadow1;
    uniform sampler2D uShadow2;
    uniform sampler2D uShadow3;
    uniform mat4 uShadowMat0;
    uniform mat4 uShadowMat1;
    uniform mat4 uShadowMat2;
    uniform mat4 uShadowMat3;

    // Depth is stored packed in RGBA8 (works without depth-texture extensions).
    float lil3d_unpack(vec4 c)
    {
        return dot(c, vec4(1.0/16777216.0, 1.0/65536.0, 1.0/256.0, 1.0)*(255.0/256.0));
    }

    float lil3d_pcf(sampler2D m, vec2 uv, vec2 lo, vec2 hi, float depth, vec2 texel)
    {
        float sum = 0.0;
        for (int x = -LIL3D_PCF; x <= LIL3D_PCF; x++)
        {
            for (int y = -LIL3D_PCF; y <= LIL3D_PCF; y++)
            {
                vec2 p = clamp(uv + vec2(float(x), float(y))*texel, lo, hi);
                sum += step(depth, lil3d_unpack(texture2D(m, p)));
            }
        }
        return sum/float((2*LIL3D_PCF + 1)*(2*LIL3D_PCF + 1));
    }

    // Omni shadows live in a 3x2 atlas (face = +X,-X,+Y,-Y,+Z,-Z). The face basis
    // here MUST match lil3d_face_fwd / lil3d_face_up on the C side.
    float lil3d_omni(sampler2D m, vec3 d, float invFar, float bias, float texelStep)
    {
        vec3 ad = abs(d);
        vec3 f = vec3(0.0, 0.0, 1.0);
        vec3 u = vec3(0.0, 1.0, 0.0);
        float face = 4.0;
        if (ad.x >= ad.y && ad.x >= ad.z)
        {
            if (d.x >= 0.0) { f = vec3(1.0, 0.0, 0.0); face = 0.0; }
            else { f = vec3(-1.0, 0.0, 0.0); face = 1.0; }
        }
        else if (ad.y >= ad.z)
        {
            if (d.y >= 0.0) { f = vec3(0.0, 1.0, 0.0); u = vec3(0.0, 0.0, -1.0); face = 2.0; }
            else { f = vec3(0.0, -1.0, 0.0); u = vec3(0.0, 0.0, 1.0); face = 3.0; }
        }
        else
        {
            if (d.z >= 0.0) { f = vec3(0.0, 0.0, 1.0); face = 4.0; }
            else { f = vec3(0.0, 0.0, -1.0); face = 5.0; }
        }
        vec3 r = cross(f, u);
        float z = max(dot(d, f), 0.0001);
        vec2 ndc = vec2(dot(d, r), dot(d, u))/z;
        vec2 cell = vec2(mod(face, 3.0), floor(face/3.0));
        vec2 inv = vec2(1.0/3.0, 1.0/2.0);
        vec2 uv = (cell + ndc*0.5 + 0.5)*inv;
        vec2 texel = vec2(texelStep/3.0, texelStep/2.0);
        vec2 lo = cell*inv + texel*0.5;
        vec2 hi = (cell + 1.0)*inv - texel*0.5;
        float depth = length(d)*invFar - bias;
        return lil3d_pcf(m, uv, lo, hi, depth, texel);
    }

    float lil3d_shadow(sampler2D m, mat4 vp, vec4 lp, float range, vec3 p, float bias, float texelStep)
    {
        if (lp.w > 1.5) return lil3d_omni(m, p - lp.xyz, 1.0/range, bias, texelStep);
        vec4 c = vp*vec4(p, 1.0);
        vec3 n = c.xyz/c.w;
        vec2 uv = n.xy*0.5 + 0.5;
        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || n.z > 1.0) return 1.0;
        float depth = (lp.w < 0.5) ? (n.z*0.5 + 0.5) : (length(p - lp.xyz)/range);
        return lil3d_pcf(m, uv, vec2(0.0), vec2(1.0), depth - bias, vec2(texelStep));
    }

    float lil3d_ggx(float ndh, float a2)
    {
        float d = ndh*ndh*(a2 - 1.0) + 1.0;
        return a2/(3.14159265*d*d);
    }

    float lil3d_vis(float ndl, float ndv, float alpha)
    {
        return 0.5/max(mix(2.0*ndl*ndv, ndl + ndv, alpha), 0.0001);
    }

    void main()
    {
        vec4 albedo = texture2D(texture0, vUV)*colDiffuse;
        if (albedo.a < uMat.z) discard;

        vec3 orm = texture2D(uORM, vUV).rgb;
        float ao = mix(1.0, orm.r, uMat.w);
        float rough = clamp(orm.g*uMat.y, 0.06, 1.0);
        float metal = clamp(orm.b*uMat.x, 0.0, 1.0);
        float alpha = rough*rough;
        float a2 = alpha*alpha;

        vec3 base = pow(albedo.rgb, vec3(2.2));
        vec3 F0 = mix(vec3(0.04), base, metal);
        vec3 diffuse = base*(1.0 - metal);

        vec3 N = normalize(vNormal);
        if (!gl_FrontFacing) N = -N;
        vec3 P = vWorldPos;
        vec3 V = normalize(uViewPos - P);
        float ndv = max(dot(N, V), 0.0001);

        // Ambient: diffuse irradiance + specular reflection of the sky
        vec3 env = uAmbient.rgb;
        if (uEnv.x > 0.5)
        {
            vec3 sky = pow(textureCube(uSky, reflect(-V, N)).rgb, vec3(2.2))*uEnv.y;
            env = mix(sky, uAmbient.rgb, rough);
        }
        vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
        vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
        vec4 r = rough*c0 + c1;
        float a004 = min(r.x*r.x, exp2(-9.28*ndv))*r.x + r.y;
        vec2 ab = vec2(-1.04, 1.04)*a004 + r.zw;
        vec3 result = (uAmbient.rgb*diffuse + env*(F0*ab.x + ab.y))*ao + uEmission.rgb;

        for (int i = 0; i < LIL3D_MAX_LIGHTS; i++)
        {
            if (i >= uLightCount) break;

            vec4 lp = uLightPos[i];
            vec4 ld = uLightDir[i];
            vec4 lc = uLightColor[i];
            vec4 lq = uLightParams[i];
            vec4 ls = uLightShadow[i];

            vec3 L = -ld.xyz;
            float att = 1.0;
            float dist = 0.0;
            if (lp.w > 0.5)
            {
                vec3 toLight = lp.xyz - P;
                dist = length(toLight);
                L = toLight/max(dist, 0.0001);
                att = pow(clamp(1.0 - dist/lq.w, 0.0, 1.0), lq.z);
                if (lp.w < 1.5) att *= smoothstep(lq.y, lq.x, dot(-L, ld.xyz));
            }

            float ndl = dot(N, L);
            if (ndl > 0.0 && att > 0.0)
            {
                float vis = 1.0;
                if (ls.x > 0.5)
                {
                    float off = (lp.w < 0.5) ? ls.z : ls.z*dist;
                    vec3 po = P + N*off;
                    float bias = ls.y*(1.0 + 2.0*(1.0 - ndl));
                    if (i == 0) vis = lil3d_shadow(uShadow0, uShadowMat0, lp, lq.w, po, bias, ls.w);
                    else if (i == 1) vis = lil3d_shadow(uShadow1, uShadowMat1, lp, lq.w, po, bias, ls.w);
                    else if (i == 2) vis = lil3d_shadow(uShadow2, uShadowMat2, lp, lq.w, po, bias, ls.w);
                    else if (i == 3) vis = lil3d_shadow(uShadow3, uShadowMat3, lp, lq.w, po, bias, ls.w);
                }

                vec3 H = normalize(L + V);
                float ndh = max(dot(N, H), 0.0);
                float f = 1.0 - max(dot(V, H), 0.0);
                vec3 F = F0 + (1.0 - F0)*(f*f*f*f*f);
                vec3 spec = min(lil3d_ggx(ndh, a2)*lil3d_vis(ndl, ndv, alpha)*3.14159265*F*lc.w, vec3(20.0));
                result += (diffuse + spec)*ndl*lc.rgb*att*vis;
            }
        }

        result = pow(max(result, vec3(0.0)), vec3(1.0/2.2));
        gl_FragColor = vec4(result, albedo.a);
    }
);

/* ---- depth (shadow caster) ---- */
static const char* const lil3d_vs_depth_common = LIL3D_GLSL(
    attribute vec3 vertexPosition;
    attribute vec2 vertexTexCoord;
    uniform mat4 mvp;
    uniform vec4 uUV;           // xy offset, zw scale
    varying vec3 vWorldPos;
    varying vec2 vUV;
);

static const char* const lil3d_vs_depth_plain = LIL3D_GLSL(
    uniform mat4 matModel;
    void main()
    {
        vec4 wp = matModel*vec4(vertexPosition, 1.0);
        vWorldPos = wp.xyz;
        vUV = vertexTexCoord*uUV.zw + uUV.xy;
        gl_Position = mvp*vec4(vertexPosition, 1.0);
    }
);

static const char* const lil3d_vs_depth_inst = LIL3D_GLSL(
    attribute mat4 instanceTransform;
    uniform mat4 uInstanceBase;
    void main()
    {
        vec4 wp = uInstanceBase*instanceTransform*vec4(vertexPosition, 1.0);
        vWorldPos = wp.xyz;
        vUV = vertexTexCoord*uUV.zw + uUV.xy;
        gl_Position = mvp*wp;
    }
);

static const char* const lil3d_fs_depth = LIL3D_GLSL(
    varying vec3 vWorldPos;
    varying vec2 vUV;
    uniform sampler2D texture0;
    uniform vec4 colDiffuse;
    uniform float uAlphaCutoff;
    uniform vec4 uShadowLight;      // xyz light position, w = 1/range (0 -> directional)

    vec4 lil3d_pack(float v)
    {
        vec4 r = vec4(fract(v*vec3(16777216.0, 65536.0, 256.0)), v);
        r.yzw -= r.xyz*(1.0/256.0);
        return r*(256.0/255.0);
    }

    void main()
    {
        if (uAlphaCutoff > 0.0)
        {
            if (texture2D(texture0, vUV).a*colDiffuse.a < uAlphaCutoff) discard;
        }
        float d = gl_FragCoord.z;
        if (uShadowLight.w > 0.0) d = length(vWorldPos - uShadowLight.xyz)*uShadowLight.w;
        gl_FragColor = lil3d_pack(clamp(d, 0.0, 1.0));
    }
);

/* ---- sky ---- */
static const char* const lil3d_vs_sky = LIL3D_GLSL(
    attribute vec3 vertexPosition;
    uniform mat4 matView;
    uniform mat4 matProjection;
    varying vec3 vDir;
    void main()
    {
        vDir = vertexPosition;
        mat4 v = matView;
        v[3] = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 p = matProjection*v*vec4(vertexPosition, 1.0);
        gl_Position = p.xyww;
    }
);

static const char* const lil3d_fs_sky = LIL3D_GLSL(
    varying vec3 vDir;
    uniform samplerCube uSky;
    uniform float uSkyEnergy;
    void main()
    {
        gl_FragColor = vec4(textureCube(uSky, vDir).rgb*uSkyEnergy, 1.0);
    }
);

/* -------------------------------------------------------------------------- */
/* Shader building                                                            */
/* -------------------------------------------------------------------------- */

static const char* lil3d_glsl_header(bool fragment)
{
    switch (lil3d_ctx.glVersion)
    {
        case RL_OPENGL_33:
        case RL_OPENGL_43:
            return fragment
                ? "#version 330\n"
                  "#define varying in\n"
                  "#define texture2D texture\n"
                  "#define textureCube texture\n"
                  "out vec4 lil3d_FragColor;\n"
                  "#define gl_FragColor lil3d_FragColor\n"
                : "#version 330\n"
                  "#define attribute in\n"
                  "#define varying out\n";

        case RL_OPENGL_ES_30:
            return fragment
                ? "#version 300 es\n"
                  "precision highp float;\n"
                  "precision highp int;\n"
                  "precision highp sampler2D;\n"
                  "precision highp samplerCube;\n"
                  "#define varying in\n"
                  "#define texture2D texture\n"
                  "#define textureCube texture\n"
                  "out vec4 lil3d_FragColor;\n"
                  "#define gl_FragColor lil3d_FragColor\n"
                : "#version 300 es\n"
                  "precision highp float;\n"
                  "#define attribute in\n"
                  "#define varying out\n";

        case RL_OPENGL_ES_20:
            return fragment
                ? "#version 100\n"
                  "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
                  "precision highp float;\n"
                  "precision highp sampler2D;\n"
                  "precision highp samplerCube;\n"
                  "#else\n"
                  "precision mediump float;\n"
                  "#endif\n"
                : "#version 100\n"
                  "precision highp float;\n";

        case RL_OPENGL_21:
            return "#version 120\n";

        default:
            return NULL;
    }
}

/* header + defs + (part0, part1), with a newline after every ';', '{' and '}'
 * so that driver error messages carry useful line numbers. */
static char* lil3d_make_source(bool fragment, const char* defs, const char* part0, const char* part1)
{
    const char* header = lil3d_glsl_header(fragment);
    if (header == NULL) return NULL;

    size_t cap = strlen(header) + strlen(defs) + 2*(strlen(part0) + (part1 ? strlen(part1) : 0)) + 16;
    char* out = (char*)malloc(cap);
    if (out == NULL) return NULL;

    char* w = out;
    size_t n = strlen(header); memcpy(w, header, n); w += n;
    n = strlen(defs); memcpy(w, defs, n); w += n;

    const char* parts[2] = { part0, part1 };
    for (int k = 0; k < 2; k++)
    {
        const char* s = parts[k];
        if (s == NULL) continue;
        for (; *s; s++)
        {
            *w++ = *s;
            if (*s == ';' || *s == '{' || *s == '}') *w++ = '\n';
        }
        *w++ = '\n';
    }
    *w = '\0';
    return out;
}

static Shader lil3d_compile(const char* name, const char* vs0, const char* vs1, const char* fs0, const char* defs)
{
    Shader out = { 0 };
    char* vs = lil3d_make_source(false, defs, vs0, vs1);
    char* fs = lil3d_make_source(true, defs, fs0, NULL);

    if (vs != NULL && fs != NULL)
    {
#ifdef LIL3D_DEBUG_SHADERS
        TraceLog(LOG_INFO, LIL3D_LOG_TAG "---- %s (vertex) ----\n%s", name, vs);
        TraceLog(LOG_INFO, LIL3D_LOG_TAG "---- %s (fragment) ----\n%s", name, fs);
#endif
        Shader s = LoadShaderFromMemory(vs, fs);
        if (s.id != 0 && s.id != rlGetShaderIdDefault()) out = s;
        else TraceLog(LOG_ERROR, LIL3D_LOG_TAG "failed to build the '%s' shader", name);
    }
    else TraceLog(LOG_ERROR, LIL3D_LOG_TAG "unsupported OpenGL flavour (rlGetVersion() = %d)", lil3d_ctx.glVersion);

    free(vs);
    free(fs);
    return out;
}

static Material lil3d_make_material(Shader shader)
{
    Material m = LoadMaterialDefault();     /* allocates the maps array */
    m.shader = shader;
    return m;
}

static bool lil3d_load_lit(lil3d_lit_program* p, bool instanced, const char* defs)
{
    memset(p, 0, sizeof(*p));
    p->shader = lil3d_compile(instanced ? "lit/instanced" : "lit",
        lil3d_vs_lit_common, instanced ? lil3d_vs_lit_inst : lil3d_vs_lit_plain, lil3d_fs_lit, defs);
    if (p->shader.id == 0) return false;

    Shader* s = &p->shader;
    p->locViewPos      = GetShaderLocation(*s, "uViewPos");
    p->locAmbient      = GetShaderLocation(*s, "uAmbient");
    p->locEnv          = GetShaderLocation(*s, "uEnv");
    p->locMat          = GetShaderLocation(*s, "uMat");
    p->locEmission     = GetShaderLocation(*s, "uEmission");
    p->locUV           = GetShaderLocation(*s, "uUV");
    p->locLightCount   = GetShaderLocation(*s, "uLightCount");
    p->locLightPos     = GetShaderLocation(*s, "uLightPos");
    p->locLightDir     = GetShaderLocation(*s, "uLightDir");
    p->locLightColor   = GetShaderLocation(*s, "uLightColor");
    p->locLightParams  = GetShaderLocation(*s, "uLightParams");
    p->locLightShadow  = GetShaderLocation(*s, "uLightShadow");
    p->locShadowMat[0] = GetShaderLocation(*s, "uShadowMat0");
    p->locShadowMat[1] = GetShaderLocation(*s, "uShadowMat1");
    p->locShadowMat[2] = GetShaderLocation(*s, "uShadowMat2");
    p->locShadowMat[3] = GetShaderLocation(*s, "uShadowMat3");
    p->locInstanceBase = instanced ? GetShaderLocation(*s, "uInstanceBase") : -1;

    /* Shadow maps ride on the material "maps" array so DrawMesh binds/unbinds them
     * (slots 1..4 are plain 2D maps: METALNESS, NORMAL, ROUGHNESS, OCCLUSION). */
    s->locs[SHADER_LOC_MAP_ALBEDO]    = GetShaderLocation(*s, "texture0");
    s->locs[SHADER_LOC_MAP_METALNESS] = GetShaderLocation(*s, "uShadow0");
    s->locs[SHADER_LOC_MAP_NORMAL]    = GetShaderLocation(*s, "uShadow1");
    s->locs[SHADER_LOC_MAP_ROUGHNESS] = GetShaderLocation(*s, "uShadow2");
    s->locs[SHADER_LOC_MAP_OCCLUSION] = GetShaderLocation(*s, "uShadow3");
    s->locs[SHADER_LOC_MAP_EMISSION]  = GetShaderLocation(*s, "uORM");          /* unit 5 */
    s->locs[SHADER_LOC_MAP_CUBEMAP]   = GetShaderLocation(*s, "uSky");          /* unit 7 */
    s->locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(*s, "colDiffuse");
    {
        /* Never leave a samplerCube on unit 0 (it holds a 2D texture): pin the units. */
        int unitOrm = MATERIAL_MAP_EMISSION, unitSky = MATERIAL_MAP_CUBEMAP;
        SetShaderValue(*s, s->locs[SHADER_LOC_MAP_EMISSION], &unitOrm, SHADER_UNIFORM_INT);
        SetShaderValue(*s, s->locs[SHADER_LOC_MAP_CUBEMAP], &unitSky, SHADER_UNIFORM_INT);
    }
    if (instanced)
    {
        s->locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(*s, "instanceTransform");
        if (s->locs[SHADER_LOC_MATRIX_MODEL] < 0) return false;
    }
    else
    {
        s->locs[SHADER_LOC_MATRIX_MODEL]  = GetShaderLocation(*s, "matModel");
        s->locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(*s, "matNormal");
    }

    p->material = lil3d_make_material(*s);
    for (int i = 0; i < 4; i++) { p->lastMat[i] = -1.0f; p->lastEmission[i] = -1.0f; p->lastUV[i] = -1e30f; }
    return true;
}

static bool lil3d_load_depth(lil3d_depth_program* p, bool instanced, const char* defs)
{
    memset(p, 0, sizeof(*p));
    p->shader = lil3d_compile(instanced ? "depth/instanced" : "depth",
        lil3d_vs_depth_common, instanced ? lil3d_vs_depth_inst : lil3d_vs_depth_plain, lil3d_fs_depth, defs);
    if (p->shader.id == 0) return false;

    Shader* s = &p->shader;
    p->locLight        = GetShaderLocation(*s, "uShadowLight");
    p->locAlpha        = GetShaderLocation(*s, "uAlphaCutoff");
    p->locUV           = GetShaderLocation(*s, "uUV");
    p->locInstanceBase = instanced ? GetShaderLocation(*s, "uInstanceBase") : -1;

    s->locs[SHADER_LOC_MAP_ALBEDO]    = GetShaderLocation(*s, "texture0");
    s->locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(*s, "colDiffuse");
    if (instanced)
    {
        s->locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(*s, "instanceTransform");
        if (s->locs[SHADER_LOC_MATRIX_MODEL] < 0) return false;
    }
    else s->locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(*s, "matModel");

    p->material = lil3d_make_material(*s);
    p->lastAlpha = -1.0f;
    for (int i = 0; i < 4; i++) p->lastUV[i] = -1e30f;
    return true;
}

static bool lil3d_load_sky(lil3d_sky_program* p, const char* defs)
{
    memset(p, 0, sizeof(*p));
    p->shader = lil3d_compile("sky", lil3d_vs_sky, NULL, lil3d_fs_sky, defs);
    if (p->shader.id == 0) return false;

    Shader* s = &p->shader;
    p->locEnergy = GetShaderLocation(*s, "uSkyEnergy");
    s->locs[SHADER_LOC_MAP_CUBEMAP]      = GetShaderLocation(*s, "uSky");
    s->locs[SHADER_LOC_MATRIX_VIEW]      = GetShaderLocation(*s, "matView");
    s->locs[SHADER_LOC_MATRIX_PROJECTION] = GetShaderLocation(*s, "matProjection");

    p->material = lil3d_make_material(*s);
    return true;
}

static void lil3d_unload_material_shell(Material* m)
{
    /* Do NOT use UnloadMaterial(): it would also unload the shader/textures. */
    if (m->maps != NULL) { MemFree(m->maps); m->maps = NULL; }
}

static void lil3d_release_programs(void)
{
    lil3d_lit_program* lits[2] = { &lil3d_ctx.lit, &lil3d_ctx.litInst };
    lil3d_depth_program* depths[2] = { &lil3d_ctx.depth, &lil3d_ctx.depthInst };
    for (int i = 0; i < 2; i++)
    {
        lil3d_unload_material_shell(&lits[i]->material);
        if (lits[i]->shader.id != 0) UnloadShader(lits[i]->shader);
        lil3d_unload_material_shell(&depths[i]->material);
        if (depths[i]->shader.id != 0) UnloadShader(depths[i]->shader);
    }
    lil3d_unload_material_shell(&lil3d_ctx.sky.material);
    if (lil3d_ctx.sky.shader.id != 0) UnloadShader(lil3d_ctx.sky.shader);
}

/* -------------------------------------------------------------------------- */
/* Init / Close                                                               */
/* -------------------------------------------------------------------------- */

void LIL3D_SetHint(LIL3D_Hint hint, int value)
{
    if (lil3d_ctx.initialized)
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "LIL3D_SetHint() must be called before LIL3D_Init(); ignored");
        return;
    }
    if (hint < 0 || hint >= LIL3D_HINT_COUNT) return;
    lil3d_hints[hint] = value;
}

bool LIL3D_Init(int resWidth, int resHeight)
{
    (void)resWidth;
    (void)resHeight;

    if (lil3d_ctx.initialized)
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "already initialised");
        return true;
    }
    if (!IsWindowReady())
    {
        TraceLog(LOG_ERROR, LIL3D_LOG_TAG "call InitWindow() before LIL3D_Init()");
        return false;
    }
    int gl = rlGetVersion();
    if (gl == RL_OPENGL_11)
    {
        TraceLog(LOG_ERROR, LIL3D_LOG_TAG "OpenGL 1.1 has no shaders; unsupported");
        return false;
    }

    memset(&lil3d_ctx, 0, sizeof(lil3d_ctx));
    lil3d_ctx.glVersion = gl;
    lil3d_ctx.hwInstancing = (gl == RL_OPENGL_33 || gl == RL_OPENGL_43 || gl == RL_OPENGL_ES_30);

    lil3d_ctx.env.background.color  = (Color){ 24, 28, 36, 255 };
    lil3d_ctx.env.background.energy = 1.0f;
    lil3d_ctx.env.ambient.color     = (Color){ 60, 62, 72, 255 };
    lil3d_ctx.env.ambient.energy    = 1.0f;

    int pcf = lil3d_hints[LIL3D_HINT_SHADOW_FILTER];
    pcf = (pcf < 0) ? 0 : ((pcf > 2) ? 2 : pcf);
    char defs[128];
    snprintf(defs, sizeof(defs), "#define LIL3D_MAX_LIGHTS %d\n#define LIL3D_PCF %d\n", LIL3D_MAX_LIGHTS, pcf);

    bool ok = lil3d_load_lit(&lil3d_ctx.lit, false, defs)
           && lil3d_load_depth(&lil3d_ctx.depth, false, defs)
           && lil3d_load_sky(&lil3d_ctx.sky, defs);

    if (ok && lil3d_ctx.hwInstancing)
    {
        if (!lil3d_load_lit(&lil3d_ctx.litInst, true, defs) || !lil3d_load_depth(&lil3d_ctx.depthInst, true, defs))
        {
            TraceLog(LOG_WARNING, LIL3D_LOG_TAG "instanced shaders failed, falling back to per-instance draws");
            lil3d_ctx.hwInstancing = false;
        }
    }
    if (!ok)
    {
        lil3d_release_programs();
        memset(&lil3d_ctx, 0, sizeof(lil3d_ctx));
        return false;
    }

    lil3d_ctx.skyCube = GenMeshCube(1.0f, 1.0f, 1.0f);
    lil3d_ctx.initialized = true;
    TraceLog(LOG_INFO, LIL3D_LOG_TAG "initialised v" LIL3D_VERSION " (GL flavour %d, hardware instancing: %s)",
        gl, lil3d_ctx.hwInstancing ? "yes" : "no");
    return true;
}

static void lil3d_free_shadow_map(lil3d_light* l)
{
    if (l->shadowMap.id != 0) UnloadRenderTexture(l->shadowMap);
    memset(&l->shadowMap, 0, sizeof(l->shadowMap));
    l->shadowMapSize = 0;
}

void LIL3D_Close(void)
{
    if (!lil3d_ctx.initialized) return;

    for (int i = 0; i < lil3d_ctx.lightCap; i++) lil3d_free_shadow_map(&lil3d_ctx.lights[i]);
    UnloadMesh(lil3d_ctx.skyCube);
    lil3d_release_programs();

    free(lil3d_ctx.lights);
    free(lil3d_ctx.candidates);
    free(lil3d_ctx.draws);
    free(lil3d_ctx.sortList);
    memset(&lil3d_ctx, 0, sizeof(lil3d_ctx));
}

bool LIL3D_HasHardwareInstancing(void) { return lil3d_ctx.hwInstancing; }

LIL3D_Environment* LIL3D_GetEnvironment(void) { return &lil3d_ctx.env; }

/* -------------------------------------------------------------------------- */
/* Lights                                                                     */
/* -------------------------------------------------------------------------- */

static lil3d_light* lil3d_get_light(LIL3D_Light id)
{
    if (id == 0 || (int)id > lil3d_ctx.lightCap) return NULL;
    lil3d_light* l = &lil3d_ctx.lights[id - 1];
    return l->exists ? l : NULL;
}

LIL3D_Light LIL3D_CreateLight(LIL3D_LightType type)
{
    if (!lil3d_ctx.initialized)
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "LIL3D_CreateLight() called before LIL3D_Init()");
        return 0;
    }

    int slot = -1;
    for (int i = 0; i < lil3d_ctx.lightCap; i++)
    {
        if (!lil3d_ctx.lights[i].exists) { slot = i; break; }
    }
    if (slot < 0)
    {
        int old = lil3d_ctx.lightCap;
        lil3d_light* grown = (lil3d_light*)lil3d_grow_array(lil3d_ctx.lights, &lil3d_ctx.lightCap, old + 1, sizeof(lil3d_light));
        if (grown == NULL) return 0;
        lil3d_ctx.lights = grown;
        slot = old;
    }

    lil3d_light* l = &lil3d_ctx.lights[slot];
    memset(l, 0, sizeof(*l));
    l->exists = true;
    l->type = type;
    l->direction = (Vector3){ 0.0f, -1.0f, 0.0f };
    l->color = WHITE;
    l->energy = 1.0f;
    l->specular = 1.0f;
    l->range = 16.0f;
    l->attenuation = 2.0f;
    l->innerCutOff = 22.5f;
    l->outerCutOff = 45.0f;
    l->shadowBias = 0.0005f;
    l->shadowSoftness = 1.0f;
    return (LIL3D_Light)(slot + 1);
}

void LIL3D_DestroyLight(LIL3D_Light light)
{
    lil3d_light* l = lil3d_get_light(light);
    if (l == NULL) return;
    lil3d_free_shadow_map(l);
    memset(l, 0, sizeof(*l));
}

bool LIL3D_IsLightExist(LIL3D_Light light) { return lil3d_get_light(light) != NULL; }

LIL3D_LightType LIL3D_GetLightType(LIL3D_Light light)
{
    lil3d_light* l = lil3d_get_light(light);
    return l ? l->type : LIL3D_LIGHT_DIR;
}

bool LIL3D_IsLightActive(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l && l->active; }
void LIL3D_SetLightActive(LIL3D_Light light, bool active) { lil3d_light* l = lil3d_get_light(light); if (l) l->active = active; }
void LIL3D_ToggleLight(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); if (l) l->active = !l->active; }

Color LIL3D_GetLightColor(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->color : WHITE; }
void LIL3D_SetLightColor(LIL3D_Light light, Color color) { lil3d_light* l = lil3d_get_light(light); if (l) l->color = color; }

Vector3 LIL3D_GetLightColorV(LIL3D_Light light)
{
    Color c = LIL3D_GetLightColor(light);
    return (Vector3){ c.r/255.0f, c.g/255.0f, c.b/255.0f };
}

void LIL3D_SetLightColorV(LIL3D_Light light, Vector3 color)
{
    Color c = {
        (unsigned char)(Clamp(color.x, 0.0f, 1.0f)*255.0f + 0.5f),
        (unsigned char)(Clamp(color.y, 0.0f, 1.0f)*255.0f + 0.5f),
        (unsigned char)(Clamp(color.z, 0.0f, 1.0f)*255.0f + 0.5f),
        255
    };
    LIL3D_SetLightColor(light, c);
}

Vector3 LIL3D_GetLightPosition(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->position : Vector3Zero(); }
void LIL3D_SetLightPosition(LIL3D_Light light, Vector3 position) { lil3d_light* l = lil3d_get_light(light); if (l) l->position = position; }
Vector3 LIL3D_GetLightDirection(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->direction : (Vector3){ 0, -1, 0 }; }

void LIL3D_SetLightDirection(LIL3D_Light light, Vector3 direction)
{
    lil3d_light* l = lil3d_get_light(light);
    if (l) l->direction = lil3d_safe_normalize(direction, (Vector3){ 0.0f, -1.0f, 0.0f });
}

void LIL3D_LightLookAt(LIL3D_Light light, Vector3 position, Vector3 target)
{
    lil3d_light* l = lil3d_get_light(light);
    if (l == NULL) return;
    l->position = position;
    l->direction = lil3d_safe_normalize(Vector3Subtract(target, position), (Vector3){ 0.0f, -1.0f, 0.0f });
}

float LIL3D_GetLightEnergy(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->energy : 0.0f; }
void LIL3D_SetLightEnergy(LIL3D_Light light, float energy) { lil3d_light* l = lil3d_get_light(light); if (l) l->energy = LIL3D_MAX(energy, 0.0f); }
float LIL3D_GetLightSpecular(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->specular : 0.0f; }
void LIL3D_SetLightSpecular(LIL3D_Light light, float specular) { lil3d_light* l = lil3d_get_light(light); if (l) l->specular = LIL3D_MAX(specular, 0.0f); }

float LIL3D_GetLightRange(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->range : 0.0f; }
void LIL3D_SetLightRange(LIL3D_Light light, float range) { lil3d_light* l = lil3d_get_light(light); if (l) l->range = LIL3D_MAX(range, 0.01f); }
float LIL3D_GetLightAttenuation(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->attenuation : 0.0f; }
void LIL3D_SetLightAttenuation(LIL3D_Light light, float attenuation) { lil3d_light* l = lil3d_get_light(light); if (l) l->attenuation = LIL3D_MAX(attenuation, 0.01f); }

float LIL3D_GetLightInnerCutOff(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->innerCutOff : 0.0f; }
void LIL3D_SetLightInnerCutOff(LIL3D_Light light, float degrees) { lil3d_light* l = lil3d_get_light(light); if (l) l->innerCutOff = Clamp(degrees, 0.0f, 89.0f); }
float LIL3D_GetLightOuterCutOff(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->outerCutOff : 0.0f; }
void LIL3D_SetLightOuterCutOff(LIL3D_Light light, float degrees) { lil3d_light* l = lil3d_get_light(light); if (l) l->outerCutOff = Clamp(degrees, 1.0f, 89.0f); }

bool LIL3D_IsShadowEnabled(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l && l->shadow; }
void LIL3D_EnableShadow(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); if (l) l->shadow = true; }

void LIL3D_DisableShadow(LIL3D_Light light)
{
    lil3d_light* l = lil3d_get_light(light);
    if (l == NULL) return;
    l->shadow = false;
    lil3d_free_shadow_map(l);
}

float LIL3D_GetShadowBias(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->shadowBias : 0.0f; }
void LIL3D_SetShadowBias(LIL3D_Light light, float bias) { lil3d_light* l = lil3d_get_light(light); if (l) l->shadowBias = LIL3D_MAX(bias, 0.0f); }
float LIL3D_GetShadowSoftness(LIL3D_Light light) { lil3d_light* l = lil3d_get_light(light); return l ? l->shadowSoftness : 0.0f; }
void LIL3D_SetShadowSoftness(LIL3D_Light light, float softness) { lil3d_light* l = lil3d_get_light(light); if (l) l->shadowSoftness = LIL3D_MAX(softness, 0.1f); }

/* -------------------------------------------------------------------------- */
/* Cubemaps / sky                                                             */
/* -------------------------------------------------------------------------- */

/* Direction of texel (i, j) on a GL cubemap face (OpenGL spec table 8.19, inverted). */
static Vector3 lil3d_cube_dir(int face, int i, int j, int size)
{
    float sc = 2.0f*((float)i + 0.5f)/(float)size - 1.0f;
    float tc = 2.0f*((float)j + 0.5f)/(float)size - 1.0f;
    Vector3 r;
    switch (face)
    {
        case 0:  r = (Vector3){ 1.0f, -tc, -sc }; break;
        case 1:  r = (Vector3){ -1.0f, -tc, sc }; break;
        case 2:  r = (Vector3){ sc, 1.0f, tc }; break;
        case 3:  r = (Vector3){ sc, -1.0f, -tc }; break;
        case 4:  r = (Vector3){ sc, -tc, 1.0f }; break;
        default: r = (Vector3){ -sc, -tc, -1.0f }; break;
    }
    return Vector3Normalize(r);
}

static Color lil3d_average_image(Image img)
{
    if (img.data == NULL || img.width <= 0 || img.height <= 0) return GRAY;
    int sx = LIL3D_MAX(1, img.width/64), sy = LIL3D_MAX(1, img.height/64);
    float r = 0.0f, g = 0.0f, b = 0.0f;
    int n = 0;
    for (int y = 0; y < img.height; y += sy)
    {
        for (int x = 0; x < img.width; x += sx)
        {
            Color c = GetImageColor(img, x, y);
            r += c.r; g += c.g; b += c.b; n++;
        }
    }
    if (n == 0) return GRAY;
    Color out = { (unsigned char)(r/n + 0.5f), (unsigned char)(g/n + 0.5f), (unsigned char)(b/n + 0.5f), 255 };
    return out;
}

static void lil3d_sample_bilinear(const unsigned char* px, int w, int h, float u, float v, unsigned char out[4])
{
    float x = u*(float)w - 0.5f, y = v*(float)h - 0.5f;
    int x0 = (int)floorf(x), y0 = (int)floorf(y);
    float fx = x - (float)x0, fy = y - (float)y0;
    int x1 = x0 + 1, y1 = y0 + 1;
    x0 = ((x0 % w) + w) % w;            /* wrap horizontally (longitude) */
    x1 = ((x1 % w) + w) % w;
    y0 = (y0 < 0) ? 0 : ((y0 >= h) ? h - 1 : y0);   /* clamp vertically (latitude) */
    y1 = (y1 < 0) ? 0 : ((y1 >= h) ? h - 1 : y1);
    for (int c = 0; c < 4; c++)
    {
        float a = (float)px[(y0*w + x0)*4 + c]*(1.0f - fx) + (float)px[(y0*w + x1)*4 + c]*fx;
        float b = (float)px[(y1*w + x0)*4 + c]*(1.0f - fx) + (float)px[(y1*w + x1)*4 + c]*fx;
        out[c] = (unsigned char)(a*(1.0f - fy) + b*fy + 0.5f);
    }
}

/* Equirectangular -> 6 faces stacked vertically (GL face order). Image centre = -Z,
 * +X to the right, top row = zenith. */
static Image lil3d_panorama_to_faces(Image src, int size)
{
    Image px = ImageCopy(src);
    ImageFormat(&px, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Image faces = GenImageColor(size, size*6, BLACK);
    unsigned char* dst = (unsigned char*)faces.data;

    for (int face = 0; face < 6; face++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int i = 0; i < size; i++)
            {
                Vector3 r = lil3d_cube_dir(face, i, j, size);
                float u = 0.5f + atan2f(r.x, -r.z)/(2.0f*PI);
                float v = 0.5f - asinf(Clamp(r.y, -1.0f, 1.0f))/PI;
                lil3d_sample_bilinear((const unsigned char*)px.data, px.width, px.height, u, v,
                    &dst[((face*size + j)*size + i)*4]);
            }
        }
    }
    UnloadImage(px);
    return faces;
}

/* Uploads a cubemap WITHOUT mipmaps. raylib's LoadTextureCubemap() builds the mip chain of the
 * whole 1:6 face strip (ImageMipmaps), which does not match the per-face chain rlLoadTextureCubemap()
 * reads: for cross/line-horizontal layouts it overruns the buffer (heap-buffer-overflow). Sky and
 * reflections are sampled without LOD, so a single level is enough. Returns the GL id (0 = failure). */
static unsigned int lil3d_upload_cubemap(Image image, int rlLayout, int* outSize)
{
    int size = 0;
    switch (rlLayout)
    {
        case CUBEMAP_LAYOUT_LINE_VERTICAL:     size = image.height/6; break;
        case CUBEMAP_LAYOUT_LINE_HORIZONTAL:   size = image.width/6; break;
        case CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR: size = image.width/3; break;
        case CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE: size = image.width/4; break;
        default: break;
    }
    if (size <= 0 || image.data == NULL) return 0;

    Image src = ImageCopy(image);
    ImageFormat(&src, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    src.mipmaps = 1;
    if (src.data == NULL) return 0;

    Image faces = { 0 };
    if (rlLayout == CUBEMAP_LAYOUT_LINE_VERTICAL)
    {
        faces = src;
    }
    else
    {
        Rectangle rec[6];
        for (int i = 0; i < 6; i++) rec[i] = (Rectangle){ 0, 0, (float)size, (float)size };
        const float s = (float)size;
        if (rlLayout == CUBEMAP_LAYOUT_LINE_HORIZONTAL)
        {
            for (int i = 0; i < 6; i++) rec[i].x = s*(float)i;
        }
        else if (rlLayout == CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR)
        {
            rec[0].x = s;   rec[0].y = s;
            rec[1].x = s;   rec[1].y = s*3;
            rec[2].x = s;   rec[2].y = 0;
            rec[3].x = s;   rec[3].y = s*2;
            rec[4].x = 0;   rec[4].y = s;
            rec[5].x = s*2; rec[5].y = s;
        }
        else /* CROSS_FOUR_BY_THREE */
        {
            rec[0].x = s*2; rec[0].y = s;
            rec[1].x = 0;   rec[1].y = s;
            rec[2].x = s;   rec[2].y = 0;
            rec[3].x = s;   rec[3].y = s*2;
            rec[4].x = s;   rec[4].y = s;
            rec[5].x = s*3; rec[5].y = s;
        }
        faces = GenImageColor(size, size*6, MAGENTA);
        ImageFormat(&faces, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        for (int i = 0; i < 6; i++) ImageDraw(&faces, src, rec[i], (Rectangle){ 0, s*(float)i, s, s }, WHITE);
        UnloadImage(src);
    }

    unsigned int id = 0;
    if (faces.data != NULL && faces.height >= size*6) id = rlLoadTextureCubemap(faces.data, size, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
    UnloadImage(faces);
    if (outSize != NULL) *outSize = size;
    return id;
}

LIL3D_Cubemap LIL3D_LoadCubemapFromImage(Image image, LIL3D_CubemapLayout layout)
{
    LIL3D_Cubemap out = { 0 };
    if (image.data == NULL || image.width <= 0 || image.height <= 0) return out;

    if (layout == LIL3D_CUBEMAP_LAYOUT_AUTO_DETECT)     /* same heuristics as raylib */
    {
        int w = image.width, h = image.height;
        if (w > h)
        {
            if (w/6 == h) layout = LIL3D_CUBEMAP_LAYOUT_LINE_HORIZONTAL;
            else if (w/4 == h/3) layout = LIL3D_CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE;
            else if (w >= (int)((float)h*1.85f)) layout = LIL3D_CUBEMAP_LAYOUT_PANORAMA;
        }
        else if (h > w)
        {
            if (h/6 == w) layout = LIL3D_CUBEMAP_LAYOUT_LINE_VERTICAL;
            else if (w/3 == h/4) layout = LIL3D_CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR;
        }
        if (layout == LIL3D_CUBEMAP_LAYOUT_AUTO_DETECT)
        {
            TraceLog(LOG_ERROR, LIL3D_LOG_TAG "could not auto-detect the cubemap layout of a %dx%d image", w, h);
            return out;
        }
    }

    unsigned int texId = 0;
    int texSize = 0;
    if (layout == LIL3D_CUBEMAP_LAYOUT_PANORAMA)
    {
        int size = image.height/2;
        size = (size < 64) ? 64 : ((size > 1024) ? 1024 : size);
        Image faces = lil3d_panorama_to_faces(image, size);
        texId = lil3d_upload_cubemap(faces, CUBEMAP_LAYOUT_LINE_VERTICAL, &texSize);
        UnloadImage(faces);
    }
    else
    {
        int rl = CUBEMAP_LAYOUT_LINE_VERTICAL;
        switch (layout)
        {
            case LIL3D_CUBEMAP_LAYOUT_LINE_HORIZONTAL:     rl = CUBEMAP_LAYOUT_LINE_HORIZONTAL; break;
            case LIL3D_CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR: rl = CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR; break;
            case LIL3D_CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE: rl = CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE; break;
            default: break;
        }
        texId = lil3d_upload_cubemap(image, rl, &texSize);
    }

    if (texId == 0)
    {
        TraceLog(LOG_ERROR, LIL3D_LOG_TAG "failed to create the cubemap texture");
        return out;
    }
    out.texture = texId;
    out.size = texSize;
    out.average = lil3d_average_image(image);
    return out;
}

LIL3D_Cubemap LIL3D_LoadCubemap(const char* fileName, LIL3D_CubemapLayout layout)
{
    LIL3D_Cubemap out = { 0 };
    Image img = LoadImage(fileName);
    if (img.data == NULL) return out;
    out = LIL3D_LoadCubemapFromImage(img, layout);
    UnloadImage(img);
    return out;
}

LIL3D_Cubemap LIL3D_GenGradientSky(int size, Color top, Color horizon, Color bottom)
{
    LIL3D_Cubemap out = { 0 };
    if (size < 4) size = 4;
    Image faces = GenImageColor(size, size*6, BLACK);
    unsigned char* dst = (unsigned char*)faces.data;

    for (int face = 0; face < 6; face++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int i = 0; i < size; i++)
            {
                float y = lil3d_cube_dir(face, i, j, size).y;
                float t = powf(fabsf(y), 0.6f);
                Color e = (y >= 0.0f) ? top : bottom;
                unsigned char* p = &dst[((face*size + j)*size + i)*4];
                p[0] = (unsigned char)Lerp((float)horizon.r, (float)e.r, t);
                p[1] = (unsigned char)Lerp((float)horizon.g, (float)e.g, t);
                p[2] = (unsigned char)Lerp((float)horizon.b, (float)e.b, t);
                p[3] = 255;
            }
        }
    }

    int texSize = 0;
    unsigned int texId = lil3d_upload_cubemap(faces, CUBEMAP_LAYOUT_LINE_VERTICAL, &texSize);
    if (texId != 0)
    {
        out.texture = texId;
        out.size = texSize;
        out.average = lil3d_average_image(faces);
    }
    UnloadImage(faces);
    return out;
}

void LIL3D_UnloadCubemap(LIL3D_Cubemap cubemap)
{
    if (cubemap.texture != 0) rlUnloadTexture(cubemap.texture);
}

/* -------------------------------------------------------------------------- */
/* Materials / meshes / models / instances                                    */
/* -------------------------------------------------------------------------- */

LIL3D_Material LIL3D_GetDefaultMaterial(void)
{
    LIL3D_Material m;
    memset(&m, 0, sizeof(m));
    m.albedo.texture = lil3d_white_texture();
    m.albedo.color = WHITE;
    m.emission.color = BLACK;
    m.emission.energy = 1.0f;
    m.orm.texture = lil3d_white_texture();
    m.orm.occlusion = 1.0f;
    m.orm.roughness = 0.5f;
    m.orm.metalness = 0.0f;
    m.uvOffset = (Vector2){ 0.0f, 0.0f };
    m.uvScale = (Vector2){ 1.0f, 1.0f };
    m.alphaCutoff = 0.0f;
    m.blendMode = LIL3D_BLEND_OPAQUE;
    m.cullMode = LIL3D_CULL_BACK;
    m.shadowCastMode = LIL3D_SHADOW_CAST_ON;
    return m;
}

LIL3D_Mesh LIL3D_LoadMesh(Mesh mesh)
{
    if (mesh.vboId == NULL || mesh.vboId[0] == 0) UploadMesh(&mesh, false);
    LIL3D_Mesh out;
    out.mesh = mesh;
    out.aabb = GetMeshBoundingBox(mesh);
    return out;
}

void LIL3D_UnloadMesh(LIL3D_Mesh mesh) { UnloadMesh(mesh.mesh); }

LIL3D_Mesh LIL3D_GenMeshPlane(float width, float length, int resX, int resZ) { return LIL3D_LoadMesh(GenMeshPlane(width, length, resX, resZ)); }
LIL3D_Mesh LIL3D_GenMeshCube(float width, float height, float length) { return LIL3D_LoadMesh(GenMeshCube(width, height, length)); }
LIL3D_Mesh LIL3D_GenMeshSphere(float radius, int rings, int slices) { return LIL3D_LoadMesh(GenMeshSphere(radius, rings, slices)); }
LIL3D_Mesh LIL3D_GenMeshHemiSphere(float radius, int rings, int slices) { return LIL3D_LoadMesh(GenMeshHemiSphere(radius, rings, slices)); }
LIL3D_Mesh LIL3D_GenMeshCylinder(float radius, float height, int slices) { return LIL3D_LoadMesh(GenMeshCylinder(radius, height, slices)); }
LIL3D_Mesh LIL3D_GenMeshCone(float radius, float height, int slices) { return LIL3D_LoadMesh(GenMeshCone(radius, height, slices)); }
LIL3D_Mesh LIL3D_GenMeshTorus(float radius, float size, int radSeg, int sides) { return LIL3D_LoadMesh(GenMeshTorus(radius, size, radSeg, sides)); }
LIL3D_Mesh LIL3D_GenMeshKnot(float radius, float size, int radSeg, int sides) { return LIL3D_LoadMesh(GenMeshKnot(radius, size, radSeg, sides)); }

LIL3D_Mesh LIL3D_GenMeshHeightmap(Image heightmap, Vector3 size)
{
    LIL3D_Mesh out;
    memset(&out, 0, sizeof(out));
    if (heightmap.data == NULL || heightmap.width < 2 || heightmap.height < 2)
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "LIL3D_GenMeshHeightmap(): image must be at least 2x2");
        return out;
    }

    int w = heightmap.width, h = heightmap.height;
    int step = 1;
    int mapX = w, mapZ = h;
    while ((long)mapX*(long)mapZ > 65535)       /* raylib meshes use 16-bit indices */
    {
        step++;
        mapX = (w - 1)/step + 1;
        mapZ = (h - 1)/step + 1;
    }
    if (step > 1) TraceLog(LOG_WARNING, LIL3D_LOG_TAG "heightmap %dx%d too large, sampled with stride %d (%dx%d vertices)", w, h, step, mapX, mapZ);
    if (mapX < 2 || mapZ < 2) return out;

    Color* px = LoadImageColors(heightmap);
    float* hgt = (float*)malloc((size_t)mapX*(size_t)mapZ*sizeof(float));
    if (px == NULL || hgt == NULL) { UnloadImageColors(px); free(hgt); return out; }
    for (int z = 0; z < mapZ; z++)
    {
        int sz = LIL3D_MIN(z*step, h - 1);
        for (int x = 0; x < mapX; x++)
        {
            Color c = px[sz*w + LIL3D_MIN(x*step, w - 1)];
            hgt[z*mapX + x] = ((float)c.r + (float)c.g + (float)c.b)/(3.0f*255.0f);
        }
    }
    UnloadImageColors(px);

    const int vertexCount = mapX*mapZ;
    const int triangleCount = 2*(mapX - 1)*(mapZ - 1);
    Mesh mesh = { 0 };
    mesh.vertexCount = vertexCount;
    mesh.triangleCount = triangleCount;
    mesh.vertices  = (float*)MemAlloc((unsigned int)(vertexCount*3*(int)sizeof(float)));
    mesh.normals   = (float*)MemAlloc((unsigned int)(vertexCount*3*(int)sizeof(float)));
    mesh.texcoords = (float*)MemAlloc((unsigned int)(vertexCount*2*(int)sizeof(float)));
    mesh.indices   = (unsigned short*)MemAlloc((unsigned int)(triangleCount*3*(int)sizeof(unsigned short)));
    if (!mesh.vertices || !mesh.normals || !mesh.texcoords || !mesh.indices)
    {
        UnloadMesh(mesh);
        free(hgt);
        return out;
    }

    const float dx = size.x/(float)(mapX - 1), dz = size.z/(float)(mapZ - 1);
    for (int z = 0; z < mapZ; z++)
    {
        for (int x = 0; x < mapX; x++)
        {
            int i = z*mapX + x;
            /* Centred on X/Z (like r3d), y in [0, size.y] */
            mesh.vertices[i*3 + 0] = ((float)x/(float)(mapX - 1) - 0.5f)*size.x;
            mesh.vertices[i*3 + 1] = hgt[i]*size.y;
            mesh.vertices[i*3 + 2] = ((float)z/(float)(mapZ - 1) - 0.5f)*size.z;
            mesh.texcoords[i*2 + 0] = (float)x/(float)(mapX - 1);
            mesh.texcoords[i*2 + 1] = (float)z/(float)(mapZ - 1);

            int xl = LIL3D_MAX(x - 1, 0), xr = LIL3D_MIN(x + 1, mapX - 1);
            int zd = LIL3D_MAX(z - 1, 0), zu = LIL3D_MIN(z + 1, mapZ - 1);
            float sx = (hgt[z*mapX + xr] - hgt[z*mapX + xl])*size.y/((float)(xr - xl)*dx);
            float sz = (hgt[zu*mapX + x] - hgt[zd*mapX + x])*size.y/((float)(zu - zd)*dz);
            Vector3 n = Vector3Normalize((Vector3){ -sx, 1.0f, -sz });
            mesh.normals[i*3 + 0] = n.x;
            mesh.normals[i*3 + 1] = n.y;
            mesh.normals[i*3 + 2] = n.z;
        }
    }

    int t = 0;
    for (int z = 0; z < mapZ - 1; z++)
    {
        for (int x = 0; x < mapX - 1; x++)
        {
            unsigned short i00 = (unsigned short)(z*mapX + x), i10 = (unsigned short)(i00 + 1);
            unsigned short i01 = (unsigned short)(i00 + mapX), i11 = (unsigned short)(i01 + 1);
            mesh.indices[t++] = i00; mesh.indices[t++] = i01; mesh.indices[t++] = i10;      /* CCW seen from +Y */
            mesh.indices[t++] = i10; mesh.indices[t++] = i01; mesh.indices[t++] = i11;
        }
    }
    free(hgt);
    return LIL3D_LoadMesh(mesh);
}

static BoundingBox lil3d_union_aabb(BoundingBox a, BoundingBox b)
{
    BoundingBox out = { Vector3Min(a.min, b.min), Vector3Max(a.max, b.max) };
    return out;
}

LIL3D_Model LIL3D_LoadModel(const char* filePath)
{
    LIL3D_Model out;
    memset(&out, 0, sizeof(out));

    Model m = LoadModel(filePath);
    if (m.meshCount <= 0 || m.meshes == NULL) return out;

    int matCount = (m.materialCount > 0) ? m.materialCount : 1;
    const bool gltf = IsFileExtension(filePath, ".gltf") || IsFileExtension(filePath, ".glb");
    out.meshes = (LIL3D_Mesh*)calloc((size_t)m.meshCount, sizeof(LIL3D_Mesh));
    out.materials = (LIL3D_Material*)calloc((size_t)matCount, sizeof(LIL3D_Material));
    out.meshMaterials = (int*)calloc((size_t)m.meshCount, sizeof(int));
    if (out.meshes == NULL || out.materials == NULL || out.meshMaterials == NULL)
    {
        free(out.meshes); free(out.materials); free(out.meshMaterials);
        memset(&out, 0, sizeof(out));
        UnloadModel(m);
        return out;
    }
    out.meshCount = m.meshCount;
    out.materialCount = matCount;

    for (int i = 0; i < matCount; i++)
    {
        out.materials[i] = LIL3D_GetDefaultMaterial();
        if (i < m.materialCount && m.materials[i].maps != NULL)
        {
            MaterialMap albedo = m.materials[i].maps[MATERIAL_MAP_ALBEDO];
            if (albedo.texture.id != 0) out.materials[i].albedo.texture = albedo.texture;
            out.materials[i].albedo.color = albedo.color;

            /* raylib fills the metallic/roughness factors (and texture) for glTF only;
             * for other formats those slots are zero, which would mean a perfect mirror. */
            if (gltf)
            {
                MaterialMap mm = m.materials[i].maps[MATERIAL_MAP_METALNESS];
                MaterialMap rm = m.materials[i].maps[MATERIAL_MAP_ROUGHNESS];
                MaterialMap om = m.materials[i].maps[MATERIAL_MAP_OCCLUSION];
                Texture2D mr = (rm.texture.id != 0) ? rm.texture : mm.texture;
                if (mr.id != 0 || mm.value > 0.0f || rm.value > 0.0f)
                {
                    out.materials[i].orm.metalness = mm.value;
                    out.materials[i].orm.roughness = rm.value;
                }
                if (mr.id != 0)
                {
                    out.materials[i].orm.texture = mr;
                    /* R is only occlusion when the file packs it into the same texture */
                    out.materials[i].orm.occlusion = (om.texture.id == mr.id) ? 1.0f : 0.0f;
                }
            }
        }
    }
    for (int i = 0; i < m.meshCount; i++)
    {
        out.meshes[i].mesh = m.meshes[i];
        out.meshes[i].aabb = GetMeshBoundingBox(m.meshes[i]);
        int mi = (m.meshMaterial != NULL) ? m.meshMaterial[i] : 0;
        out.meshMaterials[i] = (mi >= 0 && mi < matCount) ? mi : 0;
        out.aabb = (i == 0) ? out.meshes[i].aabb : lil3d_union_aabb(out.aabb, out.meshes[i].aabb);
    }

    /* Keep the skeleton (bones + model-space bind pose) for the animation player.
     * raylib 6.0: the skeleton lives in Model.skeleton (ModelSkeleton). */
    if (m.skeleton.boneCount > 0 && m.skeleton.bones != NULL && m.skeleton.bindPose != NULL)
    {
        out.skeleton.invBind = (Matrix*)malloc((size_t)m.skeleton.boneCount*sizeof(Matrix));
        if (out.skeleton.invBind != NULL)
        {
            out.skeleton.boneCount = m.skeleton.boneCount;
            out.skeleton.bones = m.skeleton.bones;
            out.skeleton.bindPose = m.skeleton.bindPose;
            for (int i = 0; i < m.skeleton.boneCount; i++) out.skeleton.invBind[i] = MatrixInvert(lil3d_trs(m.skeleton.bindPose[i]));
            m.skeleton.bones = NULL;
            m.skeleton.bindPose = NULL;
        }
    }

    /* Release raylib's shell but keep the GPU resources we now own. */
    for (int i = 0; i < m.materialCount; i++) MemFree(m.materials[i].maps);
    MemFree(m.materials);
    MemFree(m.meshes);
    MemFree(m.meshMaterial);
    MemFree(m.skeleton.bones);          /* NULL if handed over to out.skeleton above */
    MemFree(m.skeleton.bindPose);
    MemFree(m.currentPose);             /* raylib 6.0 runtime pose + skinning matrices: unused here */
    MemFree(m.boneMatrices);
    return out;
}

LIL3D_Model LIL3D_LoadModelFromMesh(LIL3D_Mesh mesh)
{
    LIL3D_Model out;
    memset(&out, 0, sizeof(out));
    out.meshes = (LIL3D_Mesh*)calloc(1, sizeof(LIL3D_Mesh));
    out.materials = (LIL3D_Material*)calloc(1, sizeof(LIL3D_Material));
    out.meshMaterials = (int*)calloc(1, sizeof(int));
    if (out.meshes == NULL || out.materials == NULL || out.meshMaterials == NULL)
    {
        free(out.meshes); free(out.materials); free(out.meshMaterials);
        memset(&out, 0, sizeof(out));
        return out;
    }
    out.meshes[0] = mesh;
    out.materials[0] = LIL3D_GetDefaultMaterial();
    out.meshCount = 1;
    out.materialCount = 1;
    out.aabb = mesh.aabb;
    return out;
}

void LIL3D_UnloadModel(LIL3D_Model model, bool unloadMaterials)
{
    for (int i = 0; i < model.meshCount; i++) UnloadMesh(model.meshes[i].mesh);
    if (unloadMaterials)
    {
        unsigned int white = rlGetTextureIdDefault();
        for (int i = 0; i < model.materialCount; i++)
        {
            Texture2D t = model.materials[i].albedo.texture;
            if (t.id != 0 && t.id != white) UnloadTexture(t);
            Texture2D o = model.materials[i].orm.texture;
            if (o.id != 0 && o.id != white && o.id != t.id) UnloadTexture(o);
        }
    }
    MemFree(model.skeleton.bones);
    MemFree(model.skeleton.bindPose);
    free(model.skeleton.invBind);
    free(model.meshes);
    free(model.materials);
    free(model.meshMaterials);
}

LIL3D_InstanceBuffer LIL3D_LoadInstanceBuffer(int capacity)
{
    LIL3D_InstanceBuffer b = { NULL, 0 };
    if (capacity <= 0) return b;
    b.transforms = (Matrix*)malloc((size_t)capacity*sizeof(Matrix));
    if (b.transforms == NULL) return b;
    for (int i = 0; i < capacity; i++) b.transforms[i] = MatrixIdentity();
    b.capacity = capacity;
    return b;
}

void LIL3D_UnloadInstanceBuffer(LIL3D_InstanceBuffer buffer) { free(buffer.transforms); }

void LIL3D_UpdateInstanceBuffer(LIL3D_InstanceBuffer buffer, const Matrix* transforms, int count)
{
    if (buffer.transforms == NULL || transforms == NULL || count <= 0) return;
    if (count > buffer.capacity) count = buffer.capacity;
    memcpy(buffer.transforms, transforms, (size_t)count*sizeof(Matrix));
}

Matrix* LIL3D_MapInstances(LIL3D_InstanceBuffer buffer) { return buffer.transforms; }

/* -------------------------------------------------------------------------- */
/* Draw queue                                                                 */
/* -------------------------------------------------------------------------- */

static void lil3d_push_draw(LIL3D_Mesh mesh, LIL3D_Material material, Matrix transform, const Matrix* instances, int instanceCount)
{
    if (!lil3d_ctx.initialized || !lil3d_ctx.inFrame)
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "draw call outside LIL3D_Begin()/LIL3D_End(); ignored");
        return;
    }
    if (mesh.mesh.vertexCount <= 0) return;
    if (instances != NULL && instanceCount <= 0) return;

    lil3d_drawcall* grown = (lil3d_drawcall*)lil3d_grow_array(lil3d_ctx.draws, &lil3d_ctx.drawCap, lil3d_ctx.drawCount + 1, sizeof(lil3d_drawcall));
    if (grown == NULL) return;
    lil3d_ctx.draws = grown;

    lil3d_drawcall* dc = &lil3d_ctx.draws[lil3d_ctx.drawCount++];
    dc->mesh = mesh;
    dc->material = material;
    dc->transform = transform;
    dc->instances = instances;
    dc->instanceCount = instanceCount;
    dc->hasBounds = (instances == NULL);
    if (dc->hasBounds) dc->bounds = lil3d_transform_aabb(mesh.aabb, transform);
    else memset(&dc->bounds, 0, sizeof(dc->bounds));

    Vector3 center = dc->hasBounds
        ? Vector3Scale(Vector3Add(dc->bounds.min, dc->bounds.max), 0.5f)
        : (Vector3){ transform.m12, transform.m13, transform.m14 };
    dc->sortKey = Vector3DistanceSqr(center, lil3d_ctx.camera.position);
}

static Matrix lil3d_compose(Vector3 position, Quaternion rotation, Vector3 scale)
{
    return MatrixMultiply(MatrixMultiply(MatrixScale(scale.x, scale.y, scale.z), QuaternionToMatrix(rotation)),
                          MatrixTranslate(position.x, position.y, position.z));
}

void LIL3D_DrawMesh(LIL3D_Mesh mesh, LIL3D_Material material, Vector3 position, float scale)
{
    Matrix t = MatrixMultiply(MatrixScale(scale, scale, scale), MatrixTranslate(position.x, position.y, position.z));
    lil3d_push_draw(mesh, material, t, NULL, 0);
}

void LIL3D_DrawMeshEx(LIL3D_Mesh mesh, LIL3D_Material material, Vector3 position, Quaternion rotation, Vector3 scale)
{
    lil3d_push_draw(mesh, material, lil3d_compose(position, rotation, scale), NULL, 0);
}

void LIL3D_DrawMeshPro(LIL3D_Mesh mesh, LIL3D_Material material, Matrix transform)
{
    lil3d_push_draw(mesh, material, transform, NULL, 0);
}

void LIL3D_DrawMeshInstanced(LIL3D_Mesh mesh, LIL3D_Material material, LIL3D_InstanceBuffer instances, int count)
{
    LIL3D_DrawMeshInstancedEx(mesh, material, instances, count, MatrixIdentity());
}

void LIL3D_DrawMeshInstancedEx(LIL3D_Mesh mesh, LIL3D_Material material, LIL3D_InstanceBuffer instances, int count, Matrix transform)
{
    if (instances.transforms == NULL) return;
    if (count > instances.capacity) count = instances.capacity;
    lil3d_push_draw(mesh, material, transform, instances.transforms, count);
}

static const LIL3D_Material* lil3d_model_material(const LIL3D_Model* model, int meshIndex)
{
    int mi = (model->meshMaterials != NULL) ? model->meshMaterials[meshIndex] : 0;
    if (mi < 0 || mi >= model->materialCount) mi = 0;
    return &model->materials[mi];
}

void LIL3D_DrawModelPro(LIL3D_Model model, Matrix transform)
{
    for (int i = 0; i < model.meshCount; i++)
        lil3d_push_draw(model.meshes[i], *lil3d_model_material(&model, i), transform, NULL, 0);
}

void LIL3D_DrawModel(LIL3D_Model model, Vector3 position, float scale)
{
    LIL3D_DrawModelPro(model, MatrixMultiply(MatrixScale(scale, scale, scale), MatrixTranslate(position.x, position.y, position.z)));
}

void LIL3D_DrawModelEx(LIL3D_Model model, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale)
{
    Matrix t = MatrixMultiply(MatrixMultiply(MatrixScale(scale.x, scale.y, scale.z), MatrixRotate(rotationAxis, rotationAngle*DEG2RAD)),
                              MatrixTranslate(position.x, position.y, position.z));
    LIL3D_DrawModelPro(model, t);
}

void LIL3D_DrawModelInstancedEx(LIL3D_Model model, LIL3D_InstanceBuffer instances, int count, Matrix transform)
{
    for (int i = 0; i < model.meshCount; i++)
        LIL3D_DrawMeshInstancedEx(model.meshes[i], *lil3d_model_material(&model, i), instances, count, transform);
}

void LIL3D_DrawModelInstanced(LIL3D_Model model, LIL3D_InstanceBuffer instances, int count)
{
    LIL3D_DrawModelInstancedEx(model, instances, count, MatrixIdentity());
}

/* -------------------------------------------------------------------------- */
/* Frame: low-level drawing helpers                                           */
/* -------------------------------------------------------------------------- */

/* Submit one draw call through raylib using the given program + material. */
static void lil3d_submit(Shader shader, Material* material, const lil3d_drawcall* dc, int locInstanceBase)
{
    if (dc->instances == NULL)
    {
        DrawMesh(dc->mesh.mesh, *material, dc->transform);
    }
    else if (lil3d_ctx.hwInstancing)
    {
        SetShaderValueMatrix(shader, locInstanceBase, dc->transform);
        DrawMeshInstanced(dc->mesh.mesh, *material, dc->instances, dc->instanceCount);
    }
    else
    {
        for (int i = 0; i < dc->instanceCount; i++)
            DrawMesh(dc->mesh.mesh, *material, MatrixMultiply(dc->instances[i], dc->transform));
    }
}

static void lil3d_apply_material_state(const LIL3D_Material* m)
{
    if (m->blendMode != lil3d_ctx.curBlend)
    {
        switch (m->blendMode)
        {
            case LIL3D_BLEND_ALPHA:
                rlEnableColorBlend(); rlSetBlendMode(RL_BLEND_ALPHA); rlDisableDepthMask(); break;
            case LIL3D_BLEND_ADDITIVE:
                rlEnableColorBlend(); rlSetBlendMode(RL_BLEND_ADDITIVE); rlDisableDepthMask(); break;
            default:
                rlDisableColorBlend(); rlEnableDepthMask(); break;
        }
        lil3d_ctx.curBlend = m->blendMode;
    }
    if (m->cullMode != lil3d_ctx.curCull)
    {
        switch (m->cullMode)
        {
            case LIL3D_CULL_NONE:  rlDisableBackfaceCulling(); break;
            case LIL3D_CULL_FRONT: rlEnableBackfaceCulling(); rlSetCullFace(RL_CULL_FACE_FRONT); break;
            default:               rlEnableBackfaceCulling(); rlSetCullFace(RL_CULL_FACE_BACK); break;
        }
        lil3d_ctx.curCull = m->cullMode;
    }
}

static Texture2D lil3d_albedo_or_white(const LIL3D_Material* m)
{
    return (m->albedo.texture.id != 0) ? m->albedo.texture : lil3d_white_texture();
}

/* UV transform uniform (offset.xy, scale.zw), cached per program. A zeroed scale (material not
 * created through LIL3D_GetDefaultMaterial) is treated as 1 so the texture never collapses. */
static void lil3d_set_uv(Shader shader, int loc, float* last, const LIL3D_Material* m)
{
    float sx = m->uvScale.x, sy = m->uvScale.y;
    if (sx == 0.0f && sy == 0.0f) { sx = 1.0f; sy = 1.0f; }
    float uv[4] = { m->uvOffset.x, m->uvOffset.y, sx, sy };
    if (memcmp(uv, last, sizeof(uv)) != 0)
    {
        SetShaderValue(shader, loc, uv, SHADER_UNIFORM_VEC4);
        memcpy(last, uv, sizeof(uv));
    }
}

static void lil3d_draw_lit(const lil3d_drawcall* dc)
{
    lil3d_lit_program* p = (dc->instances != NULL && lil3d_ctx.hwInstancing) ? &lil3d_ctx.litInst : &lil3d_ctx.lit;
    const LIL3D_Material* m = &dc->material;

    p->material.maps[MATERIAL_MAP_ALBEDO].texture = lil3d_albedo_or_white(m);
    p->material.maps[MATERIAL_MAP_ALBEDO].color = m->albedo.color;
    p->material.maps[MATERIAL_MAP_EMISSION].texture = (m->orm.texture.id != 0) ? m->orm.texture : lil3d_white_texture();

    float mat[4] = { m->orm.metalness, m->orm.roughness, m->alphaCutoff, m->orm.occlusion };
    if (memcmp(mat, p->lastMat, sizeof(mat)) != 0)
    {
        SetShaderValue(p->shader, p->locMat, mat, SHADER_UNIFORM_VEC4);
        memcpy(p->lastMat, mat, sizeof(mat));
    }
    float em[4] = {
        lil3d_to_linear(m->emission.color.r)*m->emission.energy,
        lil3d_to_linear(m->emission.color.g)*m->emission.energy,
        lil3d_to_linear(m->emission.color.b)*m->emission.energy, 0.0f
    };
    if (memcmp(em, p->lastEmission, sizeof(em)) != 0)
    {
        SetShaderValue(p->shader, p->locEmission, em, SHADER_UNIFORM_VEC4);
        memcpy(p->lastEmission, em, sizeof(em));
    }

    lil3d_set_uv(p->shader, p->locUV, p->lastUV, m);
    lil3d_apply_material_state(m);
    lil3d_submit(p->shader, &p->material, dc, p->locInstanceBase);
}

static void lil3d_draw_depth(const lil3d_drawcall* dc)
{
    lil3d_depth_program* p = (dc->instances != NULL && lil3d_ctx.hwInstancing) ? &lil3d_ctx.depthInst : &lil3d_ctx.depth;
    const LIL3D_Material* m = &dc->material;

    p->material.maps[MATERIAL_MAP_ALBEDO].texture = lil3d_albedo_or_white(m);
    p->material.maps[MATERIAL_MAP_ALBEDO].color = m->albedo.color;
    if (m->alphaCutoff != p->lastAlpha)
    {
        SetShaderValue(p->shader, p->locAlpha, &m->alphaCutoff, SHADER_UNIFORM_FLOAT);
        p->lastAlpha = m->alphaCutoff;
    }
    lil3d_set_uv(p->shader, p->locUV, p->lastUV, m);
    lil3d_submit(p->shader, &p->material, dc, p->locInstanceBase);
}

/* Replacement for BeginMode3D() with explicit matrices (used for shadow views). */
static void lil3d_view_begin(Matrix view, Matrix proj)
{
    rlDrawRenderBatchActive();
    rlMatrixMode(RL_PROJECTION);
    rlPushMatrix();
    rlLoadIdentity();
    rlMultMatrixf(MatrixToFloat(proj));
    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();
    rlMultMatrixf(MatrixToFloat(view));
    rlEnableDepthTest();
}

static void lil3d_view_end(void)
{
    rlDrawRenderBatchActive();
    rlMatrixMode(RL_PROJECTION);
    rlPopMatrix();
    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();
    rlDisableDepthTest();
}

/* -------------------------------------------------------------------------- */
/* Frame: lights + shadow passes                                              */
/* -------------------------------------------------------------------------- */

static int lil3d_shadow_size(const lil3d_light* l)
{
    int s = lil3d_hints[(l->type == LIL3D_LIGHT_OMNI) ? LIL3D_HINT_OMNI_SHADOW_MAP_SIZE : LIL3D_HINT_SHADOW_MAP_SIZE];
    return (s < 64) ? 64 : ((s > 4096) ? 4096 : s);
}

static float lil3d_spot_fov_deg(const lil3d_light* l)
{
    return Clamp(2.0f*l->outerCutOff, 10.0f, 170.0f);
}

static Vector3 lil3d_pick_up(Vector3 dir)
{
    return (fabsf(dir.y) > 0.99f) ? (Vector3){ 0.0f, 0.0f, 1.0f } : (Vector3){ 0.0f, 1.0f, 0.0f };
}

static Vector3 lil3d_sort_cam;

static int lil3d_cmp_lights(const void* a, const void* b)
{
    const lil3d_light* la = *(lil3d_light* const*)a;
    const lil3d_light* lb = *(lil3d_light* const*)b;
    int sa = la->shadow ? 0 : 1, sb = lb->shadow ? 0 : 1;
    if (sa != sb) return sa - sb;                                   /* shadowed lights first */
    int da = (la->type == LIL3D_LIGHT_DIR) ? 0 : 1, db = (lb->type == LIL3D_LIGHT_DIR) ? 0 : 1;
    if (da != db) return da - db;                                   /* then directional      */
    float ka = Vector3DistanceSqr(la->position, lil3d_sort_cam);
    float kb = Vector3DistanceSqr(lb->position, lil3d_sort_cam);
    return (ka > kb) - (ka < kb);                                   /* then nearest          */
}

static void lil3d_select_lights(lil3d_frame* f)
{
    lil3d_context* c = &lil3d_ctx;
    memset(f, 0, sizeof(*f));

    lil3d_light** cand = (lil3d_light**)lil3d_grow_array(c->candidates, &c->candidateCap, c->lightCap > 0 ? c->lightCap : 1, sizeof(lil3d_light*));
    if (cand != NULL) c->candidates = cand;

    int n = 0;
    if (c->candidates != NULL)
    {
        for (int i = 0; i < c->lightCap; i++)
        {
            lil3d_light* l = &c->lights[i];
            if (l->exists && l->active) c->candidates[n++] = l;
        }
        lil3d_sort_cam = c->camera.position;
        qsort(c->candidates, (size_t)n, sizeof(lil3d_light*), lil3d_cmp_lights);
    }

    f->lightCount = LIL3D_MIN(n, LIL3D_MAX_LIGHTS);
    for (int i = 0; i < f->lightCount; i++) f->lights[i] = c->candidates[i];

    while (f->shadowCount < f->lightCount && f->shadowCount < LIL3D_MAX_SHADOWS && f->lights[f->shadowCount]->shadow)
        f->shadowCount++;

    for (int i = 0; i < f->lightCount; i++)
    {
        const lil3d_light* l = f->lights[i];
        float k = l->energy;
        float outer = l->outerCutOff;
        float inner = LIL3D_MAX(0.0f, LIL3D_MIN(l->innerCutOff, outer - 0.5f));

        f->pos[i]    = (Vector4){ l->position.x, l->position.y, l->position.z, (float)l->type };
        f->dir[i]    = (Vector4){ l->direction.x, l->direction.y, l->direction.z, 0.0f };
        f->color[i]  = (Vector4){ lil3d_to_linear(l->color.r)*k, lil3d_to_linear(l->color.g)*k, lil3d_to_linear(l->color.b)*k, l->specular };
        f->params[i] = (Vector4){ cosf(inner*DEG2RAD), cosf(outer*DEG2RAD), l->attenuation, l->range };

        if (i < f->shadowCount)
        {
            float size = (float)lil3d_shadow_size(l);
            float normalOffset;
            if (l->type == LIL3D_LIGHT_DIR) normalOffset = 1.5f*(2.0f*l->range/size);                       /* world units  */
            else if (l->type == LIL3D_LIGHT_SPOT) normalOffset = 1.5f*(2.0f*tanf(0.5f*lil3d_spot_fov_deg(l)*DEG2RAD)/size); /* per metre */
            else normalOffset = 1.5f*(2.0f/size);                                                          /* per metre    */
            f->shadow[i] = (Vector4){ 1.0f, l->shadowBias, normalOffset, l->shadowSoftness/size };
        }
    }

    const LIL3D_Environment* env = &c->env;
    Color ac = (env->ambient.fromSky && env->background.sky.texture != 0) ? env->background.sky.average : env->ambient.color;
    f->ambient = (Vector4){
        lil3d_to_linear(ac.r)*env->ambient.energy,
        lil3d_to_linear(ac.g)*env->ambient.energy,
        lil3d_to_linear(ac.b)*env->ambient.energy, 1.0f
    };
}

static bool lil3d_ensure_shadow_map(lil3d_light* l)
{
    int size = lil3d_shadow_size(l);
    if (l->shadowMap.id != 0 && l->shadowMapSize == size) return true;

    lil3d_free_shadow_map(l);
    bool omni = (l->type == LIL3D_LIGHT_OMNI);
    l->shadowMap = LoadRenderTexture(omni ? size*3 : size, omni ? size*2 : size);
    if (l->shadowMap.id == 0)
    {
        TraceLog(LOG_ERROR, LIL3D_LOG_TAG "failed to allocate a %dx%d shadow map; shadows disabled for this light", size, size);
        l->shadow = false;
        return false;
    }
    /* Packed depth cannot be filtered: point sampling + edge clamp (also NPOT-safe on WebGL1). */
    SetTextureFilter(l->shadowMap.texture, TEXTURE_FILTER_POINT);
    SetTextureWrap(l->shadowMap.texture, TEXTURE_WRAP_CLAMP);
    l->shadowMapSize = size;
    return true;
}

static void lil3d_draw_casters(const lil3d_light* l, bool cull)
{
    for (int i = 0; i < lil3d_ctx.drawCount; i++)
    {
        const lil3d_drawcall* dc = &lil3d_ctx.draws[i];
        if (dc->material.shadowCastMode == LIL3D_SHADOW_CAST_OFF) continue;
        if (cull && dc->hasBounds && !lil3d_aabb_sphere(dc->bounds, l->position, l->range)) continue;
        lil3d_draw_depth(dc);
    }
}

static void lil3d_set_depth_light(Vector4 v)
{
    SetShaderValue(lil3d_ctx.depth.shader, lil3d_ctx.depth.locLight, &v, SHADER_UNIFORM_VEC4);
    if (lil3d_ctx.hwInstancing) SetShaderValue(lil3d_ctx.depthInst.shader, lil3d_ctx.depthInst.locLight, &v, SHADER_UNIFORM_VEC4);
}

static void lil3d_render_shadow(lil3d_frame* f, int slot)
{
    lil3d_light* l = f->lights[slot];
    if (!lil3d_ensure_shadow_map(l)) { f->shadowValid[slot] = false; f->shadow[slot].x = 0.0f; return; }
    f->shadowValid[slot] = true;

    const int size = l->shadowMapSize;
    const Camera3D* cam = &lil3d_ctx.camera;
    lil3d_ctx.depth.lastAlpha = -1.0f;
    lil3d_ctx.depthInst.lastAlpha = -1.0f;
    for (int i = 0; i < 4; i++) { lil3d_ctx.depth.lastUV[i] = -1e30f; lil3d_ctx.depthInst.lastUV[i] = -1e30f; }

    BeginTextureMode(l->shadowMap);
    rlDisableColorBlend();              /* alpha channel carries packed depth */
    rlDisableBackfaceCulling();
    rlEnableDepthMask();

    if (l->type == LIL3D_LIGHT_OMNI)
    {
        float range = l->range;
        lil3d_set_depth_light((Vector4){ l->position.x, l->position.y, l->position.z, 1.0f/range });
        Matrix proj = MatrixPerspective(90.0*DEG2RAD, 1.0, LIL3D_MAX(0.05, (double)range*0.01), (double)range);

        rlEnableScissorTest();
        for (int face = 0; face < 6; face++)
        {
            int x = (face % 3)*size, y = (face / 3)*size;
            rlViewport(x, y, size, size);
            rlScissor(x, y, size, size);
            rlClearColor(255, 255, 255, 255);
            rlClearScreenBuffers();

            Matrix view = MatrixLookAt(l->position, Vector3Add(l->position, lil3d_face_fwd[face]), lil3d_face_up[face]);
            lil3d_view_begin(view, proj);
            lil3d_draw_casters(l, true);
            lil3d_view_end();
        }
        rlDisableScissorTest();
    }
    else
    {
        Vector3 dir = lil3d_safe_normalize(l->direction, (Vector3){ 0.0f, -1.0f, 0.0f });
        Vector3 up = lil3d_pick_up(dir);
        Matrix view, proj;

        if (l->type == LIL3D_LIGHT_DIR)
        {
            /* Ortho box of radius 'range' centred half a range in front of the camera,
             * snapped to the shadow texel grid to avoid shimmering when the camera moves. */
            float R = l->range;
            Vector3 fwd = lil3d_safe_normalize(Vector3Subtract(cam->target, cam->position), (Vector3){ 0, 0, -1 });
            Vector3 center = Vector3Add(cam->position, Vector3Scale(fwd, R*0.5f));
            Vector3 right = Vector3Normalize(Vector3CrossProduct(dir, up));
            Vector3 upL = Vector3CrossProduct(right, dir);
            float texel = (2.0f*R)/(float)size;
            float cx = Vector3DotProduct(center, right), cy = Vector3DotProduct(center, upL);
            float sx = floorf(cx/texel + 0.5f)*texel, sy = floorf(cy/texel + 0.5f)*texel;
            center = Vector3Add(center, Vector3Add(Vector3Scale(right, sx - cx), Vector3Scale(upL, sy - cy)));

            Vector3 eye = Vector3Subtract(center, Vector3Scale(dir, R));
            view = MatrixLookAt(eye, center, up);
            proj = MatrixOrtho(-R, R, -R, R, 0.0, 2.0*R);
            lil3d_set_depth_light((Vector4){ 0.0f, 0.0f, 0.0f, 0.0f });
        }
        else
        {
            float range = l->range;
            view = MatrixLookAt(l->position, Vector3Add(l->position, dir), up);
            proj = MatrixPerspective((double)lil3d_spot_fov_deg(l)*DEG2RAD, 1.0, LIL3D_MAX(0.05, (double)range*0.01), (double)range);
            lil3d_set_depth_light((Vector4){ l->position.x, l->position.y, l->position.z, 1.0f/range });
        }

        f->shadowVP[slot] = MatrixMultiply(view, proj);
        rlClearColor(255, 255, 255, 255);
        rlClearScreenBuffers();
        lil3d_view_begin(view, proj);
        lil3d_draw_casters(l, l->type != LIL3D_LIGHT_DIR);
        lil3d_view_end();
    }
    EndTextureMode();
}

/* -------------------------------------------------------------------------- */
/* Frame: main pass                                                           */
/* -------------------------------------------------------------------------- */

static void lil3d_upload_frame(lil3d_lit_program* p, const lil3d_frame* f)
{
    Vector3 viewPos = lil3d_ctx.camera.position;
    SetShaderValue(p->shader, p->locViewPos, &viewPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(p->shader, p->locAmbient, &f->ambient, SHADER_UNIFORM_VEC4);

    /* Sky reflections: bind the cubemap to its slot (unit 7) when there is one */
    const LIL3D_Environment* env = &lil3d_ctx.env;
    Vector4 envInfo = { 0.0f, env->background.energy, 0.0f, 0.0f };
    Texture2D* skyTex = &p->material.maps[MATERIAL_MAP_CUBEMAP].texture;
    if (env->background.sky.texture != 0)
    {
        TextureCubemap cube = { env->background.sky.texture, env->background.sky.size, env->background.sky.size, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
        *skyTex = cube;
        envInfo.x = 1.0f;
    }
    else memset(skyTex, 0, sizeof(*skyTex));
    SetShaderValue(p->shader, p->locEnv, &envInfo, SHADER_UNIFORM_VEC4);

    int count = f->lightCount;
    SetShaderValue(p->shader, p->locLightCount, &count, SHADER_UNIFORM_INT);
    if (count > 0)
    {
        SetShaderValueV(p->shader, p->locLightPos, f->pos, SHADER_UNIFORM_VEC4, count);
        SetShaderValueV(p->shader, p->locLightDir, f->dir, SHADER_UNIFORM_VEC4, count);
        SetShaderValueV(p->shader, p->locLightColor, f->color, SHADER_UNIFORM_VEC4, count);
        SetShaderValueV(p->shader, p->locLightParams, f->params, SHADER_UNIFORM_VEC4, count);
        SetShaderValueV(p->shader, p->locLightShadow, f->shadow, SHADER_UNIFORM_VEC4, count);
    }

    for (int s = 0; s < LIL3D_SHADOW_SLOTS; s++)
    {
        Texture2D* slotTex = &p->material.maps[MATERIAL_MAP_METALNESS + s].texture;
        bool used = (s < f->shadowCount) && f->shadowValid[s];
        if (used)
        {
            *slotTex = f->lights[s]->shadowMap.texture;
            SetShaderValueMatrix(p->shader, p->locShadowMat[s], f->shadowVP[s]);
        }
        else memset(slotTex, 0, sizeof(*slotTex));
    }

    for (int i = 0; i < 4; i++) { p->lastMat[i] = -1.0f; p->lastEmission[i] = -1.0f; p->lastUV[i] = -1e30f; }
}

static void lil3d_draw_sky(void)
{
    const LIL3D_Environment* env = &lil3d_ctx.env;
    if (env->background.sky.texture == 0) return;

    lil3d_sky_program* p = &lil3d_ctx.sky;
    TextureCubemap cube = { env->background.sky.texture, env->background.sky.size, env->background.sky.size, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
    p->material.maps[MATERIAL_MAP_CUBEMAP].texture = cube;
    SetShaderValue(p->shader, p->locEnergy, &env->background.energy, SHADER_UNIFORM_FLOAT);

    rlDisableColorBlend();
    rlDisableBackfaceCulling();
    rlDisableDepthMask();               /* z = w in the vertex shader: only fills untouched pixels */
    DrawMesh(lil3d_ctx.skyCube, p->material, MatrixIdentity());
    rlEnableDepthMask();
    lil3d_ctx.curBlend = -1;
    lil3d_ctx.curCull = -1;
}

static int lil3d_cmp_far_to_near(const void* a, const void* b)
{
    float ka = (*(lil3d_drawcall* const*)a)->sortKey, kb = (*(lil3d_drawcall* const*)b)->sortKey;
    return (ka < kb) - (ka > kb);
}

void LIL3D_Begin(Camera3D camera)
{
    LIL3D_BeginEx(camera, (RenderTexture2D){ 0 });
}

void LIL3D_BeginEx(Camera3D camera, RenderTexture2D target)
{
    if (!lil3d_ctx.initialized)
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "LIL3D_Begin() called before LIL3D_Init()");
        return;
    }
    if (lil3d_ctx.inFrame)
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "LIL3D_Begin() called twice without LIL3D_End()");
        return;
    }
    lil3d_ctx.camera = camera;
    lil3d_ctx.target = target;
    lil3d_ctx.hasTarget = (target.id != 0);
    lil3d_ctx.drawCount = 0;
    lil3d_ctx.inFrame = true;
}

void LIL3D_End(void)
{
    lil3d_context* c = &lil3d_ctx;
    if (!c->initialized || !c->inFrame) return;
    c->inFrame = false;

    lil3d_frame frame;
    lil3d_select_lights(&frame);

    /* 1) Shadow maps (each pass binds its own framebuffer) */
    for (int s = 0; s < frame.shadowCount; s++) lil3d_render_shadow(&frame, s);

    /* 2) Main pass */
    rlDrawRenderBatchActive();
    if (c->hasTarget) BeginTextureMode(c->target);
    Color bg = c->env.background.color;
    rlClearColor(bg.r, bg.g, bg.b, bg.a);
    rlClearScreenBuffers();

    BeginMode3D(c->camera);
        lil3d_upload_frame(&c->lit, &frame);
        if (c->hwInstancing) lil3d_upload_frame(&c->litInst, &frame);
        c->curBlend = -1;
        c->curCull = -1;

        /* Opaque */
        int transparentCount = 0;
        for (int i = 0; i < c->drawCount; i++)
        {
            if (c->draws[i].material.blendMode == LIL3D_BLEND_OPAQUE) lil3d_draw_lit(&c->draws[i]);
            else transparentCount++;
        }

        /* Sky (after opaque so early-z rejects covered pixels) */
        lil3d_draw_sky();

        /* Transparent, back-to-front */
        if (transparentCount > 0)
        {
            lil3d_drawcall** list = (lil3d_drawcall**)lil3d_grow_array(c->sortList, &c->sortCap, transparentCount, sizeof(lil3d_drawcall*));
            if (list != NULL)
            {
                c->sortList = list;
                int n = 0;
                for (int i = 0; i < c->drawCount; i++)
                    if (c->draws[i].material.blendMode != LIL3D_BLEND_OPAQUE) list[n++] = &c->draws[i];
                qsort(list, (size_t)n, sizeof(lil3d_drawcall*), lil3d_cmp_far_to_near);
                for (int i = 0; i < n; i++) lil3d_draw_lit(list[i]);
            }
        }
    EndMode3D();
    if (c->hasTarget) EndTextureMode();

    /* Restore raylib's default state */
    rlEnableColorBlend();
    rlSetBlendMode(RL_BLEND_ALPHA);
    rlEnableBackfaceCulling();
    rlSetCullFace(RL_CULL_FACE_BACK);
    rlEnableDepthMask();
    c->drawCount = 0;
}

/* -------------------------------------------------------------------------- */
/* Animation                                                                  */
/* -------------------------------------------------------------------------- */

/* IQM stores a framerate per animation in the file, but raylib throws it away.
 * Layout: 16-byte magic, then u32 header fields (see the IQM spec). Anim record:
 * name(u32) first_frame(u32) num_frames(u32) framerate(f32) flags(u32). */
static void lil3d_read_iqm_anim_info(const char* path, LIL3D_Animation* anims, float* fps, int count)
{
    int size = 0;
    unsigned char* d = LoadFileData(path, &size);
    if (d == NULL) return;

    if (size >= 16 + 19*4 && memcmp(d, "INTERQUAKEMODEL\0", 16) == 0)
    {
        unsigned int h[19];
        memcpy(h, d + 16, sizeof(h));
        unsigned int numText = h[3], ofsText = h[4], numAnims = h[17], ofsAnims = h[18];
        for (int i = 0; i < (int)numAnims && i < count; i++)
        {
            size_t o = (size_t)ofsAnims + (size_t)i*20u;
            if (o + 20u > (size_t)size) break;
            unsigned int nameOfs;
            float rate;
            memcpy(&nameOfs, d + o, 4);
            memcpy(&rate, d + o + 12, 4);
            if (rate > 0.0f) fps[i] = rate;
            if (nameOfs < numText && (size_t)ofsText + nameOfs < (size_t)size && anims[i].name[0] == '\0')
            {
                strncpy(anims[i].name, (const char*)d + ofsText + nameOfs, sizeof(anims[i].name) - 1);
            }
        }
    }
    UnloadFileData(d);
}

LIL3D_AnimationLib LIL3D_LoadAnimationLib(const char* filePath)
{
    LIL3D_AnimationLib lib;
    memset(&lib, 0, sizeof(lib));

    int count = 0;
    ModelAnimation* raw = LoadModelAnimations(filePath, &count);
    if (raw == NULL || count <= 0)
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "no animations found in '%s'", filePath);
        return lib;
    }

    LIL3D_Animation* anims = (LIL3D_Animation*)calloc((size_t)count, sizeof(LIL3D_Animation));
    float* fps = (float*)malloc((size_t)count*sizeof(float));
    if (anims == NULL || fps == NULL)
    {
        free(anims); free(fps);
        UnloadModelAnimations(raw, count);
        return lib;
    }

    for (int i = 0; i < count; i++)
    {
        fps[i] = LIL3D_ANIM_RESAMPLE_FPS;       /* glTF, M3D: raylib's own sampling step */
#ifndef LIL3D_NO_ANIMATION_NAME
        strncpy(anims[i].name, raw[i].name, sizeof(anims[i].name) - 1);
#endif
    }
    if (IsFileExtension(filePath, ".iqm")) lil3d_read_iqm_anim_info(filePath, anims, fps, count);

    for (int i = 0; i < count; i++)
    {
        LIL3D_Animation* a = &anims[i];
        if (a->name[0] == '\0') snprintf(a->name, sizeof(a->name), "anim_%d", i);
        a->boneCount = raw[i].boneCount;
        a->frameCount = raw[i].keyframeCount;      /* raylib 6.0: keyframeCount */
        a->framePoses = raw[i].keyframePoses;      /* raylib 6.0: keyframePoses */
        a->fps = fps[i];
        a->duration = (a->frameCount > 0) ? (float)a->frameCount/a->fps : 0.0f;
        TraceLog(LOG_INFO, LIL3D_LOG_TAG "animation %d '%s': %d frames @ %.2f fps (%.2f s)", i, a->name, a->frameCount, a->fps, a->duration);
    }
    free(fps);

    lib.animations = anims;
    lib.count = count;
    lib.raw = raw;
    return lib;
}

void LIL3D_UnloadAnimationLib(LIL3D_AnimationLib lib)
{
    if (lib.raw != NULL) UnloadModelAnimations(lib.raw, lib.count);
    free(lib.animations);
}

bool LIL3D_IsAnimationLibValid(LIL3D_AnimationLib lib) { return lib.animations != NULL && lib.count > 0; }

int LIL3D_GetAnimationIndex(LIL3D_AnimationLib lib, const char* name)
{
    if (name == NULL) return -1;
    for (int i = 0; i < lib.count; i++)
        if (strncmp(lib.animations[i].name, name, sizeof(lib.animations[i].name)) == 0) return i;
    return -1;
}

const LIL3D_Animation* LIL3D_GetAnimation(LIL3D_AnimationLib lib, int index)
{
    return (index >= 0 && index < lib.count) ? &lib.animations[index] : NULL;
}

int LIL3D_GetBoneIndex(LIL3D_Skeleton skeleton, const char* name)
{
    if (name == NULL) return -1;
    for (int i = 0; i < skeleton.boneCount; i++)
        if (strncmp(skeleton.bones[i].name, name, sizeof(skeleton.bones[i].name)) == 0) return i;
    return -1;
}

/* ---- sampling / blending ---- */

static Transform lil3d_lerp_transform(Transform a, Transform b, float t)
{
    Quaternion qa = a.rotation, qb = b.rotation;
    if (qa.x*qb.x + qa.y*qb.y + qa.z*qb.z + qa.w*qb.w < 0.0f) qb = (Quaternion){ -qb.x, -qb.y, -qb.z, -qb.w };
    Transform r;
    r.translation = Vector3Lerp(a.translation, b.translation, t);
    r.scale = Vector3Lerp(a.scale, b.scale, t);
    r.rotation = QuaternionNormalize(QuaternionLerp(qa, qb, t));
    return r;
}

/* Samples the clip at 'time' seconds, interpolating between frames at the clip's own FPS. */
static void lil3d_sample_animation(const LIL3D_Animation* a, float time, bool loop, Transform* out, int bones)
{
    int fc = a->frameCount, i0 = 0, i1 = 0;
    float k = 0.0f;
    if (fc > 1)
    {
        float f = time*a->fps;
        if (loop)
        {
            f = fmodf(f, (float)fc);
            if (f < 0.0f) f += (float)fc;
            i0 = LIL3D_MIN((int)f, fc - 1);
            i1 = (i0 + 1)%fc;
        }
        else
        {
            f = Clamp(f, 0.0f, (float)(fc - 1));
            i0 = LIL3D_MIN((int)f, fc - 1);
            i1 = LIL3D_MIN(i0 + 1, fc - 1);
        }
        k = f - (float)i0;
    }
    for (int b = 0; b < bones; b++) out[b] = lil3d_lerp_transform(a->framePoses[i0][b], a->framePoses[i1][b], k);
}

/* ---- skinned meshes (CPU skinning, works on every GL flavour) ---- */

static bool lil3d_make_skin_mesh(const Mesh* src, LIL3D_Mesh* out)
{
    memset(out, 0, sizeof(*out));
    if (src->vertices == NULL || src->boneIndices == NULL || src->boneWeights == NULL || src->vertexCount <= 0) return false;

    const int vc = src->vertexCount;
    Mesh m = { 0 };
    m.vertexCount = vc;
    m.triangleCount = src->triangleCount;
    m.vertices = (float*)MemAlloc((unsigned int)(vc*3*(int)sizeof(float)));
    if (src->normals != NULL)   m.normals   = (float*)MemAlloc((unsigned int)(vc*3*(int)sizeof(float)));
    if (src->texcoords != NULL) m.texcoords = (float*)MemAlloc((unsigned int)(vc*2*(int)sizeof(float)));
    if (src->indices != NULL)   m.indices   = (unsigned short*)MemAlloc((unsigned int)(src->triangleCount*3*(int)sizeof(unsigned short)));
    if (m.vertices == NULL || (src->normals && !m.normals) || (src->texcoords && !m.texcoords) || (src->indices && !m.indices))
    {
        UnloadMesh(m);
        return false;
    }
    memcpy(m.vertices, src->vertices, (size_t)vc*3*sizeof(float));
    if (m.normals)   memcpy(m.normals, src->normals, (size_t)vc*3*sizeof(float));
    if (m.texcoords) memcpy(m.texcoords, src->texcoords, (size_t)vc*2*sizeof(float));
    if (m.indices)   memcpy(m.indices, src->indices, (size_t)src->triangleCount*3*sizeof(unsigned short));

    UploadMesh(&m, true);                       /* dynamic: rewritten every pose change */
    out->mesh = m;
    out->aabb = GetMeshBoundingBox(*src);
    return true;
}

static void lil3d_skin_mesh(const Mesh* src, Mesh* dst, const Matrix* sk, int boneCount)
{
    const int vc = src->vertexCount;
    const bool hasN = (src->normals != NULL && dst->normals != NULL);

    for (int i = 0; i < vc; i++)
    {
        const float x = src->vertices[i*3], y = src->vertices[i*3 + 1], z = src->vertices[i*3 + 2];
        float px = 0.0f, py = 0.0f, pz = 0.0f, nx = 0.0f, ny = 0.0f, nz = 0.0f, ws = 0.0f;
        float nsx = 0.0f, nsy = 0.0f, nsz = 0.0f;
        if (hasN) { nsx = src->normals[i*3]; nsy = src->normals[i*3 + 1]; nsz = src->normals[i*3 + 2]; }

        for (int j = 0; j < 4; j++)
        {
            float w = src->boneWeights[i*4 + j];
            int b = src->boneIndices[i*4 + j];
            if (w <= 0.0f || b >= boneCount) continue;
            const Matrix* m = &sk[b];
            px += w*(m->m0*x + m->m4*y + m->m8*z + m->m12);
            py += w*(m->m1*x + m->m5*y + m->m9*z + m->m13);
            pz += w*(m->m2*x + m->m6*y + m->m10*z + m->m14);
            if (hasN)
            {
                nx += w*(m->m0*nsx + m->m4*nsy + m->m8*nsz);
                ny += w*(m->m1*nsx + m->m5*nsy + m->m9*nsz);
                nz += w*(m->m2*nsx + m->m6*nsy + m->m10*nsz);
            }
            ws += w;
        }

        if (ws <= 1e-6f)                         /* unweighted vertex: keep the bind pose */
        {
            px = x; py = y; pz = z; nx = nsx; ny = nsy; nz = nsz; ws = 1.0f;
        }
        float inv = 1.0f/ws;
        dst->vertices[i*3] = px*inv; dst->vertices[i*3 + 1] = py*inv; dst->vertices[i*3 + 2] = pz*inv;
        if (hasN)
        {
            float len = sqrtf(nx*nx + ny*ny + nz*nz);
            float nl = (len > 1e-8f) ? 1.0f/len : 0.0f;
            dst->normals[i*3] = nx*nl; dst->normals[i*3 + 1] = ny*nl; dst->normals[i*3 + 2] = nz*nl;
        }
    }

    UpdateMeshBuffer(*dst, 0, dst->vertices, vc*3*(int)sizeof(float), 0);
    if (hasN) UpdateMeshBuffer(*dst, 2, dst->normals, vc*3*(int)sizeof(float), 0);
}

/* ---- player ---- */

bool LIL3D_IsAnimationPlayerValid(LIL3D_AnimationPlayer player)
{
    return player.states != NULL && player.pose != NULL && player.skin != NULL && player.stateCount > 0;
}

LIL3D_AnimationPlayer LIL3D_LoadAnimationPlayer(LIL3D_Model model, LIL3D_AnimationLib lib)
{
    LIL3D_AnimationPlayer p;
    memset(&p, 0, sizeof(p));

    if (model.skeleton.boneCount <= 0 || model.skeleton.invBind == NULL)
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "LIL3D_LoadAnimationPlayer(): the model has no skeleton");
        return p;
    }
    if (!LIL3D_IsAnimationLibValid(lib))
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "LIL3D_LoadAnimationPlayer(): invalid animation lib");
        return p;
    }

    const int nb = model.skeleton.boneCount;
    p.lib = lib;
    p.skeleton = model.skeleton;
    p.speed = 1.0f;
    p.stateCount = lib.count;
    p.states = (LIL3D_AnimationState*)calloc((size_t)lib.count, sizeof(LIL3D_AnimationState));
    p.pose = (Transform*)malloc((size_t)nb*2*sizeof(Transform));
    p.skin = (Matrix*)malloc((size_t)nb*sizeof(Matrix));
    p.skinMeshes = (LIL3D_Mesh*)calloc((size_t)(model.meshCount > 0 ? model.meshCount : 1), sizeof(LIL3D_Mesh));
    if (p.states == NULL || p.pose == NULL || p.skin == NULL || p.skinMeshes == NULL)
    {
        free(p.states); free(p.pose); free(p.skin); free(p.skinMeshes);
        memset(&p, 0, sizeof(p));
        return p;
    }
    p.scratch = p.pose + nb;
    for (int i = 0; i < lib.count; i++)
    {
        p.states[i].speed = 1.0f;
        p.states[i].loop = true;
    }
    for (int i = 0; i < nb; i++) { p.pose[i] = model.skeleton.bindPose[i]; p.skin[i] = MatrixIdentity(); }

    p.skinMeshCount = model.meshCount;
    for (int i = 0; i < model.meshCount; i++) lil3d_make_skin_mesh(&model.meshes[i].mesh, &p.skinMeshes[i]);
    p.dirty = true;
    return p;
}

void LIL3D_UnloadAnimationPlayer(LIL3D_AnimationPlayer player)
{
    for (int i = 0; i < player.skinMeshCount; i++)
        if (player.skinMeshes[i].mesh.vertexCount > 0) UnloadMesh(player.skinMeshes[i].mesh);
    free(player.skinMeshes);
    free(player.states);
    free(player.pose);          /* 'scratch' lives in the same block */
    free(player.skin);
}

void LIL3D_UpdateAnimationPlayer(LIL3D_AnimationPlayer* p, float dt)
{
    if (p == NULL || !LIL3D_IsAnimationPlayerValid(*p)) return;
    const int nb = p->skeleton.boneCount;

    /* 1) fades + time */
    for (int i = 0; i < p->stateCount; i++)
    {
        LIL3D_AnimationState* st = &p->states[i];
        const LIL3D_Animation* a = &p->lib.animations[i];

        if (st->fadeRate > 0.0f && st->weight != st->targetWeight)
        {
            float step = st->fadeRate*dt;
            if (fabsf(st->targetWeight - st->weight) <= step) st->weight = st->targetWeight;
            else st->weight += (st->targetWeight > st->weight) ? step : -step;
            if (st->weight <= 0.0f && st->targetWeight <= 0.0f)
            {
                st->weight = 0.0f;
                st->playing = false;
                st->fadeRate = 0.0f;
            }
        }

        if (st->playing && a->frameCount > 0 && a->fps > 0.0f)
        {
            st->time += dt*p->speed*st->speed;
            if (st->loop)
            {
                st->time = fmodf(st->time, a->duration);
                if (st->time < 0.0f) st->time += a->duration;
            }
            else
            {
                float last = (float)(a->frameCount - 1)/a->fps;
                if (st->time >= last) { st->time = last; st->playing = false; }
                if (st->time <= 0.0f) { st->time = 0.0f; st->playing = false; }
            }
        }
    }

    /* 2) blend */
    float total = 0.0f;
    int active = 0;
    for (int i = 0; i < p->stateCount; i++)
        if (p->states[i].weight > 0.0f && p->lib.animations[i].frameCount > 0) { total += p->states[i].weight; active++; }

    for (int b = 0; b < nb; b++) p->pose[b] = p->skeleton.bindPose[b];     /* bones not driven by any clip */
    if (active == 1)
    {
        for (int i = 0; i < p->stateCount; i++)
        {
            const LIL3D_Animation* a = &p->lib.animations[i];
            if (p->states[i].weight > 0.0f && a->frameCount > 0)
                lil3d_sample_animation(a, p->states[i].time, p->states[i].loop, p->pose, LIL3D_MIN(nb, a->boneCount));
        }
    }
    else if (active > 1)
    {
        for (int b = 0; b < nb; b++)
        {
            p->pose[b].translation = Vector3Zero();
            p->pose[b].scale = Vector3Zero();
            p->pose[b].rotation = (Quaternion){ 0.0f, 0.0f, 0.0f, 0.0f };
        }
        bool first = true;
        for (int i = 0; i < p->stateCount; i++)
        {
            const LIL3D_Animation* a = &p->lib.animations[i];
            if (!(p->states[i].weight > 0.0f && a->frameCount > 0)) continue;
            const float w = p->states[i].weight/total;
            for (int b = 0; b < nb; b++) p->scratch[b] = p->skeleton.bindPose[b];
            lil3d_sample_animation(a, p->states[i].time, p->states[i].loop, p->scratch, LIL3D_MIN(nb, a->boneCount));
            for (int b = 0; b < nb; b++)
            {
                Transform* o = &p->pose[b];
                Quaternion q = p->scratch[b].rotation;
                if (!first && o->rotation.x*q.x + o->rotation.y*q.y + o->rotation.z*q.z + o->rotation.w*q.w < 0.0f)
                    q = (Quaternion){ -q.x, -q.y, -q.z, -q.w };
                o->translation = Vector3Add(o->translation, Vector3Scale(p->scratch[b].translation, w));
                o->scale = Vector3Add(o->scale, Vector3Scale(p->scratch[b].scale, w));
                o->rotation = (Quaternion){ o->rotation.x + q.x*w, o->rotation.y + q.y*w, o->rotation.z + q.z*w, o->rotation.w + q.w*w };
            }
            first = false;
        }
        for (int b = 0; b < nb; b++) p->pose[b].rotation = QuaternionNormalize(p->pose[b].rotation);
    }

    /* 3) skin matrices (model-space pose relative to the bind pose) */
    for (int b = 0; b < nb; b++) p->skin[b] = MatrixMultiply(p->skeleton.invBind[b], lil3d_trs(p->pose[b]));
    p->dirty = true;
}

void LIL3D_PlayAnimation(LIL3D_AnimationPlayer* p, int animation, bool loop)
{
    if (p == NULL || !LIL3D_IsAnimationPlayerValid(*p) || animation < 0 || animation >= p->stateCount) return;
    for (int i = 0; i < p->stateCount; i++)
    {
        LIL3D_AnimationState* st = &p->states[i];
        st->fadeRate = 0.0f;
        if (i == animation)
        {
            st->time = 0.0f; st->playing = true; st->loop = loop;
            st->weight = 1.0f; st->targetWeight = 1.0f;
        }
        else { st->playing = false; st->weight = 0.0f; st->targetWeight = 0.0f; }
    }
}

void LIL3D_PlayAnimationByName(LIL3D_AnimationPlayer* p, const char* name, bool loop)
{
    if (p == NULL) return;
    int idx = LIL3D_GetAnimationIndex(p->lib, name);
    if (idx < 0) { TraceLog(LOG_WARNING, LIL3D_LOG_TAG "animation '%s' not found", name ? name : "(null)"); return; }
    LIL3D_PlayAnimation(p, idx, loop);
}

void LIL3D_CrossfadeAnimation(LIL3D_AnimationPlayer* p, int animation, bool loop, float seconds)
{
    if (p == NULL || !LIL3D_IsAnimationPlayerValid(*p) || animation < 0 || animation >= p->stateCount) return;
    if (seconds <= 0.0f) { LIL3D_PlayAnimation(p, animation, loop); return; }

    LIL3D_AnimationState* t = &p->states[animation];
    if (t->playing && t->targetWeight >= 1.0f && t->weight > 0.0f) return;     /* already the target */

    for (int i = 0; i < p->stateCount; i++)
    {
        LIL3D_AnimationState* st = &p->states[i];
        if (i == animation)
        {
            st->time = 0.0f; st->playing = true; st->loop = loop;
            st->targetWeight = 1.0f;
            st->fadeRate = (1.0f - st->weight)/seconds;     /* every fade ends together */
        }
        else if (st->playing || st->weight > 0.0f)
        {
            st->targetWeight = 0.0f;
            st->fadeRate = st->weight/seconds;
        }
    }
}

void LIL3D_StopAnimations(LIL3D_AnimationPlayer* p)
{
    if (p == NULL || p->states == NULL) return;
    for (int i = 0; i < p->stateCount; i++)
    {
        p->states[i].playing = false;
        p->states[i].weight = 0.0f;
        p->states[i].targetWeight = 0.0f;
        p->states[i].fadeRate = 0.0f;
    }
}

bool LIL3D_IsAnimationPlaying(const LIL3D_AnimationPlayer* p, int animation)
{
    return p != NULL && p->states != NULL && animation >= 0 && animation < p->stateCount && p->states[animation].playing;
}

Matrix LIL3D_GetBoneMatrix(const LIL3D_AnimationPlayer* p, int bone)
{
    if (p == NULL || p->pose == NULL || bone < 0 || bone >= p->skeleton.boneCount) return MatrixIdentity();
    return lil3d_trs(p->pose[bone]);
}

/* ---- animated draws ---- */

void LIL3D_DrawAnimatedModelPro(LIL3D_Model model, LIL3D_AnimationPlayer* p, Matrix transform)
{
    if (p == NULL || !LIL3D_IsAnimationPlayerValid(*p) || p->skinMeshCount != model.meshCount)
    {
        LIL3D_DrawModelPro(model, transform);       /* bind pose fallback */
        return;
    }
    if (!lil3d_ctx.initialized || !lil3d_ctx.inFrame)
    {
        TraceLog(LOG_WARNING, LIL3D_LOG_TAG "draw call outside LIL3D_Begin()/LIL3D_End(); ignored");
        return;
    }

    if (p->dirty)
    {
        for (int i = 0; i < model.meshCount; i++)
            if (p->skinMeshes[i].mesh.vertexCount > 0)
                lil3d_skin_mesh(&model.meshes[i].mesh, &p->skinMeshes[i].mesh, p->skin, p->skeleton.boneCount);
        p->dirty = false;
    }

    for (int i = 0; i < model.meshCount; i++)
    {
        const bool skinned = p->skinMeshes[i].mesh.vertexCount > 0;
        int before = lil3d_ctx.drawCount;
        lil3d_push_draw(skinned ? p->skinMeshes[i] : model.meshes[i], *lil3d_model_material(&model, i), transform, NULL, 0);
        /* Posed geometry can leave the bind-pose AABB: never cull skinned casters. */
        if (skinned && lil3d_ctx.drawCount > before) lil3d_ctx.draws[before].hasBounds = false;
    }
}

void LIL3D_DrawAnimatedModel(LIL3D_Model model, LIL3D_AnimationPlayer* player, Vector3 position, float scale)
{
    LIL3D_DrawAnimatedModelPro(model, player, MatrixMultiply(MatrixScale(scale, scale, scale), MatrixTranslate(position.x, position.y, position.z)));
}

void LIL3D_DrawAnimatedModelEx(LIL3D_Model model, LIL3D_AnimationPlayer* player, Vector3 position, Vector3 rotationAxis, float rotationAngle, Vector3 scale)
{
    Matrix t = MatrixMultiply(MatrixMultiply(MatrixScale(scale.x, scale.y, scale.z), MatrixRotate(rotationAxis, rotationAngle*DEG2RAD)),
                              MatrixTranslate(position.x, position.y, position.z));
    LIL3D_DrawAnimatedModelPro(model, player, t);
}

/* -------------------------------------------------------------------------- */
/* Debug                                                                      */
/* -------------------------------------------------------------------------- */

static void lil3d_debug_circle(Vector3 c, Vector3 u, Vector3 v, float r, Color col)
{
    const int seg = 32;
    Vector3 prev = Vector3Add(c, Vector3Scale(u, r));
    for (int i = 1; i <= seg; i++)
    {
        float a = 2.0f*PI*(float)i/(float)seg;
        Vector3 p = Vector3Add(c, Vector3Add(Vector3Scale(u, cosf(a)*r), Vector3Scale(v, sinf(a)*r)));
        DrawLine3D(prev, p, col);
        prev = p;
    }
}

void LIL3D_DrawLightShape(LIL3D_Light light)
{
    const lil3d_light* l = lil3d_get_light(light);
    if (l == NULL) return;

    Color col = l->active ? l->color : Fade(l->color, 0.35f);
    Color dim = Fade(col, 0.45f);
    const Vector3 pos = l->position;
    const Vector3 X = { 1, 0, 0 }, Y = { 0, 1, 0 }, Z = { 0, 0, 1 };

    switch (l->type)
    {
        case LIL3D_LIGHT_OMNI:
        {
            lil3d_debug_circle(pos, X, Y, l->range, col);
            lil3d_debug_circle(pos, X, Z, l->range, col);
            lil3d_debug_circle(pos, Y, Z, l->range, col);
            float k = 0.15f;                                    /* small source cross */
            DrawLine3D(Vector3Subtract(pos, Vector3Scale(X, k)), Vector3Add(pos, Vector3Scale(X, k)), col);
            DrawLine3D(Vector3Subtract(pos, Vector3Scale(Y, k)), Vector3Add(pos, Vector3Scale(Y, k)), col);
            DrawLine3D(Vector3Subtract(pos, Vector3Scale(Z, k)), Vector3Add(pos, Vector3Scale(Z, k)), col);
        } break;

        case LIL3D_LIGHT_SPOT:
        {
            Vector3 d = lil3d_safe_normalize(l->direction, (Vector3){ 0.0f, -1.0f, 0.0f });
            Vector3 up = lil3d_pick_up(d);
            Vector3 right = Vector3Normalize(Vector3CrossProduct(d, up));
            Vector3 upv = Vector3CrossProduct(right, d);

            Vector3 base = Vector3Add(pos, Vector3Scale(d, l->range));
            float ro = l->range*tanf(LIL3D_MIN(l->outerCutOff, 75.0f)*DEG2RAD);
            float ri = l->range*tanf(LIL3D_MIN(l->innerCutOff, 75.0f)*DEG2RAD);

            lil3d_debug_circle(base, right, upv, ro, col);
            if (ri > 0.001f) lil3d_debug_circle(base, right, upv, ri, dim);
            DrawLine3D(pos, base, dim);
            Vector3 edge[4] = { right, Vector3Negate(right), upv, Vector3Negate(upv) };
            for (int i = 0; i < 4; i++) DrawLine3D(pos, Vector3Add(base, Vector3Scale(edge[i], ro)), col);
        } break;

        default:    /* LIL3D_LIGHT_DIR */
        {
            Vector3 d = lil3d_safe_normalize(l->direction, (Vector3){ 0.0f, -1.0f, 0.0f });
            Vector3 up = lil3d_pick_up(d);
            Vector3 right = Vector3Normalize(Vector3CrossProduct(d, up));
            Vector3 upv = Vector3CrossProduct(right, d);
            const float len = 2.0f, ring = 0.6f, head = 0.25f;

            lil3d_debug_circle(pos, right, upv, ring, dim);
            Vector3 offs[5] = { Vector3Zero(), Vector3Scale(right, ring), Vector3Scale(right, -ring), Vector3Scale(upv, ring), Vector3Scale(upv, -ring) };
            for (int i = 0; i < 5; i++)
            {
                Vector3 a = Vector3Add(pos, offs[i]);
                Vector3 b = Vector3Add(a, Vector3Scale(d, len));
                DrawLine3D(a, b, col);
                Vector3 hb = Vector3Subtract(b, Vector3Scale(d, head));
                DrawLine3D(b, Vector3Add(hb, Vector3Scale(right, head*0.5f)), col);
                DrawLine3D(b, Vector3Subtract(hb, Vector3Scale(right, head*0.5f)), col);
                DrawLine3D(b, Vector3Add(hb, Vector3Scale(upv, head*0.5f)), col);
                DrawLine3D(b, Vector3Subtract(hb, Vector3Scale(upv, head*0.5f)), col);
            }
        } break;
    }
}

#endif /* LIL3D_IMPLEMENTATION */