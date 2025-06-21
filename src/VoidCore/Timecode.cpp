/* APPLE */
#ifdef __APPLE__
#include <arm_neon.h>
#endif // __APPLE__

#if defined(__SSE2__)
#include <smmintrin.h>
#endif // __SSE2__

/* STD */
#include <iomanip> // for std::setfill
#include <sstream>

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
    t.minutes = (frame / (framerate * 60)) - (t.hours * 60);
    t.seconds = (frame / framerate) - ((t.hours * 3600) - (t.hours * 60));
    t.frames = frame % framerate;

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

#if defined(__SSE2__)
inline __m128i _div_epi32(__m128i __a, __m128i __b)
{
    /**
     * Cast the __m128i to __m128 (float)
     * perform the division
     * Cast the result back to __m128i (integer)
     * return the casted value
     */
    return _mm_cvtps_epi32(_mm_div_ps(_mm_cvtepi32_ps(__a), _mm_cvtepi32_ps(__b)));

}
#endif // defined(__SSE2__)

Timecode VoidTC::GetTimecodeSSE2(const int frame, const int framerate)
{
    Timecode t;
    t.dropframe = false;

    #if defined(__SSE2__)
    /* Vectors */
    __m128i vframe = _mm_set1_epi32(frame);
    __m128i vframerate = _mm_set1_epi32(framerate);

    /* Varied framerates */
    __m128i v60 = _mm_set1_epi32(60);
    __m128i vframerate3600 = _mm_mullo_epi32(vframerate, _mm_set1_epi32(3600)); // framerate x 3600
    __m128i vframerate60 = _mm_mullo_epi32(vframerate, _mm_set1_epi32(60)); // framerate x 60

    // Hours (frame / (framerate * 3600))
    __m128i hours = _div_epi32(vframe, vframerate3600);

    // Minutes ((frame / (framerate * 60)) - (hours * 60))
    __m128i fmins = _div_epi32(vframe, vframerate60);
    // __m128i minutes = _mm_sub_epi32(fmins, _mm_mullo_epi32(_div_epi32(fmins, v60), v60));
    __m128i minutes = _mm_sub_epi32(fmins, _mm_mullo_epi32(hours, v60));

    // Seconds ((frame / framerate) - ((hours * 3600) - (hours * 60))
    __m128i fsecs = _div_epi32(vframe, vframerate);
    // __m128i seconds = _mm_sub_epi32(fsecs, _mm_mullo_epi32(_div_epi32(fsecs, v60), v60));
    __m128i seconds = _mm_sub_epi32(fsecs, _mm_sub_epi32(_mm_mullo_epi32(hours, _mm_set1_epi32(3600)), _mm_mullo_epi32(hours, _mm_set1_epi32(60))));

    // Remaining frames (frame % framerate)
    __m128i frames = _mm_sub_epi32(vframe, _mm_mullo_epi32(_div_epi32(vframe, vframerate), vframerate));

    /* Update the struct wit values */
    t.hours = _mm_cvtsi128_si32(hours);
    t.minutes = _mm_cvtsi128_si32(minutes);
    t.seconds = _mm_cvtsi128_si32(seconds);
    t.frames = _mm_cvtsi128_si32(frames);
    #endif // defined(__SSE2__)

    return t;
}

std::string VoidTC::TimecodeString(const int frame, const int framerate)
{
    /* Get the timecode */
    Timecode t = GetTimecode(frame, framerate);

    std::stringstream sss;
    sss << std::setfill('0') << std::setw(2) << t.hours     << ":"
        << std::setfill('0') << std::setw(2) << t.minutes   << ":"
        << std::setfill('0') << std::setw(2) << t.seconds   << ":"
        << std::setfill('0') << std::setw(2) << t.frames;

    return sss.str();
}

VOID_NAMESPACE_CLOSE
