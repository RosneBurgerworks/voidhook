/*
 * macros.hpp
 *
 *  Created on: May 25, 2017
 *      Author: nullifiedcat
 */

#pragma once

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

/*
 * make -j4 -e GAME=tf2
 */

// http://stackoverflow.com/questions/2335888/how-to-compare-string-in-c-conditional-preprocessor-directives
constexpr int c_strcmp( char const* lhs, char const* rhs ) {
    return (('\0' == lhs[0]) && ('\0' == rhs[0])) ? 0
        :  (lhs[0] != rhs[0]) ? (lhs[0] - rhs[0])
        : c_strcmp( lhs+1, rhs+1 );
}

#ifndef DATA_PATH
#	define DATA_PATH "/opt/cathook"
#endif

#if RENDERING_ENGINE_XOVERLAY

#define FEATURE_EMOJI_ESP_DISABLED
#define FEATURE_RADAR_DISABLED
#define FEATURE_FIDGET_SPINNER_DISABLED

#endif
