#ifndef LC_CONFIG_H
#define LC_CONFIG_H

#define __LC_NAMESPACE_BEGIN namespace lc {
#define __LC_NAMESPACE_END };

#define __LC_TRY try
#define __LC_CATCH_ALL catch(...)
#define __LC_THROW(x) throw x
#define __LC_RETHROW throw
#define __LC_NOTHROW throw()
#define __LC_UNWIND(action) catch(...) { action; throw; }

#endif
