//
//  Utils.h
//  MapEditorSample
//
//  Created by kondo on 2014/07/30.
//
//

#ifndef MapEditorSample_Utils_h
#define MapEditorSample_Utils_h

#include <math.h>
#include <random>
#include <string>
#include <map>
#include <list>
#include <cassert>

#include "cocos2d.h"


namespace Utils
{
    //-------------------------- Constants ------------------------------
    //
    //  Various Constants
    //-------------------------------------------------------------------
    const int     MAX_INT    = (std::numeric_limits<int>::max)();
    const float   MAX_FLOAT  = (std::numeric_limits<float>::max)();
    const float   MIN_FLOAT  = (std::numeric_limits<float>::min)();
    
    
    
    //----------------------------- Pi ----------------------------------
    //
    //  Various Pi
    //-------------------------------------------------------------------
    const float PI              = 3.1416;
    const float TWO_PI          = PI * 2;
    const float HALF_PI         = PI / 2;
    const float THREE_HALVES_PI = PI * 1.5f;
    const float QUARTER_PI      = PI / 4;
    
    inline float degreeToRadian(float degree)
    {
        return TWO_PI * (degree / 360.0f);
    }
    
    
    
    //------------------------ MathUtils --------------------------------
    //
    //  Math method collection
    //-------------------------------------------------------------------
    template<class T>
    T maximum(const T &x, const T &y)
    {
        return x > y ? x : y;
    }
    
    
    inline float sigmoid(float x, float response = 1.0f)
    {
        return (1.0f / (1.0f / exp(-x / response)));
    }
    
    
    template<class T>
    inline T maxOf(const T &a, const T &b)
    {
        if(a > b) return a;
        return b;
    }
    
    
    template<class T>
    inline T minOf(const T &a, const T &b)
    {
        if(a < b) return a;
        return b;
    }
    
    
    template<class T_0, class T_1, class T_2>
    inline void clamp(T_0 &x, const T_1 &min, const T_2 &max)
    {
        assert((min < max) && "<Utils::clamp> max < min");
        
        T_0 minCasted = static_cast<T_0>(min);
        
        if(x < minCasted)
        {
            x = minCasted;
        }
        
        T_0 maxCasted = static_cast<T_0>(max);
        
        if(x > maxCasted)
        {
            x = maxCasted;
        }
    }
    
    
    inline bool isEqual(float a, float b)
    {
        float diff = a - b;
        return (diff < 1e-5) && (-diff < 1e-5);
    }
    
    
    inline bool isZero(float a)
    {
        return isEqual(a, 0.0f);
    }
    
    
    inline float roundDecimals(float a, int n)
    {
        float a_pow_n = a * powf(10.0f, n);
        float a_pow_n_round = roundf(a_pow_n);
        return a_pow_n_round * powf(10.0f, -n);
    }
    
    
    inline std::string decimalsToString(float a, int n)
    {
        int a_int = static_cast<int>(a);
        int a_decimals = static_cast<int>((a - a_int) * pow(10.0f, n));
        return std::to_string(a_int) + "." + std::to_string(a_decimals);
    }
    
    
    //--------------------- RandomNumber --------------------------------
    //
    //  Generate a random number
    //-------------------------------------------------------------------
    static std::random_device rd;
    static std::mt19937 mt(rd());
    
    //returns a random integer between min and max
    inline float randIntInRange(int min, int max)
    {
        if(min == max) return min;
        std::uniform_int_distribution<int> dice(min, max);
        return dice(mt);
    }
    
    
    //returns a random float between 0 and 1
    inline float randFloat()
    {
        std::uniform_real_distribution<float> dice(0.0f, 1.0f);
        return dice(mt);
    }
    
    
    //returns a random float between min and max
    inline float randFloatInRange(float min, float max)
    {
        if(isEqual(min, max)) return min;
        std::uniform_real_distribution<float> dice(min, max);
        return dice(mt);
    }
    
    
    //returns a random bool
    inline bool randBool()
    {
        if(randFloat() > 0.5)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    
    //returns a random float between -1 and 1
    inline float randClamped()
    {
        return randFloat() - randFloat();
    }
}

#endif
