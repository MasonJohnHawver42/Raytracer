#include "math/common.h"

float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); } //6t^5 - 15t^4 + 10t^3 approx sigmoid function
float lerp(float t, float a, float b) { return a + t * (b - a); }
float clamp(float t, float a, float b) { return (t > b) ? b : ((t < a) ? a : t); }
