#ifndef ROUTING_ENGINE_H
#define ROUTING_ENGINE_H

#include "config.h"
#include <vector>

// Determine target languages based on routing mode. The hubLanguage parameter is used when mode==Hub.
std::vector<Language> determineTargets(Language from, Language to, RoutingMode mode, Language hubLanguage);

#endif // ROUTING_ENGINE_H
