file(GLOB mp3_src
    "*.h"
    "*.c"
    "html/*.h"
    "html/*.c"
    "yuarel/*.h"
    "yuarel/*.c"
    )

add_executable(mp3 ${mp3_src} )
