#include <iostream>
#include "mem.h"
#include "overlay.h"
#include <unordered_map>
Kmem mem{};
#define M_PI 3.14159265358979323846f
#define M_RADPI	57.295779513082f
#define M_PI_F ((float)(M_PI))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))
#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))
#define atan2(a, b) ((float)atan2((double)(a), (double)(b)))

uint64_t il2cpp_get_handle(uint64_t game_assembly, uint64_t handle)
{
    // 0xd4c3cf0
    uint64_t rdi_1 = (uint64_t)((int32_t)handle >> 3);
    uint64_t rcx_1 = (uint64_t)(((int32_t)handle & 7) - 1);
    uintptr_t ObjectArray = mem.read<uintptr_t>((rcx_1 * 0x28) + (game_assembly + 0xd4c3cf0 + 0x8)) + (rdi_1 << 3);
    if (mem.read<uint8_t>((rcx_1 * 0x28) + (game_assembly + 0xd4c3cf0 + 0x14)) > 1)
        return mem.read<uintptr_t>(ObjectArray);
    else 
    {
        uint32_t eax = mem.read<uint32_t>(ObjectArray);
        eax = ~eax;
        return eax;
    }
}

std::uintptr_t client_entities(std::uintptr_t a1, uint64_t game_assembly) {
    std::uint32_t r8d = 0, eax = 0, ecx = 0, edx = 0; // initialize r8d, eax, ecx, edx
    std::uintptr_t rax = mem.read<std::uintptr_t >(a1 + 0x18); // rax = *(_QWORD *)(a1 + 0x18)
    std::uintptr_t* rdx = &rax; // rdx points to rax
    // unhandled: push rbx
    r8d = 0x02; // mov r8d, 0x02
    do { // start of loop
        ecx = *(std::uint32_t*)(rdx); // mov ecx, [rdx]
        eax = *(std::uint32_t*)(rdx); // mov eax, [rdx]
        rdx = (std::uintptr_t*)((char*)rdx + 0x04); // lea rdx, [rdx+0x04]
        eax <<= 0x0F; // shl eax, 0x0F
        ecx >>= 0x11; // shr ecx, 0x11
        ecx |= eax; // or ecx, eax
        ecx += 0xE5766956; // add ecx, 0xE5766956
        eax = ecx; // mov eax, ecx
        ecx <<= 0x06; // shl ecx, 0x06
        eax >>= 0x1A; // shr eax, 0x1A
        eax |= ecx; // or eax, ecx
        *((std::uint32_t*)rdx - 1) = eax; // mov [rdx-0x04], eax
        r8d -= 0x01; // sub r8d, 0x01
    } while (r8d); // end of loop

    return il2cpp_get_handle(game_assembly, rax); // return il2cpp handle
}
// decrypt block auto-generated @AMDdaam // function: entity_list
std::uintptr_t get_entity_list(std::uintptr_t a1, uint64_t game_assembly) {
    std::uint32_t r8d = 0, eax = 0, ecx = 0, edx = 0; // initialize r8d, eax, ecx, edx
    std::uintptr_t rax = mem.read<std::uintptr_t >(a1 + 0x18); // rax = *(_QWORD *)(a1 + 0x18)
    std::uintptr_t* rdx = &rax; // rdx points to rax
    // unhandled: push rbx
    r8d = 0x02; // mov r8d, 0x02
    do { // start of loop
        ecx = *(std::uint32_t*)(rdx); // mov ecx, [rdx]
        rdx = (std::uintptr_t*)((char*)rdx + 0x04); // lea rdx, [rdx+0x04]
        ecx ^= 0x5923674C; // xor ecx, 0x5923674C
        eax = ecx; // mov eax, ecx
        ecx <<= 0x06; // shl ecx, 0x06
        eax >>= 0x1A; // shr eax, 0x1A
        eax |= ecx; // or eax, ecx
        eax += 0xA293B27A; // add eax, 0xA293B27A
        ecx = eax; // mov ecx, eax
        eax <<= 0x18; // shl eax, 0x18
        ecx >>= 0x08; // shr ecx, 0x08
        ecx |= eax; // or ecx, eax
        *((std::uint32_t*)rdx - 1) = ecx; // mov [rdx-0x04], ecx
        r8d -= 0x01; // sub r8d, 0x01
    } while (r8d); // end of loop

    return il2cpp_get_handle(game_assembly, rax); // return il2cpp handle
}

struct vec3
{
    float x, y, z;

    vec3() : x(0), y(0), z(0) {}  // Add this line
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    inline float operator[](int i) const {
        return ((float*)this)[i];
    }

    inline vec3& operator-=(float v) {
        x -= v; y -= v; z -= v; return *this;
    }

    inline vec3 operator*(float v) const {
        return vec3(x * v, y * v, z * v);
    }

    inline vec3 operator/(float v) const
    {
        return vec3(x / v, y / v, z / v);
    }

    inline vec3& operator+=(const vec3& v) {
        x += v.x; y += v.y; z += v.z; return *this;
    }

    inline vec3 operator-(const vec3& v) const {
        return vec3(x - v.x, y - v.y, z - v.z);
    }

    inline vec3 operator+(const vec3& v) const {
        return vec3(x + v.x, y + v.y, z + v.z);
    }
};

struct vec2
{
    float x, y;

    vec2() : x(0), y(0) {}  // Add this line
    vec2(float x, float y) : x(x), y(y) {}
};

class Vector4 {
public:
    float x, y, z, w;
};

struct Matrix3x4
{
    Vector4 vec0;
    Vector4 vec1;
    Vector4 vec2;
};

struct Matrix34 {
    Vector4 vec0;
    Vector4 vec1;
    Vector4 vec2;
};

struct Matrix4x4 {
    union {
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

        }; float m[4][4];
    };
};


enum BoneList : int
{
    l_hip = 1,
    l_knee,
    l_foot,
    l_toe,
    l_ankle_scale,
    pelvis,
    penis,
    GenitalCensor,
    GenitalCensor_LOD0,
    Inner_LOD0,
    GenitalCensor_LOD1,
    GenitalCensor_LOD2,
    r_hip,
    r_knee,
    r_foot,
    r_toe,
    r_ankle_scale,
    spine1,
    spine1_scale,
    spine2,
    spine3,
    spine4,
    l_clavicle,
    l_upperarm,
    l_forearm,
    l_hand,
    l_index1,
    l_index2,
    l_index3,
    l_little1,
    l_little2,
    l_little3,
    l_middle1,
    l_middle2,
    l_middle3,
    l_prop,
    l_ring1,
    l_ring2,
    l_ring3,
    l_thumb1,
    l_thumb2,
    l_thumb3,
    IKtarget_righthand_min,
    IKtarget_righthand_max,
    l_ulna,
    neck,
    head,
    jaw,
    eyeTranform,
    l_eye,
    l_Eyelid,
    r_eye,
    r_Eyelid,
    r_clavicle,
    r_upperarm,
    r_forearm,
    r_hand,
    r_index1,
    r_index2,
    r_index3,
    r_little1,
    r_little2,
    r_little3,
    r_middle1,
    r_middle2,
    r_middle3,
    r_prop,
    r_ring1,
    r_ring2,
    r_ring3,
    r_thumb1,
    r_thumb2,
    r_thumb3,
    IKtarget_lefthand_min,
    IKtarget_lefthand_max,
    r_ulna,
    l_breast,
    r_breast,
    BoobCensor,
    BreastCensor_LOD0,
    BreastCensor_LOD1,
    BreastCensor_LOD2,
    collision,
    displacement
};

std::vector<std::pair<BoneList, BoneList>> SkeletonPairs = {
    // Spine
    { spine1, pelvis },
    { spine2, spine1 },
    { spine3, spine2 },
    { spine4, spine3 },
    { neck, spine4 },

    // Left leg
    { l_hip, pelvis },
    { l_knee, l_hip },
    { l_foot, l_knee },
    { l_ankle_scale, l_foot },
    { l_toe, l_foot },

    // Right leg
    { r_hip, pelvis },
    { r_knee, r_hip },
    { r_foot, r_knee },
    { r_ankle_scale, r_foot },
    { r_toe, r_foot },

    // Left arm
    { l_clavicle, spine4 },
    { l_upperarm, l_clavicle },
    { l_forearm, l_upperarm },
    { l_hand, l_forearm },

    // Left fingers
    { l_thumb1, l_hand }, { l_thumb2, l_thumb1 }, { l_thumb3, l_thumb2 },
    { l_index1, l_hand }, { l_index2, l_index1 }, { l_index3, l_index2 },
    { l_middle1, l_hand }, { l_middle2, l_middle1 }, { l_middle3, l_middle2 },
    { l_ring1, l_hand }, { l_ring2, l_ring1 }, { l_ring3, l_ring2 },
    { l_little1, l_hand }, { l_little2, l_little1 }, { l_little3, l_little2 },

    // Right arm
    { r_clavicle, spine4 },
    { r_upperarm, r_clavicle },
    { r_forearm, r_upperarm },
    { r_hand, r_forearm },

    // Right fingers
    { r_thumb1, r_hand }, { r_thumb2, r_thumb1 }, { r_thumb3, r_thumb2 },
    { r_index1, r_hand }, { r_index2, r_index1 }, { r_index3, r_index2 },
    { r_middle1, r_hand }, { r_middle2, r_middle1 }, { r_middle3, r_middle2 },
    { r_ring1, r_hand }, { r_ring2, r_ring1 }, { r_ring3, r_ring2 },
    { r_little1, r_hand }, { r_little2, r_little1 }, { r_little3, r_little2 },
};

vec3 GetTransformPosition(uintptr_t transform) {
    if (!transform) return vec3();
    int index = mem.read<int>(transform + 0x40);
    uintptr_t pTransformData = mem.read<uintptr_t>(transform + 0x38);
    if (!pTransformData) return vec3();
    uintptr_t matrixBase = mem.read<uintptr_t>(pTransformData + 0x18);
    uintptr_t indexBase = mem.read<uintptr_t>(pTransformData + 0x20);
    if (!matrixBase || !indexBase) return vec3();
    __m128 result = mem.read<__m128>(matrixBase + 0x30 * index);
    int transformIndex = mem.read<int>(indexBase + 0x4 * index);
    while (transformIndex >= 0) {
        struct Matrix34 { BYTE vec0[16]; BYTE vec1[16]; BYTE vec2[16]; };
        Matrix34 matrix34 = mem.read<Matrix34>(matrixBase + 0x30 * transformIndex);
        const __m128 mulVec0 = { -2.f,  2.f, -2.f, 0.f };
        const __m128 mulVec1 = { 2.f, -2.f, -2.f, 0.f };
        const __m128 mulVec2 = { -2.f, -2.f,  2.f, 0.f };
        __m128 vec1 = *reinterpret_cast<const __m128*>(&matrix34.vec1);
        __m128 vec2 = *reinterpret_cast<const __m128*>(&matrix34.vec2);
        __m128 vec0 = *reinterpret_cast<const __m128*>(&matrix34.vec0);
        __m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0x00));
        __m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0x55));
        __m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0x8E));
        __m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0xDB));
        __m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0xAA));
        __m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec1), 0x71));
        __m128 tmp7 = _mm_mul_ps(vec2, result);
        result = _mm_add_ps(
            _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(
                        _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy), _mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
                    _mm_mul_ps(
                        _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw), _mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
                _mm_add_ps(
                    _mm_mul_ps(
                        _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy), _mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
                    tmp7)),
            vec0);
        transformIndex = mem.read<int>(indexBase + 0x4 * transformIndex);
    }
    return vec3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
}

vec3 GetBonePosition(uintptr_t player, int boneIndex) {
    uintptr_t playerModel = mem.read<uintptr_t>(player + 0xd0);
    uintptr_t boneTransforms = mem.read<uintptr_t>(playerModel + 0x50);
    uintptr_t entityBone = mem.read<uintptr_t>(boneTransforms + 0x20 + boneIndex * 0x8);
    uintptr_t transform = mem.read<uintptr_t>(entityBone + 0x10);
    return GetTransformPosition(transform);
}


Matrix4x4 Transpose(const Matrix4x4& mat) {
    Matrix4x4 t;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            t.m[i][j] = mat.m[j][i];
    return t;
}

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);


vec2 w2s(vec3 world_pos, Matrix4x4 view_matrix)
{
    // w calculation: dot column 4 with position
    float w = view_matrix._14 * world_pos.x +
        view_matrix._24 * world_pos.y +
        view_matrix._34 * world_pos.z +
        view_matrix._44;

    if (w < 0.097f)
        return vec2(-999, -999);

    // x calculation: dot column 1 with position
    float x = view_matrix._11 * world_pos.x +
        view_matrix._21 * world_pos.y +
        view_matrix._31 * world_pos.z +
        view_matrix._41;

    // y calculation: dot column 2 with position  
    float y = view_matrix._12 * world_pos.x +
        view_matrix._22 * world_pos.y +
        view_matrix._32 * world_pos.z +
        view_matrix._42;

    vec2 screen;
    screen.x = (screenWidth * 0.5f) * (1.0f + x / w);
    screen.y = (screenHeight * 0.5f) * (1.0f - y / w);

    return screen;
}

int main()
{
    std::wcout << L"[+] Setting up for RustClient.exe...\n";
    if (!mem.setup(L"RustClient.exe"))
    {
        std::wcout << L"[-] setup failed\n";
        return 1;
    }

    auto cr3 = mem.GetCR3();
    std::wcout << L"[+] CR3: 0x" << std::hex << cr3 << std::dec << L"\n";

    auto game_base = mem.GetBase();
    std::wcout << L"[+] Base address: 0x" << std::hex << game_base << std::dec << L"\n";

    uintptr_t game_assembly = mem.get_module(L"GameAssembly.dll");
    if (!game_assembly)
        std::wcout << L"[-] Failed to get GameAssembly.dll\n";
    else
        std::wcout << L"[+] GameAssembly.dll: 0x"<< std::hex << game_assembly << std::dec << L"\n";

    Overlay ovr;
    if (!ovr.Initialize())
        return -67;

    bool WindowFocus = false;
    while (!WindowFocus) {
        DWORD ForegroundWindowProcessID;
        GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
        if (mem.Pid() == ForegroundWindowProcessID) {
            WindowFocus = true;
        }
    }

    while (true)
    {
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

            uintptr_t camera_class = mem.read<uintptr_t>(game_assembly + 0xD1D6830);
            std::wcout << L"[+] Camera Class: 0x" << std::hex << camera_class << std::dec << L"\n";

            uintptr_t cam_static_field = mem.read<uintptr_t>(camera_class + 0xb8);
            std::wcout << L"[+] Camera Static Fields: 0x" << std::hex << cam_static_field << std::dec << L"\n";

            uintptr_t cam_obj = mem.read<uintptr_t>(cam_static_field + 0x50); // wrapper class
            std::wcout << L"[+] Camera Object (Wrapper): 0x" << std::hex << cam_obj << std::dec << L"\n";

            uintptr_t cam_parent = mem.read<uintptr_t>(cam_obj + 0x10);
            std::wcout << L"[+] Camera Parent: 0x" << std::hex << cam_parent << std::dec << L"\n";

            Matrix4x4 view_matrix = mem.read<Matrix4x4>(cam_parent + 0x30C);
            std::wcout << L"[+] View Matrix Address: 0x" << std::hex << (cam_parent + 0x30C) << std::dec << L"\n";


            uintptr_t base_networkable = mem.read<uintptr_t>(game_assembly + 0xd1f19b0);
            std::wcout << L"[+] Base Networkable: 0x" << std::hex << base_networkable << std::dec << L"\n";

            uintptr_t static_feilds = mem.read<uintptr_t>(base_networkable + 0xb8);
            std::wcout << L"[+] Static Feilds: 0x" << std::hex << static_feilds << std::dec << L"\n";

            uintptr_t entity_ptr = mem.read<uintptr_t>(static_feilds + 0x30);
            std::wcout << L"[+] Entity Ptr: 0x" << std::hex << entity_ptr << std::dec << L"\n";

            uintptr_t client_entity = client_entities(entity_ptr, game_assembly);
            std::wcout << L"[+] Client Entity: 0x" << std::hex << client_entity << std::dec << L"\n";

            uintptr_t entity_realm = mem.read<uintptr_t>(client_entity + 0x10);
            std::wcout << L"[+] Entity Realm: 0x" << std::hex << entity_realm << std::dec << L"\n";

            uintptr_t entity_list = get_entity_list(entity_realm, game_assembly);
            std::wcout << L"[+] Entity List: 0x" << std::hex << entity_list << std::dec << L"\n";

            uintptr_t buffer_list = mem.read<uintptr_t>(entity_list + 0x18);
            std::wcout << L"[+] Buffer List: 0x" << std::hex << buffer_list << std::dec << L"\n";

            int entity_count = mem.read<int>(buffer_list + 0x18);
            std::wcout << L"[+] Entity Count: " << entity_count << std::dec << L"\n";

            uintptr_t entity_array = mem.read<uintptr_t>(buffer_list + 0x10);
            std::wcout << L"[+] Entity Array: 0x" << std::hex << entity_array << std::dec << L"\n";

            for (int x = 0; x < entity_count; ++x)
            {
                if (x == 0)
                    continue;

                uintptr_t pawn = mem.read<uint64_t>((uint64_t)entity_array + 0x20 + (x * 0x8));
                if (!pawn)
                    continue;

                uint32_t prefab_id = mem.read<uint32_t>(pawn + 0x30);
                if (prefab_id != 4108440852ULL)
                    continue;

                vec3 world = mem.read_chain<vec3>((PVOID)pawn, { 0x10, 0x30, 0x30, 0x8, 0x38, 0x90 });
                vec3 head = GetBonePosition(pawn, BoneList::head);
                std::wcout << L"[+] pawn[" << x << "] head position: (" << head.x << L", " << head.y << L", " << head.z << L")\n";
                world.x += 0.1f;

                vec2 worldpos = w2s(world, view_matrix);
                vec2 headpos = w2s(head, view_matrix);

                draw_list->AddText(ImVec2(headpos.x, headpos.y), ImColor(255, 255, 0), "player");

                for (const auto& [from, to] : SkeletonPairs)
                {
                    vec2 a = w2s(GetBonePosition(pawn, from), view_matrix);
                    vec2 b = w2s(GetBonePosition(pawn, to), view_matrix);
                    draw_list->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, b.y), ImColor(255,255,0));
                }


                std::wcout << L"[+] pawn[" << x << "]: 0x" << std::hex << pawn << std::dec << L"\n";
                std::wcout << L"[+] pawn[" << x << "] prefab id: " << prefab_id << std::dec << L"\n";
                std::wcout << L"[+] pawn[" << x << "] world position: (" << world.x << L", " << world.y << L", " << world.z << L")\n";
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

    while (true)
    {
       
    }
    return 0;
}
