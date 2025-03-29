#pragma once

//constexpr size_t NUMOCCLUSIONVOLUMES = 2048;
constexpr size_t NUMACTIVEOCCLUDERS = 64;

constexpr float DefaultFOV = 70.0f;

#define SCREEN_WIDTH RsGlobal.screenWidth
#define SCREEN_HEIGHT RsGlobal.screenHeight
#define Abs std::abs
#define Max(a,b) ((a) > (b) ? (a) : (b))
#define Min(a,b) ((a) < (b) ? (a) : (b))

struct ActiveOccluderLine {
	CVector2D origin;
	CVector2D direction;
	float length; 
};

class CActiveOccluder {

public:
	ActiveOccluderLine lines[6];
	int32_t linesCount;
	float radius; 

	bool IsPointWithinOcclusionArea(float x, float y, float area);
};

class COccluder
{
public:
	int16_t length, width, height;
	int16_t x, y, z;
	uint16_t angle;
	int16_t listIndex;

	bool NearCamera();
	bool ProcessOneOccluder(CActiveOccluder *occl);
	bool ProcessLineSegment(int corner1, int corner2, CActiveOccluder* occl);
	float GetAngle(void) { return angle*TWOPI/UINT16_MAX; }
};

class COcclusion
{
public:
	static int32_t NumOccludersOnMap;
	static int32_t FarAwayList;
	static int16_t NearbyList;
	static int16_t ListWalkThroughFA;
	static int16_t PreviousListWalkThroughFA;
	static int16_t NumActiveOccluders;

	//static COccluder aOccluders[NUMOCCLUSIONVOLUMES];
	static std::vector<COccluder> aOccluders;
	static CActiveOccluder aActiveOccluders[NUMACTIVEOCCLUDERS];

	static void Init(void);
	static void AddOne(float x, float y, float z, float width, float length, float height, float angle);
	static void ProcessBeforeRendering(void); 
	static bool OccluderHidesBehind(CActiveOccluder *occl1, CActiveOccluder *occl2);
	static bool IsAABoxOccluded(CVector pos, float width, float length, float height);
	static bool IsPositionOccluded(CVector pos, float side);
	static void Render();
};

bool CalcScreenCoors(CVector const &in, CVector *out, float *outw, float *outh);
bool CalcScreenCoors(CVector const &in, CVector *out);
bool IsEntityOccluded(CEntity* ent);

extern bool bDispayOccDebugStuff;
extern bool bFlashOcclusion;
extern bool bDisableCull;