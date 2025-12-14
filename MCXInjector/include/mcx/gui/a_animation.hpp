#pragma once
#include "mcx_inject.hpp"
#include "mcx/lib/vfx.hpp"
#include <cstdint>
#include <cstddef>
#include <time.h>

#ifndef ENABLE_LOG_AANIMATION
#define ENABLE_LOG_AANIMATION 0
#endif

#ifndef NO_ERR
#define NO_ERR	0
#endif

static constexpr float DEFAULT_FRAME_RATE = 15.0;

class aAnimation
{
public:
	int32_t curr_frame; // 0x0
	int32_t num_frames; //0x4
	void* shape_table; // 0x8
	float frame_rate; // 0xc
	int32_t width; // 0x10
	int32_t height; // 0x14
	clock_t prev_time; // 0x18
	clock_t last_time; // 0x1c

public:
	aAnimation();

	~aAnimation();

	// public: long __thiscall init(char *filename)
	long init(char* filename);

	// public: long __thiscall aAnimation::loadShape(char *)
	long loadShape(char* filename);

	// public: long __thiscall aAnimation::currentFrame(void)
	long currentFrame();

	// public: float __thiscall aAnimation::frameRate(void)
	float frameRate();

	// public: long __thiscall aAnimation::nextFrame(void)
	long nextFrame();

	//  public: long __thiscall aAnimation::numberOfFrames(void)
	long numberOfFrames();

	// public: void __thiscall aAnimation::setFrame(long)
	void setFrame(long new_frame);

	// public: void __thiscall aAnimation::setFrameRate(float)
	void setFrameRate(float new_framerate);

	// public: void * __thiscall aAnimation::shapeTable(void)
	void* shapeTable();

	// public: long __thiscall aAnimation::width(void)
	long getWidth();

	// public: long __thiscall aAnimation::height(void)
	long getHeight();

	// public: void __thiscall aAnimation::draw(struct _pane *,long,long)
	void draw(_pane* pane, long x, long y);

	// void __thiscall aAnimation::drawFrame(long,struct _pane *,long,long)
	void drawFrame(_pane* pane, long x, long y);

	// void __thiscall aAnimation::reset(); // FUN_006094e0
	void reset();
};

#pragma region VAs
static constexpr uintptr_t va_aAnimation_ctor = 0x00609450;
static constexpr uintptr_t va_aAnimation_dtor = 0x00609470;
static constexpr uintptr_t va_aAnimation_init = 0x00609490;
static constexpr uintptr_t va_aAnimation_shapeTable = 0x00609520;
static constexpr uintptr_t va_aAnimation_loadShape = 0x00609530;
static constexpr uintptr_t va_aAnimation_drawFrame = 0x006096b0;
static constexpr uintptr_t va_aAnimation_draw = 0x006096e0;
static constexpr uintptr_t va_aAnimation_width = 0x00609690;
static constexpr uintptr_t va_aAnimation_currentFrame = 0x00609790;
static constexpr uintptr_t va_aAnimation_nextFrame = 0x00609750;
static constexpr uintptr_t va_aAnimation_setFrame = 0x00609770;
static constexpr uintptr_t va_aAnimation_frameRate = 0x006097b0;
static constexpr uintptr_t va_aAnimation_setFrameRate = 0x00609780;
static constexpr uintptr_t va_aAnimation_numberOfFrames = 0x006097a0;
static constexpr uintptr_t va_aAnimation_reset = 0x006094e0;
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(aAnimation, 0x20);
ASSERT_ALIGN(aAnimation, 0x4);

ASSERT_FIELD_OFFSET(aAnimation, curr_frame, 0x00);
ASSERT_FIELD_OFFSET(aAnimation, num_frames, 0x04);
ASSERT_FIELD_OFFSET(aAnimation, shape_table, 0x08);
ASSERT_FIELD_OFFSET(aAnimation, frame_rate, 0x0C);
ASSERT_FIELD_OFFSET(aAnimation, width, 0x10);
ASSERT_FIELD_OFFSET(aAnimation, height, 0x14);
ASSERT_FIELD_OFFSET(aAnimation, prev_time, 0x18);
ASSERT_FIELD_OFFSET(aAnimation, last_time, 0x1C);
#pragma endregion