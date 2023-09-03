execute_process(
    COMMAND luarocks.bat config sysconfdir
    OUTPUT_VARIABLE LUAROCKS_SYSCONFDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

configure_file(luarocks${EXTNAME}.in luarocks${EXTNAME} @ONLY)
configure_file(lua${EXTNAME}.in lua${EXTNAME} @ONLY)
