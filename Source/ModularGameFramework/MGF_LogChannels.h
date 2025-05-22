#pragma once

#include "CoreMinimal.h"


#if UE_BUILD_SHIPPING
DECLARE_LOG_CATEGORY_EXTERN(LogModularGameFramework, Display, Display);
#elif UE_BUILD_TEST
DECLARE_LOG_CATEGORY_EXTERN(LogModularGameFramework, Display, Log);
#else // UE_BUILD_DEVELOPMENT ou UE_BUILD_DEBUG
DECLARE_LOG_CATEGORY_EXTERN(LogModularGameFramework, Log, All);
#endif