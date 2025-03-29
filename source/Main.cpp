#include "plugin.h"
#include "CFileMgr.h"
#include "Occlusion.h"
#include "debugmenu_public.h"

using namespace plugin;
DebugMenuAPI gDebugMenuAPI;

class OcclusionCull {
public:
    OcclusionCull() {
        static auto LoadOccluders = []
            {
                int fd;
                int start, end;
                int section = 0;
                static bool firstLoad = true;
                char line[1024];

                CFileMgr::ChangeDir("\\DATA\\");
                fd = CFileMgr::OpenFile("OCCLUSION.DAT", "r");
                CFileMgr::ChangeDir("\\");

                COcclusion::Init();
                while (CFileMgr::ReadLine(fd, line, sizeof(line)))
                {
                    // find first valid character in line
                    for (start = 0;; start++)
                        if (line[start] > ' ' || line[start] == '\0' || line[start] == '\n')
                            break;
                    // find end of line
                    for (end = start;; end++)
                    {
                        if (line[end] == '\0' || line[end] == '\n')
                            break;
                        if (line[end] == ',' || line[end] == '\r')
                            line[end] = ' ';
                    }

                    line[end] = '\0';

                    // empty line
                    if (line[start] == '#' || line[start] == '\0')
                        continue;

                    float x, y, z;
                    float width, length, height;
                    float angle;

                    sscanf(line, "%f %f %f %f %f %f %f",
                        &x, &y, &z,
                        &width, &length, &height,
                        &angle);
                    COcclusion::AddOne(x, y, z + height / 2.0f, width, length, height, angle);
                }
                CFileMgr::CloseFile(fd);
            };

        Events::initGameEvent.after += LoadOccluders;

        Events::initRwEvent.after += []
            {
                if (DebugMenuLoad())
                {
                    DebugMenuAddCmd("Occlusion Debug", "Reload", LoadOccluders);
                    DebugMenuAddVarBool8("Occlusion Debug", "Draw Occlusion", (int8_t*)&bDispayOccDebugStuff, NULL);
                    DebugMenuAddVarBool8("Occlusion Debug", "Flash Occlusion", (int8_t*)&bFlashOcclusion, NULL);
                    DebugMenuAddVarBool8("Occlusion Debug", "Disable Occlusion Cull", (int8_t*)&bDisableCull, NULL);
                }
            };
        
        CdeclEvent<AddressList<0x48E539, H_CALL>, PRIORITY_BEFORE, ArgPickNone, void(void)> ConstructRenderListEvent;
        ConstructRenderListEvent.AddBefore(COcclusion::ProcessBeforeRendering);

        injector::MakeInline<0x4A9472>([](injector::reg_pack& regs)
            {
                auto ent = (CEntity*)regs.ecx;
                if (!ent->GetIsOnScreen() || IsEntityOccluded(ent))
                    regs.ecx = 0x4A9511;
                else
                    regs.ecx = 0x4A9485;
            });
        injector::MakeNOP(0x4A9472 + 5, 2);
        patch::SetUChar(0x4A947F, 0xFF);
        patch::SetUChar(0x4A947F + 1, 0xE1);

        injector::MakeInline<0x4A9692>([](injector::reg_pack& regs)
            {
                auto ent = (CEntity*)regs.ecx;
                if (!ent->GetIsOnScreen() || IsEntityOccluded(ent))
                    regs.ecx = 0x4A9700;
                else
                    regs.ecx = 0x4A969B;
            });
        patch::SetUChar(0x4A9692 + 5, 0xFF);
        patch::SetUChar(0x4A9692 + 5 + 1, 0xE1);

        injector::MakeInline<0x4A97C2>([](injector::reg_pack& regs)
            {
                auto ent = (CEntity*)regs.ecx;
                if (!ent->GetIsOnScreen() || IsEntityOccluded(ent))
                    regs.ecx = 0x4A97E3;
                else
                    regs.ecx = 0x4A97CB;
            });
        patch::SetUChar(0x4A97C2 + 5, 0xFF);
        patch::SetUChar(0x4A97C2 + 5 + 1, 0xE1);

        injector::MakeInline<0x4A9AB3>([](injector::reg_pack& regs)
            {
                auto ent = (CEntity*)regs.ecx;
                if (!ent->IsVisible() || !ent->GetIsOnScreenComplex() || IsEntityOccluded(ent))
                    regs.ecx = 0x4A9B00;
                else
                    regs.ecx = 0x4A9AC7;
            });
        patch::SetUChar(0x4A9AB3 + 5, 0xFF);
        patch::SetUChar(0x4A9AB3 + 5 + 1, 0xE1);

        injector::MakeInline<0x4A9B5A>([](injector::reg_pack& regs)
            {
                auto ent = (CEntity*)regs.ecx;
                if (!ent->IsVisible() || !ent->GetIsOnScreenComplex() || IsEntityOccluded(ent))
                    regs.ecx = 0x4A9B7A;
                else
                    regs.ecx = 0x4A9B6E;
            });
        patch::SetUChar(0x4A9B5A + 5, 0xFF);
        patch::SetUChar(0x4A9B5A + 5 + 1, 0xE1);

        CdeclEvent<AddressList<0x48E443, H_CALL>, PRIORITY_BEFORE, ArgPickNone, void(void)> DrawFontsEvent;
        DrawFontsEvent.AddAfter(COcclusion::Render);
	};
} OcclusionCullPlugin;
