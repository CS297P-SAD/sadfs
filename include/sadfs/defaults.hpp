/* sadfs-wide defaults */

namespace sadfs { namespace defaults {

#ifdef __OpenBSD__
// max size of the queue of pending socket connections
inline constexpr int somaxconn{128};
#else // __linux__
inline constexpr int somaxconn{4096};
#endif

} // defaults
} // sadfs
