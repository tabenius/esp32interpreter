#include "routing_engine.h"
#include "core_system.h"

std::vector<Language> determineTargets(Language from, Language to, RoutingMode mode, Language hubLanguage) {
  std::vector<Language> out;
  if (mode == RoutingMode::OneToOne) {
    out.push_back(to);
    return out;
  }

  // One-to-Many: return all supported languages except the source
  if (mode == RoutingMode::OneToMany) {
    for (int i = 0; i <= (int)Language::German; ++i) {
      Language L = (Language)i;
      if (L != from) out.push_back(L);
    }
    return out;
  }

  // Hub mode: translate to hub first (if needed) and then to all other languages
  if (mode == RoutingMode::Hub) {
    // If hubLanguage is same as source, skip intermediate
    if (hubLanguage != from) {
      // first translate to hub
      out.push_back(hubLanguage);
    }
    // then produce translations from hub to others (excluding hub and source)
    for (int i = 0; i <= (int)Language::German; ++i) {
      Language L = (Language)i;
      if (L != hubLanguage && L != from) out.push_back(L);
    }
    return out;
  }

  // default fallback
  out.push_back(to);
  return out;
}
