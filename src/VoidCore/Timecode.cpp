/* APPLE */
#ifdef __APPLE__
#include <arm_neon.h>
#endif // __APPLE__

/* Internal */
#include "Timecode.h"

VOID_NAMESPACE_OPEN

Timecode VoidTC::GetTimecode(const int frame, const int framerate)
{
    /**
     * The timecode of any frame is made up of HH (hours)
     * MM (minutes)
     * SS (seconds)
     * FF (remaining frames)
     */
    Timecode t;
    // Setting this as false for now
    t.dropframe = false;

    t.hours = frame / (framerate * 3600);
    t.minutes = (frame / (framerate * 60)) % 60;
    t.seconds = (frame / framerate) % 60;
    t.frames = frame / framerate;

    return t;
}

#ifdef __APPLE__
int32x2_t _div_int32x2(int32x2_t __a, int32x2_t __b)
{
    /* Convert inputs to float */
    float32x2_t fa = vcvt_f32_s32(__a);
    float32x2_t fb = vcvt_f32_s32(__b);

    /* perform the division */
    float32x2_t r = vdiv_f32(fa, fb);

    /* Return the casted int32x2_t back */
    return vcvt_s32_f32(r);
}
#endif // __APPLE__

Timecode VoidTC::GetTimecodeNeon(const int frame, const int framerate)
{
    Timecode t;
    #ifdef __APPLE__
    int32x2_t vframe = vdup_n_s32(frame);
    int32x2_t vframerate = vdup_n_s32(framerate);

    /* Varied framerates */
    int32x2_t vframerate3600 = vmul_n_s32(vframerate, 3600);
    int32x2_t vframerate60 = vmul_n_s32(vframerate, 60);
    int32x2_t v60 = vdup_n_s32(60);

    /* Calculate Variables */
    // Hours
    int32x2_t hours = _div_int32x2(vframe, vframerate3600);

    // Mins
    int32x2_t minsf = _div_int32x2(vframe, vframerate60);
    int32x2_t minutes = vsub_s32(minsf, vmul_s32(_div_int32x2(minsf, v60), v60));

    // Seconds
    int32x2_t secsf = _div_int32x2(vframe, vframerate);
    int32x2_t seconds = vsub_s32(secsf, vmul_s32(_div_int32x2(secsf, v60), v60));

    // Frames
    int32x2_t frames = vsub_s32(vframe, vmul_s32(_div_int32x2(vframe, vframerate), vframerate));

    /* Update timecode values */
    vst1_lane_s32(&t.hours, hours, 0);
    vst1_lane_s32(&t.minutes, minues, 0);
    vst1_lane_s32(&t.seconds, seconds, 0);
    vst1_lane_s32(&t.frames, frames, 0);

    #endif // __APPLE__

    return t;
}

VOID_NAMESPACE_CLOSE
