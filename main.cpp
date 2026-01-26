#include "mem.h"
#include <iostream>
#include <Windows.h>
#include <d3d9.h>
#include <immintrin.h> // For __m128i and SSE instructions
#include "overlay.h"

Kmem mem{};

// ==== OFFSETS ====
// Base / globals
#define UWorld 0x0DC77CB8

// UWorld
#define PersistentLevel 0xf8 
#define GameInstance 0xC0
#define GameState 0x80
#define ActorsList 0x108

// Level
#define ActorsCount (ActorsList + sizeof(uintptr_t))

// GameInstance
#define LocalPlayers 0xF8

// LocalPlayer
#define PlayerController 0xA0

// PlayerController
#define AcknowledgedPawn 0x3E0

// Pawn / Actor
#define Mesh 0x428
#define PlayerName 0x438
#define Super 0xB0

inline bool ValidPtr(uintptr_t ptr)
{
    return ptr && ptr > 0x10000 && ptr < 0x7FFFFFFFFFFF;
}

// ==== FUNCTIONS ====
std::uintptr_t GetBoneArrayDecrypt(uintptr_t GameBase, std::uintptr_t Meh)
{
    auto xmmword_7FF6B9FF4B00 = mem.read<__m128i>(
        GameBase + (0x7FF6B9FF4B00 - 0x7FF6AF650000)
    );

    auto BonesMesh = mem.read<__m128i>(Meh + 0x7B0);
    auto v8 = _mm_shuffle_epi8(BonesMesh, xmmword_7FF6B9FF4B00);
    v8.m128i_i64[0] = _mm_or_si128(_mm_slli_epi32(v8, 0x11u), _mm_srli_epi32(v8, 0xFu)).m128i_u64[0];
    auto v9 = (v8.m128i_i64[0] ^ 0x4834C6DEA02581C7i64) + 16i64 * mem.read<int>(Meh + 0x7FC);

    auto BoneArray = mem.read<uintptr_t>(v9 + 0x88);

    if (!ValidPtr(BoneArray))
        BoneArray = mem.read<uintptr_t>(v9 + 0x98);

    return BoneArray;
}

inline uint64_t GetGameInstance(uintptr_t GameBase, uint64_t uworldAddr)
{
    auto tmp1 = mem.read<__m128i>(uworldAddr + 0x2A0);
    auto tmp2 = mem.read<__m128i>(GameBase + 0xAF231F0);

    auto v5 = _mm_shuffle_epi8(tmp1, tmp2);
    auto v6 = _mm_or_si128(_mm_slli_epi16(v5, 2u), _mm_srli_epi16(v5, 0xEu)).m128i_u64[0];

    constexpr uint64_t kConst = 0x1554577E835E9F4ULL;

    uint64_t gameInstance;
    if (v6 == kConst)
    {
        gameInstance = uworldAddr;
    }
    else
    {
        gameInstance = v6 ^ kConst;
    }

    return gameInstance;
}

struct vec3 {
    double x, y, z;
    vec3() : x(0), y(0), z(0) {}
    vec3(double X, double Y, double Z) : x(X), y(Y), z(Z) {}

    vec3 operator+(const vec3& rhs) const {
        return vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    vec3 operator-(const vec3& rhs) const {
        return vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    vec3 operator*(double scalar) const {
        return vec3(x * scalar, y * scalar, z * scalar);
    }

    vec3 operator/(double scalar) const {
        return vec3(x / scalar, y / scalar, z / scalar);
    }

    bool operator==(const vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    inline double dot(vec3 v) { return x * v.x + y * v.y + z * v.z; }
};

namespace BoneIndex {
    constexpr int Root = 0;
    constexpr int Pelvis = 1;
    constexpr int Spine01 = 2;
    constexpr int Spine02 = 3;
    constexpr int Spine03 = 4;
    constexpr int Chest = 5;
    constexpr int Neck = 6;
    constexpr int Head = 7;
    constexpr int ClavicleL = 8;
    constexpr int UpperArmL = 9;
    constexpr int LowerArmL = 10;
    constexpr int HandL = 11;
    constexpr int ThumbL01 = 12;
    constexpr int ThumbL02 = 13;
    constexpr int ThumbL03 = 14;
    constexpr int IndexL01 = 15;
    constexpr int IndexL02 = 16;
    constexpr int IndexL03 = 17;
    constexpr int MiddleL01 = 18;
    constexpr int MiddleL02 = 19;
    constexpr int MiddleL03 = 20;
    constexpr int RingL01 = 21;
    constexpr int RingL02 = 22;
    constexpr int RingL03 = 23;
    constexpr int PinkyL01 = 24;
    constexpr int PinkyL02 = 25;
    constexpr int PinkyL03 = 26;
    constexpr int ClavicleR = 42;
    constexpr int UpperArmR = 43;
    constexpr int LowerArmR = 44;
    constexpr int HandR = 45;
    constexpr int ThumbR01 = 46;
    constexpr int ThumbR02 = 47;
    constexpr int ThumbR03 = 48;
    constexpr int IndexR01 = 49;
    constexpr int IndexR02 = 50;
    constexpr int IndexR03 = 51;
    constexpr int MiddleR01 = 52;
    constexpr int MiddleR02 = 53;
    constexpr int MiddleR03 = 54;
    constexpr int RingR01 = 55;
    constexpr int RingR02 = 56;
    constexpr int RingR03 = 57;
    constexpr int PinkyR01 = 58;
    constexpr int PinkyR02 = 59;
    constexpr int PinkyR03 = 60;
    constexpr int ThighL = 65;
    constexpr int CalfL = 66;
    constexpr int FootL = 67;
    constexpr int ToeL = 68;
    constexpr int ThighR = 69;
    constexpr int CalfR = 70;
    constexpr int FootR = 71;
    constexpr int ToeR = 72;
}
inline constexpr std::pair<int, int> BoneConnections[] = {
    // Spine chain
    {BoneIndex::Root, BoneIndex::Pelvis},
    {BoneIndex::Pelvis, BoneIndex::Spine01},
    {BoneIndex::Spine01, BoneIndex::Spine02},
    {BoneIndex::Spine02, BoneIndex::Spine03},
    {BoneIndex::Spine03, BoneIndex::Chest},
    {BoneIndex::Chest, BoneIndex::Neck},
    {BoneIndex::Neck, BoneIndex::Head},

    // Left arm chain
    {BoneIndex::Chest, BoneIndex::ClavicleL},
    {BoneIndex::ClavicleL, BoneIndex::UpperArmL},
    {BoneIndex::UpperArmL, BoneIndex::LowerArmL},
    {BoneIndex::LowerArmL, BoneIndex::HandL},

    // Left hand fingers

    // Right arm chain
    {BoneIndex::Chest, BoneIndex::ClavicleR},
    {BoneIndex::ClavicleR, BoneIndex::UpperArmR},
    {BoneIndex::UpperArmR, BoneIndex::LowerArmR},
    {BoneIndex::LowerArmR, BoneIndex::HandR},

    // Right hand fingers

    // Left leg chain
    {BoneIndex::Pelvis, BoneIndex::ThighL},
    {BoneIndex::ThighL, BoneIndex::CalfL},
    {BoneIndex::CalfL, BoneIndex::FootL},

    // Right leg chain
    {BoneIndex::Pelvis, BoneIndex::ThighR},
    {BoneIndex::ThighR, BoneIndex::CalfR},
    {BoneIndex::CalfR, BoneIndex::FootR},
};

struct Quat { double x, y, z, w; };

struct Transform
{
    Quat rot;
    vec3 translation;
    char pad[4];
    vec3 scale;
    D3DMATRIX to_matrix_with_scale();
};
#define M_PI 3.14159265358979323846

D3DMATRIX Transform::to_matrix_with_scale()
{
    D3DMATRIX m{};
    m._41 = translation.x;
    m._42 = translation.y;
    m._43 = translation.z;
    double x2 = rot.x + rot.x;
    double y2 = rot.y + rot.y;
    double z2 = rot.z + rot.z;
    double xx2 = rot.x * x2;
    double yy2 = rot.y * y2;
    double zz2 = rot.z * z2;
    m._11 = (1.0f - (yy2 + zz2)) * scale.x;
    m._22 = (1.0f - (xx2 + zz2)) * scale.y;
    m._33 = (1.0f - (xx2 + yy2)) * scale.z;
    double yz2 = rot.y * z2;
    double wx2 = rot.w * x2;
    m._32 = (yz2 - wx2) * scale.z;
    m._23 = (yz2 + wx2) * scale.y;
    double xy2 = rot.x * y2;
    double wz2 = rot.w * z2;
    m._21 = (xy2 - wz2) * scale.y;
    m._12 = (xy2 + wz2) * scale.x;
    double xz2 = rot.x * z2;
    double wy2 = rot.w * y2;
    m._31 = (xz2 + wy2) * scale.z;
    m._13 = (xz2 - wy2) * scale.x;
    m._14 = 0.0f;
    m._24 = 0.0f;
    m._34 = 0.0f;
    m._44 = 1.0f;
    return m;
}

D3DMATRIX matrix_multiplication(D3DMATRIX pm1, D3DMATRIX pm2)
{
    D3DMATRIX pout{};
    pout._11 = pm1._11 * pm2._11 + pm1._12 * pm2._21 + pm1._13 * pm2._31 + pm1._14 * pm2._41;
    pout._12 = pm1._11 * pm2._12 + pm1._12 * pm2._22 + pm1._13 * pm2._32 + pm1._14 * pm2._42;
    pout._13 = pm1._11 * pm2._13 + pm1._12 * pm2._23 + pm1._13 * pm2._33 + pm1._14 * pm2._43;
    pout._14 = pm1._11 * pm2._14 + pm1._12 * pm2._24 + pm1._13 * pm2._34 + pm1._14 * pm2._44;
    pout._21 = pm1._21 * pm2._11 + pm1._22 * pm2._21 + pm1._23 * pm2._31 + pm1._24 * pm2._41;
    pout._22 = pm1._21 * pm2._12 + pm1._22 * pm2._22 + pm1._23 * pm2._32 + pm1._24 * pm2._42;
    pout._23 = pm1._21 * pm2._13 + pm1._22 * pm2._23 + pm1._23 * pm2._33 + pm1._24 * pm2._43;
    pout._24 = pm1._21 * pm2._14 + pm1._22 * pm2._24 + pm1._23 * pm2._34 + pm1._24 * pm2._44;
    pout._31 = pm1._31 * pm2._11 + pm1._32 * pm2._21 + pm1._33 * pm2._31 + pm1._34 * pm2._41;
    pout._32 = pm1._31 * pm2._12 + pm1._32 * pm2._22 + pm1._33 * pm2._32 + pm1._34 * pm2._42;
    pout._33 = pm1._31 * pm2._13 + pm1._32 * pm2._23 + pm1._33 * pm2._33 + pm1._34 * pm2._43;
    pout._34 = pm1._31 * pm2._14 + pm1._32 * pm2._24 + pm1._33 * pm2._34 + pm1._34 * pm2._44;
    pout._41 = pm1._41 * pm2._11 + pm1._42 * pm2._21 + pm1._43 * pm2._31 + pm1._44 * pm2._41;
    pout._42 = pm1._41 * pm2._12 + pm1._42 * pm2._22 + pm1._43 * pm2._32 + pm1._44 * pm2._42;
    pout._43 = pm1._41 * pm2._13 + pm1._42 * pm2._23 + pm1._43 * pm2._33 + pm1._44 * pm2._43;
    pout._44 = pm1._41 * pm2._14 + pm1._42 * pm2._24 + pm1._43 * pm2._34 + pm1._44 * pm2._44;
    return pout;
}


D3DMATRIX to_matrix(vec3 rot, vec3 origin)
{
    double radpitch = (rot.x * M_PI / 180);
    double radyaw = (rot.y * M_PI / 180);
    double radroll = (rot.z * M_PI / 180);
    double sp = sin(radpitch);
    double cp = cos(radpitch);
    double sy = sin(radyaw);
    double cy = cos(radyaw);
    double sr = sin(radroll);
    double cr = cos(radroll);
    D3DMATRIX matrix{};
    matrix.m[0][0] = cp * cy;
    matrix.m[0][1] = cp * sy;
    matrix.m[0][2] = sp;
    matrix.m[0][3] = 0.f;
    matrix.m[1][0] = sr * sp * cy - cr * sy;
    matrix.m[1][1] = sr * sp * sy + cr * cy;
    matrix.m[1][2] = -sr * cp;
    matrix.m[1][3] = 0.f;
    matrix.m[2][0] = -(cr * sp * cy + sr * sy);
    matrix.m[2][1] = cy * sr - cr * sp * sy;
    matrix.m[2][2] = cr * cp;
    matrix.m[2][3] = 0.f;
    matrix.m[3][0] = origin.x;
    matrix.m[3][1] = origin.y;
    matrix.m[3][2] = origin.z;
    matrix.m[3][3] = 1.f;
    return matrix;
}


vec3 get_bone(int boner, uintptr_t bone_array, Transform component_to_world)
{
    Transform bone = mem.read<Transform>(bone_array + (boner * 0x60));
    D3DMATRIX matrix = matrix_multiplication(bone.to_matrix_with_scale(), component_to_world.to_matrix_with_scale());

    return vec3(matrix._41, matrix._42, matrix._43);
}

void draw_cornered_box(double x, double y, double w, double h, const ImColor color, double thickness)
{
    double outline = thickness + 2;
    ImColor black = ImColor(0, 0, 0, 255);

    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), black, outline);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), black, outline);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), black, outline);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), black, outline);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), black, outline);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), black, outline);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), black, outline);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), black, outline);

    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}

struct vec2 {
    double x, y;
    vec2() : x(0), y(0) {}
    vec2(double X, double Y) : x(X), y(Y) {}
};

struct FminimalViewInfo
{
    vec3 Location{};
    vec3 Rotation{};
    float FOV;
};

D3DMATRIX rotation_matrix(vec3 rot)
{
    double pitch = rot.x * M_PI / 180.f;
    double yaw = rot.y * M_PI / 180.f;
    double roll = rot.z * M_PI / 180.f;

    double sp = sin(pitch), cp = cos(pitch);
    double sy = sin(yaw), cy = cos(yaw);
    double sr = sin(roll), cr = cos(roll);

    D3DMATRIX m{};
    m.m[0][0] = cp * cy;
    m.m[0][1] = cp * sy;
    m.m[0][2] = sp;

    m.m[1][0] = sr * sp * cy - cr * sy;
    m.m[1][1] = sr * sp * sy + cr * cy;
    m.m[1][2] = -sr * cp;

    m.m[2][0] = -(cr * sp * cy + sr * sy);
    m.m[2][1] = cy * sr - cr * sp * sy;
    m.m[2][2] = cr * cp;

    return m;
}

uintptr_t GetCameraManagerFromActors(uintptr_t gworld, uintptr_t player_controller)
{
    static uintptr_t cached_gworld = 0;
    static uintptr_t cached_player_controller = 0;
    static uintptr_t cached_camera_manager = 0;
    static bool logged_search_start = false;

    // Reset cache if inputs changed
    if (cached_gworld != gworld || cached_player_controller != player_controller) {
        cached_gworld = gworld;
        cached_player_controller = player_controller;
        cached_camera_manager = 0;
        logged_search_start = false;
    }

    // Return cached result if valid
    if (cached_camera_manager != 0) {
        return cached_camera_manager;
    }

    if (!logged_search_start) {
      //  printf("[CAMERA] Searching for CameraManager...\n");
        logged_search_start = true;
    }

    if (!gworld || !player_controller) {
        return 0;
    }

    // Get persistent level
    uintptr_t persistent_level = mem.read<uintptr_t>(gworld + PersistentLevel);
    if (!persistent_level) return 0;

    // Get actor array
    uintptr_t actors_ptr = mem.read<uintptr_t>(persistent_level + ActorsList);
    int actor_count = mem.read<int>(persistent_level + (ActorsList + sizeof(uintptr_t)));

    if (!actors_ptr || actor_count == 0) return 0;

    // Get local pawn for compariso
    uintptr_t local_pawn = mem.read<uintptr_t>(player_controller + AcknowledgedPawn);

    // Iterate through actors to find camera manager
    for (int i = 0; i < actor_count; i++) {
        uintptr_t actor = mem.read<uintptr_t>(actors_ptr + (i * sizeof(uintptr_t)));
        if (!actor) continue;

        // Check ViewTarget.Target offset (typically 0x1A0 + 0x8)
        uintptr_t view_target_target = mem.read<uintptr_t>(actor + 0xcc0);

        if (view_target_target == player_controller ||
            (local_pawn && view_target_target == local_pawn)) {
            cached_camera_manager = actor;
            //printf("[CAMERA] Found CameraManager: 0x%llX\n", actor);
            return actor;
        }
    }

    return 0;
}

vec2 w2s(vec3 world_location, const FminimalViewInfo & cam_info, int screen_width, int screen_height) {
    // Calculate relative position
    vec3 delta = world_location - cam_info.Location;

    // Convert rotation to radians
    double yaw = cam_info.Rotation.y * M_PI / 180.0;
    double pitch = cam_info.Rotation.x * M_PI / 180.0;

    // Calculate camera basis vectors (Unreal uses Z-up, left-handed)
    vec3 forward = vec3(
        cos(pitch) * cos(yaw),
        cos(pitch) * sin(yaw),
        sin(pitch)
    );

    vec3 right = vec3(
        -sin(yaw),
        cos(yaw),
        0
    );

    vec3 up = vec3(
        -sin(pitch) * cos(yaw),
        -sin(pitch) * sin(yaw),
        cos(pitch)
    );

    // Transform to camera space
    double cam_x = delta.dot(right);
    double cam_y = delta.dot(up);
    double cam_z = delta.dot(forward);

    // Check if behind camera
    if (cam_z < 1.0) return vec2(-9999, -9999);

    // Calculate aspect ratio
    double aspect_ratio = (double)screen_width / (double)screen_height;

    // Convert FOV to radians and calculate projection
    double fov_rad = cam_info.FOV * M_PI / 180.0;
    double tan_half_fov = tan(fov_rad / 2.0);

    // Project to normalized device coordinates
    double screen_x = (screen_width / 2.0) + (cam_x / (cam_z * tan_half_fov * aspect_ratio)) * (screen_width / 2.0);
    double screen_y = (screen_height / 2.0) - (cam_y / (cam_z * tan_half_fov)) * (screen_height / 2.0);

    return vec2(screen_x, screen_y);
}



int screen_width = GetSystemMetrics(SM_CXSCREEN);
int screen_height = GetSystemMetrics(SM_CYSCREEN);

// ==== MAIN LOOP ====
int main()
{
    mem.setup(L"PioneerGame.exe");
    uintptr_t base = mem.GetBase();


    bool WindowFocus = false;
    while (!WindowFocus) {
        DWORD ForegroundWindowProcessID;
        GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
        if (mem.Pid() == ForegroundWindowProcessID) {
            WindowFocus = true;
        }
    }


    Overlay ovr{};

    if (!ovr.Initialize())
        return -67;


    while (true)
    {
        // --- UWorld ---
        static RECT oldRect = { 0 };
        ZeroMemory(&ovr.dx9.message, sizeof(MSG));

        while (ovr.dx9.message.message != WM_QUIT)
        {
            if (PeekMessage(&ovr.dx9.message,
                ovr.window.hWnd, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&ovr.dx9.message);
                DispatchMessage(&ovr.dx9.message);
            }

            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

            // Get screen dimensions
            int screen_width = GetSystemMetrics(SM_CXSCREEN);
            int screen_height = GetSystemMetrics(SM_CYSCREEN);

            // Text to draw
            const char* text = "hi";

            // Calculate text size
            ImVec2 text_size = ImGui::CalcTextSize(text);

            // Calculate center position
            double pos_x = (screen_width - text_size.x) / 2.0f;
            double pos_y = (screen_height - text_size.y) / 2.0f;

            // Draw the text
            draw_list->AddText(
                ImVec2(pos_x, pos_y),
                IM_COL32(255, 255, 255, 255), // White color
                text
            );


            uintptr_t uworld_ptr = mem.read<uintptr_t>(base + UWorld);
            if (!uworld_ptr) { Sleep(1000); continue; }

            uintptr_t uworld = mem.read<uintptr_t>(uworld_ptr + 0xC0);
           // printf("UWorld: 0x%llX\n", uworld);

            // --- GameInstance ---
            uintptr_t game_instance = GetGameInstance(base, uworld);
//printf("GameInstance: 0x%llX\n", game_instance);

            // --- Persistent Level ---
            uintptr_t ulevel = mem.read<uintptr_t>(uworld + PersistentLevel);
           // printf("Level: 0x%llX\n", ulevel);

            // --- GameState ---
            uintptr_t game_state = mem.read<uintptr_t>(uworld + GameState);
           //printf("GameState: 0x%llX\n", game_state);

            // --- Actor Array ---
            uintptr_t actors = mem.read<uintptr_t>(ulevel + ActorsList);
            int actor_count = mem.read<int>(ulevel + (0x108 + sizeof(uintptr_t)));
            //  printf("Actors: %d\n", actor_count);

            // --- Local Player ---
            uintptr_t local_players = mem.read<uintptr_t>(game_instance + LocalPlayers);
            uintptr_t local_player = mem.read<uintptr_t>(local_players);
            //printf("Local Player: 0x%llX\n", local_player);

            uintptr_t player_controller = mem.read<uintptr_t>(local_player + PlayerController);
            uintptr_t local_pawn = mem.read<uintptr_t>(player_controller + AcknowledgedPawn);
         //   printf("Local Pawn: 0x%llX\n", local_pawn);

            uintptr_t camera_manager = GetCameraManagerFromActors(uworld, player_controller);
           // std::cout << "cam manager: " << camera_manager << std::endl;

            FminimalViewInfo camera_info = mem.read<FminimalViewInfo>(camera_manager + 0xcc0 + 0x10);

        // printf("Camera Location: x=%.2f, y=%.2f, z=%.2f\n",
             //  camera_info.Location.x, camera_info.Location.y, camera_info.Location.z);
        //  printf("Camera Rotation: pitch=%.2f, yaw=%.2f, roll=%.2f\n",
           //    camera_info.Rotation.x, camera_info.Rotation.y, camera_info.Rotation.z);
         // printf("Camera FOV: %.2f\n", camera_info.FOV);

            // --- Loop through actors ---
            for (int i = 0; i < actor_count; i++)
            {
                uintptr_t actor = mem.read<uintptr_t>(actors + (i * sizeof(uintptr_t)));

                //if (actor == local_pawn)
                 //   continue;

                //std::cout << "Valid Player Actor: " << std::hex << actor << std::endl;

                // For player pawns, mesh is at the Mesh component offset
                uintptr_t meh = mem.read<uintptr_t>(actor + 0x428); // Verify this offset for player pawns

                if (!meh || !ValidPtr(meh)) continue;

               // std::cout << "mesh: " << std::hex << meh << std::endl;

                uintptr_t bone_array = GetBoneArrayDecrypt(base, meh);
               // std::cout << "bone_array: " << std::hex << bone_array << std::endl;

                if (!bone_array || !ValidPtr(bone_array)) continue;

                Transform c2w = mem.read<Transform>(meh + 0x320);

                vec3 bone = get_bone(0, bone_array, c2w);

                vec3 head = bone;
                head.z += 200.f;

                vec2 head2d = w2s(head, camera_info, screen_width, screen_height);
                vec2 root2d = w2s(bone, camera_info, screen_width, screen_height);

                double box_height = std::abs(head2d.y - root2d.y);
                double box_width = box_height / 2.0f;
                double box_x = root2d.x - (box_width / 2.0f);
                double box_y = root2d.y - (box_height * 0.9f);

                draw_cornered_box(box_x, box_y, box_width, box_height, ImColor(188, 196, 247), 1.5f);

                for (auto [from, to] : BoneConnections)
                {
                    vec3 from3d = get_bone(from, bone_array, c2w);
                    vec3 to3d = get_bone(to, bone_array, c2w);

                    vec2 from2d = w2s(from3d, camera_info, screen_width, screen_height);
                    vec2 to2d = w2s(to3d, camera_info, screen_width, screen_height);

                    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(from2d.x, from2d.y), ImVec2(to2d.x, to2d.y), ImColor(255, 255, 255));
                }
            }

             ImGui::EndFrame();

            ovr.dx9.device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
            if (ovr.dx9.device->BeginScene() >= 0)
            {
                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
                ovr.dx9.device->EndScene();
            }

            HRESULT result = ovr.dx9.device->Present(NULL, NULL, NULL, NULL);
            if (result == D3DERR_DEVICELOST &&
                ovr.dx9.device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            {
                ImGui_ImplDX9_InvalidateDeviceObjects();
                ovr.dx9.device->Reset(&ovr.dx9.params);
                ImGui_ImplDX9_CreateDeviceObjects();
            }
        }
    }

    return 0;
}